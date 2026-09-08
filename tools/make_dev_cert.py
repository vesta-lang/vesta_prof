#!/usr/bin/env python3
"""Prepara la firma del driver en desarrollo: genera el material y monta la maquina.

Por que existe
==============
Un driver no se carga sin firma.  En produccion eso significa un certificado EV
y la firma por atestacion del Partner Center de Microsoft; en desarrollo se usa
un certificado propio y se pone la maquina en modo de pruebas.

Son cinco ordenes encadenadas, y saltarse una no da un error claro: da un driver
que sencillamente no carga, con un codigo que no dice cual de los cinco pasos
falto.  De ahi que esto exista.

ESTO NO ES UN CAMINO DE PUBLICACION.  Un certificado autofirmado solo sirve con
el modo de pruebas activado, que es una maquina de desarrollo con las
comprobaciones de firma relajadas.  No se envia nada firmado asi.

Que produce
===========
En Windows:
    dev_cert.pfx         el par completo, para firmar (con contrasena)
    dev_cert.pfx.pass    la contrasena
    dev_cert.cer         solo la parte publica, para instalar en la maquina
    dev_cert.thumbprint  la huella, que es lo que identifica al certificado
                         en los almacenes cuando hay que quitarlo

En Linux:
    dev_key.pem          la clave privada
    dev_cert.pem         el certificado
    dev_cert.der         el mismo, en el formato que pide el kernel

Todo eso va a `profiler/.devcert/`, que esta ignorado por git.  Una clave
privada en un repositorio es una clave publicada, y da igual que sea "de
pruebas": lo que firma esa clave lo acepta cualquier maquina que la haya
confiado.

Uso
===
    python make_dev_cert.py                 solo generar
    python make_dev_cert.py --install       generar y montar (administrador)
    python make_dev_cert.py --uninstall     desmontar
    python make_dev_cert.py --uninstall --purge   y borrar el material
    python make_dev_cert.py --force         rehacer el material

En Windows es mas comodo `dev_setup.cmd`, que pide administrador el solo.
"""
import argparse
import os
import platform
import secrets
import shutil
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)                 # profiler/
OUT_DIR = os.path.join(ROOT, ".devcert")

# El nombre va con el proyecto para que, al mirar los certificados instalados en
# una maquina, se sepa de donde salio y se pueda quitar.
SUBJECT = "CN=Vesta Profiler (desarrollo), O=Vesta, OU=No apto para publicacion"


def note(msg):
    print("  " + msg)


def fail(msg):
    print("error: " + msg, file=sys.stderr)
    return 1


def powershell(script):
    """Ejecuta un fragmento de PowerShell y devuelve (codigo, salida)."""
    p = subprocess.run(["powershell", "-NoProfile", "-NonInteractive",
                        "-Command", script],
                       capture_output=True, text=True)
    return (p.returncode, (p.stdout or "") + (p.stderr or ""))


def run_step(cmd, tolerate_failure=False):
    """Ejecuta una orden y cuenta que hizo.  Devuelve True si salio bien.

    Se imprime cada paso porque esto toca la configuracion del equipo: quien lo
    lanza tiene derecho a ver que se le hizo, sobre todo si algo falla a medias.
    """
    p = subprocess.run(cmd, capture_output=True, text=True)
    ok = (p.returncode == 0)
    note(("ok   " if ok else "FALLO") + "  " + " ".join(
        c if " " not in c else '"%s"' % c for c in cmd))
    if not ok and not tolerate_failure:
        output = ((p.stdout or "") + (p.stderr or "")).strip()
        for line in output.splitlines()[:4]:
            note("       " + line)
    return ok


def is_admin():
    if os.name != "nt":
        return os.geteuid() == 0
    try:
        import ctypes
        return bool(ctypes.windll.shell32.IsUserAnAdmin())
    except Exception:
        return False


def read_thumbprint():
    path = os.path.join(OUT_DIR, "dev_cert.thumbprint")
    if os.path.exists(path):
        with open(path) as f:
            return f.read().strip()
    return ""


