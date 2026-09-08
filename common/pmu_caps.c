/*
 * VestaVM - Maquina Virtual Distribuida
 *
 * Copyright (C) 2026 David Lopez.T (DesmonHak) (Castilla y Leon, ES)
 * Licencia: GPLv2 (ver LICENSE).
 */

/**
 * @file pmu_caps.c
 * @brief La deteccion del PMU, sin una sola cabecera del sistema operativo.
 *
 * Que no incluya nada del sistema es lo que permite compilar este fichero
 * TAMBIEN en espacio de usuario y probarlo con un `msr_read` de mentira, en vez
 * de depurarlo cargandolo en una maquina y viendo si se cuelga.
 */

#include "pmu_caps.h"

#include "cpuid_intrin.h"
#include "msr.h"
#include "writer.h"

/* -------------------------------------------------------------------------
 *  Utilidades locales.
 *
 *  `static` porque el fichero es la frontera del modulo, y escritas a mano
 *  porque en kernel no hay biblioteca estandar: no se puede llamar a `memset`
 *  ni a `snprintf`.  Y aunque se pudiera, tenerlas aqui es lo que hace que las
 *  pruebas de usuario ejerciten EXACTAMENTE el codigo que corre dentro.
 * ------------------------------------------------------------------------- */

/** @brief Pone `n` bytes a cero. */
static void zero_bytes(void *p, usize n) {
    u8 *b = (u8 *)p;
    usize i;
    for (i = 0; i < n; ++i) {
        b[i] = 0;
    }
}

/** @brief Anade un `msr_value`: el numero, o por que no se pudo leer. */
static void put_msr(writer *w, const msr_value *m, int digits) {
    if (m->rc == OK) {
        put_hex(w, m->value, digits);
        return;
    }
    if (m->rc == ERR_FAULT) {
        put_str(w, "<faulted: the CPU refused the read>");
        return;
    }
    if (m->rc == ERR_UNSUPPORTED) {
        put_str(w, "<not read: no CPUID bit for it>");
        return;
    }
    put_str(w, "<not read>");
}

/* -------------------------------------------------------------------------
 *  Deteccion.
 * ------------------------------------------------------------------------- */

/** @brief Fabricante, de la hoja 0.  Decide que MSR tienen sentido. */
static u32 detect_vendor(void) {
    /* La deteccion vive en `cpuid_intrin.h`: es CPUID puro y la necesita
     * tambien el volcado.  `cpu_vendor` y `CPUID_VENDOR_*` valen lo mismo a
     * proposito, y las dos comprobaciones de abajo lo sostienen -- si alguien
     * renumera uno de los dos, no compila en vez de devolver otro fabricante. */
    STATIC_ASSERT(CPUID_VENDOR_INTEL == (u32)CPU_VENDOR_INTEL,
                  "CPUID_VENDOR_INTEL and CPU_VENDOR_INTEL disagree");
    STATIC_ASSERT(CPUID_VENDOR_AMD == (u32)CPU_VENDOR_AMD,
                  "CPUID_VENDOR_AMD and CPU_VENDOR_AMD disagree");
    return cpuid_vendor();
}

/**
 * @brief Familia, modelo y stepping, ya compuestos con sus partes extendidas.
 *
 * La composicion no es opcional: en toda la linea moderna la familia base vale
 * 6, y sin sumar el modelo extendido todos los procesadores de los ultimos
 * quince anos salen como el mismo.
 */
