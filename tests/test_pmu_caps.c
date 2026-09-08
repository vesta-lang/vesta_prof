/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE y NOTICE).
 */

/**
 * @file test_pmu_caps.c
 * @brief La deteccion del PMU, probada en espacio de usuario.
 *
 * Esto es lo que se compra con la regla de que `common/` no incluya cabeceras
 * del sistema: el MISMO codigo que va a correr dentro del kernel se ejecuta
 * aqui, con un `msr_read` de mentira, y se depura con un arnes en vez de
 * cargando un driver y mirando si la maquina se cuelga.
 *
 * COMO SE PRUEBA ALGO QUE DEPENDE DE LA MAQUINA.  CPUID no se puede falsear
 * desde aqui, asi que las comprobaciones son INVARIANTES y no valores: "si el
 * MSR que decide no se pudo leer, el veredicto tiene que ser UNKNOWN" vale en
 * cualquier procesador, mientras que "el veredicto es AVAILABLE" solo valdria
 * en el de quien lo escribio.  Un test que solo pasa en una maquina es peor que
 * ninguno, porque se cree.
 */

#include "pmu_caps.h"

#include "msr.h"

#include <stdio.h>

static int g_checks = 0, g_fails = 0;
#define CHECK(c)                                                               \
    do {                                                                       \
        ++g_checks;                                                            \
        if (!(c)) {                                                            \
            ++g_fails;                                                         \
            printf("  FAIL L%d: %s\n", __LINE__, #c);                          \
        }                                                                      \
    } while (0)

/* -------------------------------------------------------------------------
 *  El MSR de mentira.
 *
 *  No hay tabla de punteros a funcion -- eso es deliberado, ver `msr.h` --, asi
 *  que la sustitucion se hace en el ENLACE: este fichero define `msr_read` y el
 *  binario de prueba no enlaza el de Windows.  Es la misma seleccion en tiempo
 *  de enlace que usan los dos lados de kernel.
 * ------------------------------------------------------------------------- */

typedef struct fake_msr {
    int fault_everything; /**< todo devuelve ERR_FAULT, como en usuario */
    u64 misc_enable;
    u64 perf_capabilities;
    u64 debugctl;
    u64 fixed_ctr_ctrl;
    u64 perf_global_ctrl;
    u64 evtsel[8];
} fake_msr;

static fake_msr g_fake;

status msr_read(u32 index, u64 *out) {
    if (out == 0) {
        return ERR_INVALID;
    }
    if (g_fake.fault_everything) {
        return ERR_FAULT;
    }
    switch (index) {
    case IA32_MISC_ENABLE:
        *out = g_fake.misc_enable;
        return OK;
    case IA32_PERF_CAPABILITIES:
        *out = g_fake.perf_capabilities;
        return OK;
    case IA32_DEBUGCTL:
        *out = g_fake.debugctl;
        return OK;
    case IA32_FIXED_CTR_CTRL:
        *out = g_fake.fixed_ctr_ctrl;
        return OK;
    case IA32_PERF_GLOBAL_CTRL:
        *out = g_fake.perf_global_ctrl;
        return OK;
    default:
        if (index >= IA32_PERFEVTSEL0 &&
            index < IA32_PERFEVTSEL0 + 8u) {
            *out = g_fake.evtsel[index - IA32_PERFEVTSEL0];
            return OK;
        }
        /* Un MSR que el falso no conoce se comporta como uno inexistente: se
         * niega.  Devolver cero seria ensenar a la deteccion a fiarse de un
         * valor que no leyo nadie. */
        return ERR_FAULT;
    }
}

status msr_write(u32 index, u64 value) {
    (void)index;
    (void)value;
    return ERR_UNSUPPORTED;
}

/** @brief Deja el falso en un estado conocido antes de cada caso. */
static void fake_reset(void) {
    int i;
    g_fake.fault_everything = 0;
    g_fake.misc_enable = 0;
    g_fake.perf_capabilities = 0;
    g_fake.debugctl = 0;
    g_fake.fixed_ctr_ctrl = 0;
    g_fake.perf_global_ctrl = 0;
    for (i = 0; i < 8; ++i) {
        g_fake.evtsel[i] = 0;
    }
}

/** @brief Un selector con el bit `EN` puesto, como el que arma un contador. */
static u64 armed_evtsel(void) {
    return ((u64)1) << MSR_PERFEVTSEL_EN;
}

int main(void) {
    pmu_caps caps;
    char buf[4096];
    usize written = 0;

    printf("--- contract ---\n");
    {
        /* La frontera: un destino nulo se rechaza, no se escribe en el. */
        CHECK(pmu_caps_detect(0, 0) == ERR_INVALID);
        CHECK(pmu_caps_format(0, buf, sizeof(buf), &written) == ERR_INVALID);
    }

    printf("--- verdict: the deciding MSR cannot be read ---\n");
    {
        fake_reset();
        g_fake.fault_everything = 1;
        CHECK(pmu_caps_detect(7, &caps) == OK);
        CHECK(caps.cpu_index == 7);
        /* Sin IA32_MISC_ENABLE no se puede decidir, y eso NO es un "no". */
        CHECK(caps.misc_enable.rc == ERR_FAULT);
        CHECK(caps.misc_enable.value == 0);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL) {
            CHECK(caps.verdict == (u8)PMU_PEBS_UNKNOWN);
        }
    }

    printf("--- verdict: the MSR says PEBS is unavailable ---\n");
    {
        fake_reset();
        g_fake.misc_enable = ((u64)1) << MSR_MISC_ENABLE_PEBS_UNAVAILABLE;
        CHECK(pmu_caps_detect(0, &caps) == OK);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL) {
            CHECK(caps.pebs_unavailable_bit == 1);
            CHECK(caps.verdict == (u8)PMU_PEBS_UNAVAILABLE);
        }
    }

    printf("--- verdict: the MSR says PEBS is there ---\n");
    {
        fake_reset();
        g_fake.misc_enable = 0; /* bit 12 a cero: disponible */
        CHECK(pmu_caps_detect(0, &caps) == OK);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL) {
            CHECK(caps.pebs_unavailable_bit == 0);
            /* Cual de los dos depende de lo que diga CPUID en ESTA maquina, y
             * eso no se puede falsear desde aqui.  Lo que si es invariante es
             * que se corresponden. */
            if (caps.has_ds) {
                CHECK(caps.verdict == (u8)PMU_PEBS_AVAILABLE);
            } else {
                CHECK(caps.verdict == (u8)PMU_PEBS_MASKED_BY_CPUID);
            }
        }
    }

    /* LA OTRA REGRESION.  La primera version leia el formato de registro PEBS
     * como `value & 0xF`, que son los bits bajos del formato de LBR.  En la
     * maquina de desarrollo los dos valen cero, asi que el error era invisible:
     * publicaba un numero correcto por casualidad.
     *
     * Por eso el valor de prueba pone campos DISTINTOS en los dos sitios --
     * LBR 0x2D, PEBS 3 --: asi la extraccion equivocada no puede acertar.  La
     * de antes daria 0xD, que no es ninguno de los dos. */
    printf("--- IA32_PERF_CAPABILITIES: every field, not just one bit ---\n");
    {
        fake_reset();
        g_fake.perf_capabilities = 0x173EDull;
        CHECK(pmu_caps_detect(0, &caps) == OK);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL && caps.has_pdcm) {
            CHECK(caps.lbr_format == 0x2D);         /* bits  5:0 */
            CHECK(caps.pebs_trap == 1);             /* bit     6 */
            CHECK(caps.pebs_arch_regs == 1);        /* bit     7 */
            CHECK(caps.pebs_record_format == 3);    /* bits 11:8 */
            CHECK(caps.smm_freeze == 1);            /* bit    12 */
            CHECK(caps.full_width_write == 1);      /* bit    13 */
            CHECK(caps.pebs_baseline == 1);         /* bit    14 */
            CHECK(caps.perf_metrics == 0);          /* bit    15 */
            CHECK(caps.pebs_output_pt == 1);        /* bit    16 */
        }
    }

    /* `IA32_MISC_ENABLE` con el valor REAL medido en la maquina de desarrollo,
     * para que lo que se afirma del informe se pueda contrastar con el. */
    printf("--- IA32_MISC_ENABLE: decoded, with a value measured for real ---\n");
    {
        fake_reset();
        g_fake.misc_enable = 0x851889ull;
        CHECK(pmu_caps_detect(0, &caps) == OK);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL) {
            CHECK(caps.perfmon_available == 1);    /* bit  7 */
            CHECK(caps.bts_unavailable == 1);      /* bit 11 */
            CHECK(caps.pebs_unavailable_bit == 1); /* bit 12 */
            /* Y la conclusion: BTS y PEBS caen LOS DOS, que es lo coherente --
             * cuelgan del mismo Debug Store. */
            CHECK(caps.verdict == (u8)PMU_PEBS_UNAVAILABLE);
        }
    }

    /* LA REGRESION QUE MOTIVO ESCRIBIR ESTOS CUATRO CASOS.
     *
     * La primera version deducia "alguien esta contando" de que
     * `PERF_GLOBAL_CTRL` no fuera cero.  En la maquina de desarrollo ese
     * registro se lee con los nueve bits abiertos y NINGUN contador armado, asi
     * que el informe afirmaba que el sistema estaba midiendo cuando no lo
     * estaba.  No fallaba: mentia. */
    printf("--- who is counting: the gate is not a usage flag ---\n");
    {
        fake_reset();
        g_fake.perf_global_ctrl = 0x70000003Full; /* la compuerta, abierta */
        /* ...y ningun selector armado, ni contadores fijos. */
        CHECK(pmu_caps_detect(0, &caps) == OK);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL && caps.pmu_version > 0) {
            CHECK(caps.counters_in_use == 0);
            CHECK(caps.gp_enabled_mask == 0);
        }
    }
    {
        /* Compuerta abierta Y un selector armado: eso si es estar contando. */
        fake_reset();
        g_fake.perf_global_ctrl = 0x70000003Full;
        g_fake.evtsel[2] = armed_evtsel();
        CHECK(pmu_caps_detect(0, &caps) == OK);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL && caps.gp_counters > 2) {
            CHECK(caps.counters_in_use == 1);
            CHECK(caps.gp_enabled_mask == (1u << 2));
        }
    }
    {
        /* Selector armado pero su bit CERRADO en la compuerta: no cuenta.  Se
         * anota como armado igualmente, que es informacion distinta. */
        fake_reset();
        g_fake.perf_global_ctrl = 0; /* todo cerrado */
        g_fake.evtsel[0] = armed_evtsel();
        CHECK(pmu_caps_detect(0, &caps) == OK);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL && caps.gp_counters > 0) {
            CHECK(caps.gp_enabled_mask == 1u);
            CHECK(caps.counters_in_use == 0);
        }
    }
    {
        /* Los contadores FIJOS no tienen selector: mandan ellos solos. */
        fake_reset();
        g_fake.fixed_ctr_ctrl = 0x3u;
        CHECK(pmu_caps_detect(0, &caps) == OK);
        if (caps.vendor == (u32)CPU_VENDOR_INTEL && caps.pmu_version > 0) {
            CHECK(caps.counters_in_use == 1);
        }
    }

    printf("--- formatting ---\n");
    {
        fake_reset();
        CHECK(pmu_caps_detect(0, &caps) == OK);

        /* Un bufer corto no trunca en silencio: lo dice. */
        written = 0;
        CHECK(pmu_caps_format(&caps, buf, 8, &written) == ERR_NOSPACE);
        CHECK(written <= 8);

        /* Y uno holgado sale entero. */
        written = 0;
        CHECK(pmu_caps_format(&caps, buf, sizeof(buf), &written) == OK);
        CHECK(written > 0);
        CHECK(written < sizeof(buf));
    }

    /* El volcado real de esta maquina.  No se comprueba nada de el -- los
     * numeros son de quien lo ejecute --, pero es lo que hace util correr el
     * test a mano.  Las lecturas de MSR salen como fallidas porque en espacio
     * de usuario no se pueden hacer, y eso es exactamente lo que hay que ver:
     * la mitad de CPUID funciona y la otra necesita el driver. */
    printf("\n--- this machine, with MSRs unavailable (user space) ---\n");
    fake_reset();
    g_fake.fault_everything = 1;
    if (pmu_caps_detect(0, &caps) == OK &&
        pmu_caps_format(&caps, buf, sizeof(buf) - 1, &written) == OK) {
        buf[written] = 0; /* cabe: se pidio uno menos que el bufer */
        printf("%s", buf);
    }

    printf("\n--- %d checks, %d failures ---\n", g_checks, g_fails);
    return g_fails ? 1 : 0;
}