def find_signtool():
    """Localiza el `signtool.exe` de LA ARQUITECTURA DE ESTA MAQUINA.

    No vale coger el primero que aparezca: el SDK instala uno por arquitectura
    -- x64, x86, arm64 -- y en un listado recursivo suele salir antes el de
    arm64, que en un equipo x64 no arranca.  El sintoma seria un fallo al firmar
    que no tiene nada que ver con la firma.
    """
    arch = platform.machine().lower()
    want = "arm64" if arch in ("arm64", "aarch64") else "x64"

    bases = [os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)"),
             os.environ.get("ProgramFiles", r"C:\Program Files")]
    candidates = []
    for base in bases:
        root = os.path.join(base, "Windows Kits", "10", "bin")
        if not os.path.isdir(root):
            continue
        for dirpath, _dirs, files in os.walk(root):
            if "signtool.exe" in files:
                candidates.append(os.path.join(dirpath, "signtool.exe"))

    # Version mas alta primero; dentro de eso, la arquitectura que toca.
    candidates.sort(reverse=True)
    for c in candidates:
        if os.sep + want + os.sep in c:
            return c
    return candidates[0] if candidates else shutil.which("signtool")


def generate_windows(force):
    pfx = os.path.join(OUT_DIR, "dev_cert.pfx")
    cer = os.path.join(OUT_DIR, "dev_cert.cer")
    passfile = pfx + ".pass"

    if os.path.exists(pfx) and not force:
        return fail(
            "ya existe %s.\n"
            "       Rehacerlo INVALIDA lo que ya este firmado con el anterior, y\n"
            "       obliga a reinstalar el certificado en cada maquina que lo\n"
            "       hubiera confiado.  Si es lo que quieres: --force" % pfx)

    os.makedirs(OUT_DIR, exist_ok=True)

    # Contrasena al azar y guardada al lado.  No se pide por teclado porque
    # esto lo ejecuta un guion de construccion tanto como una persona; el
    # secreto lo protege el sistema de ficheros y el .gitignore, no la memoria
    # de quien lo teclee.
    password = secrets.token_urlsafe(24)

    # `-Type CodeSigningCert` mas el uso mejorado 1.3.6.1.5.5.7.3.3 (firma de
    # codigo): sin ese uso el certificado existe pero signtool lo rechaza.
    script = (
        "$ErrorActionPreference='Stop';"
        "$c = New-SelfSignedCertificate"
        " -Type CodeSigningCert"
        " -Subject '%s'"
        " -CertStoreLocation Cert:\\CurrentUser\\My"
        " -KeyUsage DigitalSignature"
        " -KeyExportPolicy Exportable"
        " -NotAfter (Get-Date).AddYears(5)"
        " -TextExtension @('2.5.29.37={text}1.3.6.1.5.5.7.3.3');"
        "$p = ConvertTo-SecureString -String '%s' -Force -AsPlainText;"
        "Export-PfxCertificate -Cert $c -FilePath '%s' -Password $p | Out-Null;"
        "Export-Certificate -Cert $c -FilePath '%s' | Out-Null;"
        "Write-Output $c.Thumbprint;"
    ) % (SUBJECT, password, pfx, cer)

    rc, output = powershell(script)
    if rc != 0 or not os.path.exists(pfx):
        return fail("no se pudo generar el certificado:\n" + output.strip())

    with open(passfile, "w") as f:
        f.write(password + "\n")

    thumbprint = output.strip().splitlines()[-1].strip() if output.strip() else "?"

    # La huella se guarda: es lo que identifica al certificado en los almacenes,
    # y el desmontaje la necesita cuando el .cer ya no este.
    with open(os.path.join(OUT_DIR, "dev_cert.thumbprint"), "w") as f:
        f.write(thumbprint + "\n")

    print("Generado en %s" % OUT_DIR)
    note("dev_cert.pfx       el par completo (firmar con esto)")
    note("dev_cert.pfx.pass  la contrasena")
    note("dev_cert.cer       la parte publica (instalar con esto)")
    note("huella: " + thumbprint)
    return 0