static void detect_family(pmu_caps *c) {
    cpuid_regs r;
    u32 family_id, model_id, ext_model, ext_family;
    cpuid_query(CPUID_LEAF_FEATURES, 0, &r);

    family_id = (r.eax >> 8) & 0xFu;
    model_id = (r.eax >> 4) & 0xFu;
    ext_model = (r.eax >> 16) & 0xFu;
    ext_family = (r.eax >> 20) & 0xFFu;

    c->stepping = r.eax & 0xFu;
    c->family = family_id;
    if (family_id == 0xFu) {
        c->family += ext_family;
    }
    c->model = model_id;
    if (family_id == 0x6u || family_id == 0xFu) {
        c->model += (ext_model << 4);
    }

    c->has_ds = (u8)cpuid_bit(r.edx, CPUID_01_EDX_DS);
    /* El manual llama `PERF_CAPABILITIES` a lo que todo el mundo conoce como
     * PDCM.  Se usa el nombre del manual, que es el que se puede buscar en el. */
    c->has_pdcm = (u8)cpuid_bit(r.ecx, CPUID_01_ECX_PERF_CAPABILITIES);
    /* Este NO sale de la tabla generada, y no es un olvido: Intel documenta ese
     * bit como "no usado, siempre cero".  Ver `cpuid_intrin.h`. */
    c->has_hypervisor = (u8)cpuid_bit(r.ecx, CPUID_1_ECX_HYPERVISOR);
}

/** @brief El PMU arquitectonico, de la hoja 0x0A. */
static void detect_pmu(pmu_caps *c, u32 max_leaf) {
    cpuid_regs r;
    if (max_leaf < CPUID_LEAF_PMU) {
        return; /* sin hoja 0x0A no hay PMU arquitectonico: todo queda a cero */
    }
    cpuid_query(CPUID_LEAF_PMU, 0, &r);
    c->pmu_version = r.eax & 0xFFu;
    c->gp_counters = (r.eax >> 8) & 0xFFu;
    c->gp_width = (r.eax >> 16) & 0xFFu;
    c->arch_events_listed = (r.eax >> 24) & 0xFFu;
    c->arch_events_absent = r.ebx;
    c->fixed_ctr_bitmap = r.ecx;
    c->fixed_counters = r.edx & 0x1Fu;
    c->fixed_width = (r.edx >> 5) & 0xFFu;
}

/** @brief Clase de nucleo e hibridacion. */
static void detect_topology(pmu_caps *c, u32 max_leaf) {
    cpuid_regs r;
    if (max_leaf >= CPUID_LEAF_EXT_FEATURES) {
        cpuid_query(CPUID_LEAF_EXT_FEATURES, 0, &r);
        c->has_hybrid = (u8)cpuid_bit(r.edx, CPUID_07_00_EDX_HYBRID);
    }
    if (max_leaf >= CPUID_LEAF_HYBRID) {
        cpuid_query(CPUID_LEAF_HYBRID, 0, &r);
        c->core_type = (r.eax >> 24) & 0xFFu;
    }
}

/**
 * @brief La puerta de APERF y MPERF.
 *
 * Los dos contadores solo existen si `CPUID.06H:ECX[0]` lo dice, y son los que
 * permiten saber a que frecuencia REAL corrio el nucleo durante la muestra.
 * Sin ellos, un perfilador que reparta ciclos entre tiempo se equivoca en
 * cuanto la frecuencia cambia, que en una pieza moderna es siempre.
 */
static void detect_hw_feedback(pmu_caps *c, u32 max_leaf) {
    cpuid_regs r;
    if (max_leaf < CPUID_LEAF_THERMAL_POWER) {
        return;
    }
    cpuid_query(CPUID_LEAF_THERMAL_POWER, 0, &r);
    c->has_hw_feedback = (u8)cpuid_bit(r.ecx, CPUID_06_ECX_HW_FEEDBACK_CAP);
}

