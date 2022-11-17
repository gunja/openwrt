#ifndef MMI_H
#define MMI_H
/*
* ==================
* MMI Mode coils REG_F_isters
* ==================
*/
/* Coils */
#define C_MMI_STARTSTOP 1
#define C_MMI_RESET_TOTALS 2
#define C_MMI_RESET_INVENT 3
#define C_MMI_START_ZEROING 4
#define C_MMI_RESET_MASS_TOTAL 55
#define C_MMI_RESET_VOLUME_TOTAL 56
#define C_MMI_ENABLE_PCOMP 81


/* registers */
#define R_MMI_SW_VER 15
#define R_MMI_DEVTYPE_CODE 119 // must be 40
#define R_MMI_MANUF_CODE 120 // must be 31
#define R_MMI_HART_T01 67
#define R_MMI_HART_T23 68
#define R_MMI_HART_T45 69
#define R_MMI_HART_T67 70

#define R_MMI_MBUS_ADDR 312
#define R_MMI_MBUS_MAP 436
#define R_MMI_FPBO 520

#define R_MMI_MEASUREMENTS_BASE 246 /* main parameters */
#define R_MMI_MASS_FRATE (R_MMI_MEASUREMENTS_BASE)
#define R_MMI_DENSITY (R_MMI_MEASUREMENTS_BASE + 2)
#define R_MMI_TEMPERATURE (R_MMI_MEASUREMENTS_BASE + 4)
#define R_MMI_VOLUME_FRATE (R_MMI_MEASUREMENTS_BASE + 6)
#define R_MMI_DUMMY01 (R_MMI_MEASUREMENTS_BASE + 8)
#define R_MMI_PRESSURE (R_MMI_MEASUREMENTS_BASE + 10)
#define R_MMI_MASS_TOTAL (R_MMI_MEASUREMENTS_BASE + 12)
#define R_MMI_VOL_TOTLAL (R_MMI_MEASUREMENTS_BASE + 14)
#define R_MMI_MASS_INVENT (R_MMI_MEASUREMENTS_BASE + 16)
#define R_MMI_VOL_INVENT (R_MMI_MEASUREMENTS_BASE + 18)

#define R_MMI_UNITS_BASE 38 /* measure units setting registers */
#define R_MMI_MASSFLOW_U (R_MMI_UNITS_BASE)
#define R_MMI_DENSITY_U (R_MMI_UNITS_BASE + 1)
#define R_MMI_TEMPERATURE_U (R_MMI_UNITS_BASE + 2)
#define R_MMI_VOLFLOW_U (R_MMI_UNITS_BASE + 3)
#define R_MMI_DUMMY00 (R_MMI_UNITS_BASE + 4)
#define R_MMI_PRESSURE_U (R_MMI_UNITS_BASE + 5)
#define R_MMI_MASS_U (R_MMI_UNITS_BASE + 6)
#define R_MMI_VOLUME_U (R_MMI_UNITS_BASE + 7)


#define R_MMI_DIAG_BASE 284 /* diagnostics registers */
#define R_MMI_TUBE_FREQ (R_MMI_DIAG_BASE)
#define R_MMI_LEFT_V (R_MMI_DIAG_BASE + 2) // always 0.2
#define R_MMI_RIGHT_V (R_MMI_DIAG_BASE + 4) // always 0.2
#define R_MMI_DRIVE_GAIN (R_MMI_DIAG_BASE + 6)
#define R_MMI_MASS_FLOW_LZ (R_MMI_DIAG_BASE + 8)
#define R_MMI_STATE_BITMASK1 (R_MMI_DIAG_BASE + 9)
#define R_MMI_STATE_BITMASK2 (R_MMI_DIAG_BASE + 10)
#define R_MMI_STATE_BITMASK2 (R_MMI_DIAG_BASE + 10)


enum MassFlowUnit {
    g_SEC = 70,
    g_MIN,
    g_HOUR,
    kg_SEC,
    kg_MIN,
    kg_HOUR,
    kg_DAY,
    tonn_MIN,
    tonn_HOUR,
    tonn_DAY,
    tonn_SEC = 240
};

enum DensityUnit {
    g_CM3 = 91,
    kg_M3,
    kg_L = 96
};

enum TemperatureUnit {
    DEG_C = 32,
    DEG_F,
    DEG_K = 35
};

enum VolumeFlowUnit {
    cm3_SEC = 241,
    cm3_MIN,
    cm3_HOUR,
    m3_SEC = 28,
    m3_HOUR = 19,
    l_SEC = 24,
    l_MIN = 17,
    l_HOUR = 138,
    m3_DAY = 29
};

enum PressureUnit {
    PRESSURE_UNIT_PSI = 6,
    PRESSURE_UNIT_BAR,
    kg_cm2 = 10,
    kPa = 12,
    MPa = 237
};

enum SummMassUnit {
    MASS_UNIT_GRAMM = 60,
    MASS_UNIT_KG,
    MASS_UNIT_TONN
};

enum SummVolumeUnit {
    VOL_UNIT_CM3 = 244,
    VOL_UNIT_LITERS = 41,
    VOL_UNIT_M3 = 43
};

#endif