def install_windows():
    """Deja la maquina lista para cargar el driver de desarrollo.

    Lo que NO se hace es reiniciar.  El modo de pruebas no entra en vigor hasta
    el siguiente arranque, pero reiniciar por su cuenta le tiraria a alguien el
    trabajo que tuviera abierto.
    """
    cer = os.path.join(OUT_DIR, "dev_cert.cer")
    if not os.path.exists(cer):
        return fail("no hay material que instalar; genera primero")
    thumbprint = read_thumbprint()

    print()
    print("Preparando la maquina.  Esto cambia la politica del EQUIPO:")
    print()
    print("  - se confia un certificado propio para firmar codigo")
    print("  - se permite cargar drivers firmados de prueba")
    print()

    ok = True
    note("Confiando el certificado (dos almacenes: cadena y editor)")
    ok &= run_step(["certutil", "-addstore", "-f", "Root", cer])
    ok &= run_step(["certutil", "-addstore", "-f", "TrustedPublisher", cer])

    note("Permitiendo drivers de prueba")
    ok &= run_step(["bcdedit", "/set", "testsigning", "on"])

    print()
    if not ok:
        return fail("algo no se aplico; mira las lineas de arriba")

    print("Listo.  FALTA REINICIAR: el modo de pruebas no entra en vigor")
    print("hasta el siguiente arranque, y hasta entonces el driver seguira")
    print("sin cargar aunque este bien firmado.")
    print()
    print("Para firmar:")
    st = find_signtool()
    print('  "%s" sign /fd sha256 /f "%s" /p <la de .pass> driver.sys'
          % (st or "signtool", os.path.join(OUT_DIR, "dev_cert.pfx")))
    print()
    print("Para deshacerlo:  dev_teardown.cmd")
    if thumbprint:
        print("Huella: " + thumbprint)
    return 0


def uninstall_windows(purge_material):
    """Devuelve la maquina a como estaba.

    En orden inverso, y tolerando que algun paso no encuentre nada: quien
    ejecuta esto suele venir de un montaje a medias, y abortar en el primer
    hueco dejaria puesto todo lo demas.
    """
    thumbprint = read_thumbprint()
    if not thumbprint:
        return fail("no se sabe que certificado quitar: falta %s"
                    % os.path.join(OUT_DIR, "dev_cert.thumbprint"))

    print()
    print("Devolviendo la maquina a su estado normal.")
    print()
    note("Volviendo a exigir firma de produccion")
    run_step(["bcdedit", "/set", "testsigning", "off"], tolerate_failure=True)

    note("Dejando de confiar el certificado")
    run_step(["certutil", "-delstore", "Root", thumbprint],
             tolerate_failure=True)
    run_step(["certutil", "-delstore", "TrustedPublisher", thumbprint],
             tolerate_failure=True)
    powershell("Remove-Item -ErrorAction SilentlyContinue "
               "Cert:\\CurrentUser\\My\\%s" % thumbprint)
    note("ok     quitado del almacen personal")

    if purge_material:
        note("Borrando el material de firma")
        for name in ("dev_cert.pfx", "dev_cert.pfx.pass", "dev_cert.cer",
                     "dev_cert.thumbprint"):
            path = os.path.join(OUT_DIR, name)
            if os.path.exists(path):
                os.remove(path)
        note("ok     " + OUT_DIR)

    print()
    print("Hecho.  FALTA REINICIAR para que vuelva a exigirse la firma de")
    print("produccion; hasta entonces el equipo sigue admitiendo drivers de")
    print("prueba.")
    return 0