/** @brief Lo del rango extendido: TSC invariante, RDTSCP e IBS de AMD. */
static void detect_extended(pmu_caps *c) {
    cpuid_regs r;
    u32 max_ext = cpuid_max_extended_leaf();
    if (max_ext >= CPUID_LEAF_EXT_POWER) {
        cpuid_query(CPUID_LEAF_EXT_POWER, 0, &r);
        c->has_invariant_tsc =
                (u8)cpuid_bit(r.edx, CPUID_80000007_EDX_TSC_INVARIANT);
    }
    if (max_ext >= CPUID_LEAF_EXT_FEATURES_AMD) {
        cpuid_query(CPUID_LEAF_EXT_FEATURES_AMD, 0, &r);
        /* `rdtscp` es la unica forma de leer el contador Y saber en que nucleo
         * se leyo sin una llamada al sistema por medio, y el mismo bit es la
         * puerta de `IA32_TSC_AUX`.  Lo definen los dos fabricantes en la misma
         * posicion, asi que se pregunta antes de mirar el fabricante. */
        c->has_rdtscp = (u8)cpuid_bit(r.edx, CPUID_80000001_EDX_RDTSCP);
        if (c->vendor == (u32)CPU_VENDOR_AMD) {
            c->has_ibs = (u8)cpuid_bit(r.ecx, CPUID_80000001_ECX_IBS);
        }
    }
}

/**
 * @brief Lee un MSR y deja el resultado con su estado pegado.
 *
 * @param gated si es cero, ni se intenta: se anota `ERR_UNSUPPORTED`.  Sirve
 *              para los MSR cuya existencia depende de un bit de CPUID -- leer
 *              uno que no existe provoca una excepcion de proteccion general,
 *              y en kernel eso es un pantallazo.
 */
static void read_msr_into(msr_value *out, u32 index, int gated) {
    if (!gated) {
        out->value = 0;
        out->rc = ERR_UNSUPPORTED;
        return;
    }
    out->rc = msr_read(index, &out->value);
    if (out->rc != OK) {
        out->value = 0;
    }
}

/**
 * @brief La segunda opinion: lo que dicen los MSR.
 *
 * Solo se leen los GARANTIZADOS.  `IA32_DS_AREA` y `IA32_PEBS_ENABLE` existen
 * unicamente si existe el Debug Store, que es justo lo que se esta preguntando;
 * intentarlo seria arriesgar un pantallazo para adelantar una respuesta que
 * `IA32_MISC_ENABLE` ya da.  Ver la nota al final de `msr.h`.
 */
static void detect_msrs(pmu_caps *c) {
    int is_intel = (c->vendor == (u32)CPU_VENDOR_INTEL);

    /* IA32_MISC_ENABLE existe en toda la linea Intel desde el P6. */
    read_msr_into(&c->misc_enable, IA32_MISC_ENABLE, is_intel);

    /* IA32_PERF_CAPABILITIES solo si CPUID anuncia PDCM.  Esta es la razon de
     * ser del parametro `gated`. */
    read_msr_into(&c->perf_capabilities, IA32_PERF_CAPABILITIES,
                  is_intel && c->has_pdcm);

    /* Los tres siguientes existen si hay PMU arquitectonico. */
    read_msr_into(&c->debugctl, IA32_DEBUGCTL, is_intel);
    read_msr_into(&c->fixed_ctr_ctrl, IA32_FIXED_CTR_CTRL,
                  is_intel && c->pmu_version > 0);
    read_msr_into(&c->perf_global_ctrl, IA32_PERF_GLOBAL_CTRL,
                  is_intel && c->pmu_version > 0);

    /* Los selectores de evento, uno por contador de proposito general.  El
     * limite lo pone CPUID y no una constante: leer el selector de un contador
     * que no existe es leer un MSR que no existe. */
    {
        u32 i;
        u32 count = c->gp_counters;
        if (count > MSR_PERFEVTSEL_MAX) {
            count = MSR_PERFEVTSEL_MAX;
        }
        for (i = 0; i < MSR_PERFEVTSEL_MAX; ++i) {
            read_msr_into(&c->evtsel[i], IA32_PERFEVTSEL0 + i,
                          is_intel && c->pmu_version > 0 && i < count);
        }
    }
}

/**
 * @brief El veredicto, que es donde se junta lo que dicen las dos fuentes.
 *
 * La distincion que importa, y la razon de que esto no sea un booleano: cuando
 * el MSR dice que PEBS esta y CPUID dice que no hay Debug Store, la respuesta
 * NO es "no".  Es "CPUID lo esta tapando", que manda a un sitio completamente
 * distinto -- a probarlo -- en vez de a darse por vencido.
 */
static void derive_verdict(pmu_caps *c) {
    if (c->vendor != (u32)CPU_VENDOR_INTEL) {
        c->verdict = (u8)PMU_PEBS_NOT_APPLICABLE;
        return;
    }

    /* `IA32_PERF_CAPABILITIES`, entero.
     *
     * El formato de PEBS son los bits 11:8, NO los bajos -- esos son el formato
     * de LBR.  La primera version leia `value & 0xF` y publicaba el formato de
     * LBR llamandolo de PEBS; en una maquina sin ninguno de los dos, los dos
     * valen cero y el error es invisible. */
    if (c->perf_capabilities.rc == OK) {
        u64 v = c->perf_capabilities.value;
        c->lbr_format = (u8)((v >> MSR_PERF_CAP_LBR_FORMAT_SHIFT) &
                             MSR_PERF_CAP_LBR_FORMAT_MASK);
        c->pebs_record_format = (u8)((v >> MSR_PERF_CAP_PEBS_FORMAT_SHIFT) &
                                     MSR_PERF_CAP_PEBS_FORMAT_MASK);
        c->pebs_trap = (u8)((v >> MSR_PERF_CAP_PEBS_TRAP) & 1u);
        c->pebs_arch_regs = (u8)((v >> MSR_PERF_CAP_PEBS_ARCH_REGS) & 1u);
        c->smm_freeze = (u8)((v >> MSR_PERF_CAP_SMM_FREEZE) & 1u);
        c->full_width_write = (u8)((v >> MSR_PERF_CAP_FULL_WIDTH_WRITE) & 1u);
        c->pebs_baseline = (u8)((v >> MSR_PERF_CAP_PEBS_BASELINE) & 1u);
        c->perf_metrics = (u8)((v >> MSR_PERF_CAP_PERF_METRICS) & 1u);
        c->pebs_output_pt = (u8)((v >> MSR_PERF_CAP_PEBS_OUTPUT_PT) & 1u);
    }
    /* Que contadores estan ARMADOS de verdad.
     *
     * Un contador de proposito general cuenta si su selector tiene el `EN`
     * puesto Y su bit esta abierto en la compuerta global.  Los fijos, si
     * `FIXED_CTR_CTRL` no es cero.  Mirar solo la compuerta -- que es lo que
     * hacia la primera version -- da un si permanente: en esta maquina se lee
     * con los nueve bits abiertos y ningun contador contando. */
    {
        u32 i;
        u64 gate = (c->perf_global_ctrl.rc == OK) ? c->perf_global_ctrl.value
                                                  : ~(u64)0;
        for (i = 0; i < MSR_PERFEVTSEL_MAX; ++i) {
            if (c->evtsel[i].rc != OK) {
                continue;
            }
            if (((c->evtsel[i].value >> MSR_PERFEVTSEL_EN) & 1u) == 0) {
                continue;
            }
            c->gp_enabled_mask |= (1u << i);
            if ((gate >> i) & 1u) {
                c->counters_in_use = 1;
            }
        }
        if (c->fixed_ctr_ctrl.rc == OK && c->fixed_ctr_ctrl.value != 0) {
            c->counters_in_use = 1;
        }
    }

    if (c->misc_enable.rc != OK) {
        c->verdict = (u8)PMU_PEBS_UNKNOWN;
        return;
    }

    /* `IA32_MISC_ENABLE`, tambien entero.  Que BTS y PEBS caigan los dos es
     * coherente -- los dos cuelgan del Debug Store -- y esa coherencia es
     * informacion: un bit suelto no habria dicho nada. */
    c->perfmon_available =
            (u8)((c->misc_enable.value >>
                  MSR_MISC_ENABLE_PERFMON_AVAILABLE) & 1u);
    c->bts_unavailable =
            (u8)((c->misc_enable.value >>
                  MSR_MISC_ENABLE_BTS_UNAVAILABLE) & 1u);
    c->pebs_unavailable_bit =
            (u8)((c->misc_enable.value >>
                  MSR_MISC_ENABLE_PEBS_UNAVAILABLE) & 1u);

    if (c->pebs_unavailable_bit) {
        c->verdict = (u8)PMU_PEBS_UNAVAILABLE;
        return;
    }
    c->verdict = c->has_ds ? (u8)PMU_PEBS_AVAILABLE
                           : (u8)PMU_PEBS_MASKED_BY_CPUID;
}