def generate_linux(force):
    key = os.path.join(OUT_DIR, "dev_key.pem")
    crt = os.path.join(OUT_DIR, "dev_cert.pem")
    der = os.path.join(OUT_DIR, "dev_cert.der")

    if os.path.exists(key) and not force:
        return fail(
            "ya existe %s.\n"
            "       Rehacerlo INVALIDA lo firmado con la clave anterior, y si la\n"
            "       inscribiste con mokutil hay que volver a hacerlo.  Si aun\n"
            "       asi quieres: --force" % key)

    if not shutil.which("openssl"):
        return fail("no hay openssl en el PATH")

    os.makedirs(OUT_DIR, exist_ok=True)

    # Sin contrasena en la clave: `scripts/sign-file` del kernel la lee sin
    # poder preguntar nada.  Lo que la protege es el sistema de ficheros.
    cmd = ["openssl", "req", "-new", "-x509", "-newkey", "rsa:4096",
           "-nodes", "-days", "1825",
           "-keyout", key, "-out", crt,
           "-subj", "/" + SUBJECT.replace(", ", "/")]
    p = subprocess.run(cmd, capture_output=True, text=True)
    if p.returncode != 0 or not os.path.exists(key):
        return fail("openssl fallo:\n" + (p.stderr or "").strip())

    # El kernel quiere el certificado en DER, no en PEM.
    p = subprocess.run(["openssl", "x509", "-in", crt, "-outform", "DER",
                        "-out", der], capture_output=True, text=True)
    if p.returncode != 0:
        return fail("no se pudo convertir a DER:\n" + (p.stderr or "").strip())

    try:
        os.chmod(key, 0o600)
    except OSError:
        pass

    print("Generado en %s" % OUT_DIR)
    note("dev_key.pem   la clave privada (permisos 600)")
    note("dev_cert.pem  el certificado")
    note("dev_cert.der  el mismo, como lo quiere el kernel")
    print()
    print("Lo que queda:")
    print()
    print("  1) Firmar el modulo, con el guion que trae el propio kernel:")
    print("     /usr/src/linux-headers-$(uname -r)/scripts/sign-file \\")
    print('         sha256 "%s" "%s" ruta/al/modulo.ko' % (key, crt))
    print()
    print("  2) Solo si hay Arranque Seguro: inscribir el certificado.  Pide")
    print("     una contrasena y se confirma en el arranque siguiente, en el")
    print("     menu azul del firmware -- si se pasa ese menu, no queda")
    print("     inscrito y el modulo seguira sin cargar.")
    print('     sudo mokutil --import "%s"' % der)
    print()
    print("  Sin Arranque Seguro no hace falta el paso 2: basta con que el")
    print("  kernel no exija firma, que es lo habitual en una distribucion de")
    print("  escritorio compilada sin CONFIG_MODULE_SIG_FORCE.")
    return 0


def main():
    ap = argparse.ArgumentParser(
        description="Prepara la firma del driver en desarrollo.")
    ap.add_argument("--platform", choices=("auto", "windows", "linux"),
                    default="auto",
                    help="para que sistema; por defecto, el de esta maquina")
    ap.add_argument("--force", action="store_true",
                    help="rehacer aunque ya exista (invalida lo ya firmado)")
    ap.add_argument("--install", action="store_true",
                    help="ademas, dejar la maquina lista (pide administrador)")
    ap.add_argument("--uninstall", action="store_true",
                    help="devolver la maquina a su estado normal")
    ap.add_argument("--purge", action="store_true",
                    help="con --uninstall, borrar tambien el material de firma")
    args = ap.parse_args()

    target = args.platform
    if target == "auto":
        target = "windows" if os.name == "nt" else "linux"

    if target != "windows":
        if args.install or args.uninstall:
            return fail("en Linux el montaje depende del Arranque Seguro y de\n"
                        "       si el kernel exige firma; se generan las claves\n"
                        "       y se indican los pasos, que no son los mismos\n"
                        "       en dos distribuciones")
        return generate_linux(args.force)

    if os.name != "nt":
        return fail("el certificado de Windows se genera en Windows: "
                    "New-SelfSignedCertificate es de PowerShell")

    if (args.install or args.uninstall) and not is_admin():
        return fail("esto cambia la politica del equipo y necesita\n"
                    "       administrador.  Usa dev_setup.cmd, que lo pide\n"
                    "       solo, o abre una consola como administrador.")

    if args.uninstall:
        return uninstall_windows(args.purge)

    have = os.path.exists(os.path.join(OUT_DIR, "dev_cert.pfx"))
    if not have or args.force:
        rc = generate_windows(args.force)
        if rc != 0:
            return rc
    elif not args.install:
        # Sin --install, encontrarselo hecho es lo que hay que decir.
        return generate_windows(args.force)

    if args.install:
        return install_windows()
    return 0


if __name__ == "__main__":
    sys.exit(main())