status pmu_caps_detect(u32 cpu_index, pmu_caps *out) {
    u32 max_leaf;

    if (out == 0) {
        return ERR_INVALID;
    }
    zero_bytes(out, sizeof(*out));
    out->cpu_index = cpu_index;

    max_leaf = cpuid_max_basic_leaf();
    out->vendor = detect_vendor();
    detect_family(out);
    detect_pmu(out, max_leaf);
    detect_topology(out, max_leaf);
    detect_hw_feedback(out, max_leaf);
    detect_extended(out);
    detect_msrs(out);
    derive_verdict(out);
    return OK;
}

/* -------------------------------------------------------------------------
 *  Presentacion.
 * ------------------------------------------------------------------------- */

const char *pmu_caps_verdict_name(u8 verdict) {
    switch ((pebs_verdict)verdict) {
    case PMU_PEBS_AVAILABLE:
        return "AVAILABLE (CPUID and MSR agree)";
    case PMU_PEBS_MASKED_BY_CPUID:
        return "MASKED BY CPUID (the MSR says PEBS is there)";
    case PMU_PEBS_UNAVAILABLE:
        return "UNAVAILABLE (IA32_MISC_ENABLE bit 12 is set)";
    case PMU_PEBS_NOT_APPLICABLE:
        return "NOT APPLICABLE (not an Intel part; AMD uses IBS)";
    case PMU_PEBS_UNKNOWN:
    default:
        return "UNKNOWN (the deciding MSR could not be read)";
    }
}

const char *pmu_caps_core_class_name(u32 core_type) {
    switch ((core_class)core_type) {
    case CORE_CLASS_ATOM:
        return "E-core";
    case CORE_CLASS_CORE:
        return "P-core";
    case CORE_CLASS_UNKNOWN:
    default:
        return "single class";
    }
}

/** @brief Los siete eventos arquitectonicos, en el orden de los bits de EBX. */
/**
 * @brief Cuantos eventos arquitectonicos enumera el mapa de bits de CPUID.0AH.
 *
 * TRECE, no siete.  Los siete clasicos son los que todo el mundo conoce, y por
 * eso el codigo llevaba ocho entradas: los siete y las ranuras de topdown.  El
 * manual enumera seis mas -- el reparto de topdown al completo y las
 * inserciones de LBR --, y sin recorrerlos todos el informe diria "no
 * enumerado" de algo que la pieza si tiene.
 *
 * Es el motivo de sacar estas tablas del manual y no de la memoria: lo que uno
 * recuerda es lo que habia cuando lo aprendio.
 */
#define ARCH_EVENT_MAX 13

/** @brief El nombre que les da el manual, en el orden de los bits de EBX. */
static const char *const arch_event_name[ARCH_EVENT_MAX] = {
        "CORE_CYC        core cycles",
        "INTR_RET        instructions retired",
        "REF_CYC         reference cycles",
        "LLC_CYC         LLC references",
        "LLC_MISSES      LLC misses",
        "BR_INSTR_RET    branch instructions retired",
        "BR_MISPRED_RET  branch mispredicts retired",
        "SLOTS           topdown slots",
        "BACKEND         topdown backend bound",
        "BADSPEC         topdown bad speculation",
        "FRONTEND        topdown frontend bound",
        "RETIRING        topdown retiring",
        "LBR_INSERTS     LBR inserts"};

/**
 * @brief Su codificacion, donde se conoce.
 *
 * Los siete primeros salen de la tabla de eventos arquitectonicos y estan
 * contrastados.  De los seis ultimos **no se pone nada**: el reparto de topdown
 * no se lee por un selector de evento sino por el registro de metricas, y la
 * codificacion de las inserciones de LBR es propia de cada microarquitectura.
 *
 * Escribir un numero plausible ahi seria justo el fallo que este modulo existe
 * para no cometer.  Cuando haga falta, sale de las tablas por microarquitectura,
 * no de la memoria.
 */
static const char *const arch_event_encoding[ARCH_EVENT_MAX] = {
        "0x3C/0x00", "0xC0/0x00", "0x3C/0x01", "0x2E/0x4F",
        "0x2E/0x41", "0xC4/0x00", "0xC5/0x00", "0xA4/0x01",
        "(microarchitecture-specific)", "(microarchitecture-specific)",
        "(microarchitecture-specific)", "(microarchitecture-specific)",
        "(microarchitecture-specific)"};

/** @brief Anade "yes"/"no" segun una bandera. */
static void put_flag(writer *w, const char *label, u8 flag) {
    put_str(w, label);
    put_str(w, flag ? "yes" : "no");
}

status pmu_caps_format(const pmu_caps *caps, char *buf, usize cap,
                       usize *written) {
    writer w;
    u32 i;

    if (caps == 0 || buf == 0 || written == 0) {
        return ERR_INVALID;
    }
    writer_init(&w, buf, cap);

    put_str(&w, "cpu ");
    put_u64(&w, caps->cpu_index);
    put_str(&w, "  ");
    put_str(&w, pmu_caps_core_class_name(caps->core_type));
    put_str(&w, "  family ");
    put_u64(&w, caps->family);
    put_str(&w, " model ");
    put_u64(&w, caps->model);
    put_str(&w, " stepping ");
    put_u64(&w, caps->stepping);
    put_ch(&w, '\n');

    put_str(&w, "  PMU version ");
    put_u64(&w, caps->pmu_version);
    put_str(&w, ", gp ");
    put_u64(&w, caps->gp_counters);
    put_ch(&w, '@');
    put_u64(&w, caps->gp_width);
    put_str(&w, "b, fixed ");
    put_u64(&w, caps->fixed_counters);
    put_ch(&w, '@');
    put_u64(&w, caps->fixed_width);
    put_str(&w, "b, fixed bitmap ");
    put_hex(&w, caps->fixed_ctr_bitmap, 8);
    put_ch(&w, '\n');

    put_flag(&w, "  hypervisor=", caps->has_hypervisor);
    put_flag(&w, "  hybrid=", caps->has_hybrid);
    put_flag(&w, "  DS=", caps->has_ds);
    put_flag(&w, "  PDCM=", caps->has_pdcm);
    put_flag(&w, "  invariant TSC=", caps->has_invariant_tsc);
    put_flag(&w, "  RDTSCP=", caps->has_rdtscp);
    put_flag(&w, "  APERF/MPERF=", caps->has_hw_feedback);
    put_ch(&w, '\n');

    put_str(&w, "  architectural events (");
    put_u64(&w, caps->arch_events_listed);
    put_str(&w, " enumerated):\n");
    for (i = 0; i < ARCH_EVENT_MAX; ++i) {
        int listed = (i < caps->arch_events_listed);
        int absent = (int)((caps->arch_events_absent >> i) & 1u);
        put_str(&w, "    ");
        if (!listed) {
            put_str(&w, "[not enumerated] ");
        } else if (absent) {
            put_str(&w, "[ABSENT]         ");
        } else {
            put_str(&w, "[ok]             ");
        }
        put_str(&w, arch_event_name[i]);
        put_str(&w, "  ");
        put_str(&w, arch_event_encoding[i]);
        put_ch(&w, '\n');
    }

    put_str(&w, "  MSR reads (what CPUID cannot deny):\n");
    put_str(&w, "    IA32_MISC_ENABLE        = ");
    put_msr(&w, &caps->misc_enable, 16);
    put_ch(&w, '\n');
    put_str(&w, "    IA32_PERF_CAPABILITIES  = ");
    put_msr(&w, &caps->perf_capabilities, 16);
    put_ch(&w, '\n');
    put_str(&w, "    IA32_DEBUGCTL           = ");
    put_msr(&w, &caps->debugctl, 16);
    put_ch(&w, '\n');
    put_str(&w, "    IA32_FIXED_CTR_CTRL     = ");
    put_msr(&w, &caps->fixed_ctr_ctrl, 16);
    put_ch(&w, '\n');
    put_str(&w, "    IA32_PERF_GLOBAL_CTRL   = ");
    put_msr(&w, &caps->perf_global_ctrl, 16);
    put_str(&w, "   (a gate, not a usage flag)\n");
    for (i = 0; i < MSR_PERFEVTSEL_MAX; ++i) {
        if (caps->evtsel[i].rc != OK) {
            continue;
        }
        put_str(&w, "    IA32_PERFEVTSEL");
        put_u64(&w, i);
        put_str(&w, "        = ");
        put_msr(&w, &caps->evtsel[i], 16);
        if ((caps->gp_enabled_mask >> i) & 1u) {
            put_str(&w, "   <- armed");
        }
        put_ch(&w, '\n');
    }
    put_str(&w, "  counters in use right now: ");
    put_str(&w, caps->counters_in_use ? "YES -- something else is measuring"
                                      : "no");
    put_ch(&w, '\n');

    /* Los dos registros DECODIFICADOS.  Leer uno y quedarse con un bit es como
     * se llega a conclusiones sueltas: que BTS y PEBS caigan los dos explica la
     * causa -- los dos cuelgan del Debug Store --, y eso no se ve mirando solo
     * el bit de PEBS. */
    put_str(&w, "  IA32_MISC_ENABLE decoded:");
    put_flag(&w, "  perfmon=", caps->perfmon_available);
    put_flag(&w, "  BTS unavailable=", caps->bts_unavailable);
    put_flag(&w, "  PEBS unavailable=", caps->pebs_unavailable_bit);
    put_ch(&w, '\n');

    put_str(&w, "  IA32_PERF_CAPABILITIES decoded:\n");
    put_str(&w, "    LBR record format  = ");
    put_u64(&w, caps->lbr_format);
    if (caps->lbr_format == 0) {
        put_str(&w, "   (no LBR on this part)");
    }
    put_ch(&w, '\n');
    put_str(&w, "    PEBS record format = ");
    put_u64(&w, caps->pebs_record_format);
    if (caps->pebs_record_format == 0) {
        put_str(&w, "   (no PEBS on this part)");
    }
    put_ch(&w, '\n');
    put_str(&w, "   ");
    put_flag(&w, " PEBS trap=", caps->pebs_trap);
    put_flag(&w, "  arch regs=", caps->pebs_arch_regs);
    put_flag(&w, "  SMM freeze=", caps->smm_freeze);
    put_ch(&w, '\n');
    put_str(&w, "   ");
    put_flag(&w, " full width write=", caps->full_width_write);
    put_flag(&w, "  PEBS baseline=", caps->pebs_baseline);
    put_flag(&w, "  perf metrics=", caps->perf_metrics);
    put_flag(&w, "  PEBS to PT=", caps->pebs_output_pt);
    put_ch(&w, '\n');
    if (!caps->full_width_write) {
        /* Un limite del que conviene enterarse aqui y no al ver salir el doble
         * de interrupciones de las pedidas. */
        put_str(&w, "    -> no full-width write: periods above 2^31 cannot be "
                    "programmed\n");
    }

    put_str(&w, "  PEBS verdict: ");
    put_str(&w, pmu_caps_verdict_name(caps->verdict));
    put_ch(&w, '\n');

    *written = w.len;
    return w.overflow ? ERR_NOSPACE : OK;
}
