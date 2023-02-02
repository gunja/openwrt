#ifndef FLOMAC_H
#define FLOMAC_H
/*
 *   =====================
 *   FLOMAC mode registers
 *   =====================
 */

typedef enum {
	Mode_Flomac = 0,
	Mode_MMI = 1
} Flomac_Mode_t;

typedef enum {
    Baudrate_1200 = 0,
    Baudrate_2400 = 1,
    Baudrate_4800 = 2,
    Baudrate_9600 = 3,
	Baudrate_14400 = 4,
    Baudrate_19200 = 5,
    Baudrate_28800 = 6,
    Baudrate_38400 = 7,
    Baudrate_57600 = 8,
    Baudrate_115200 = 9
} Flomac_Baudrate_t;

/* Main params registres */
#define REG_F_MASS_FLOW 300
#define REG_F_VOLUME_VLOW 302
#define REG_F_DENSITY 304
#define REG_F_TEMPERATURE 306
#define REG_F_REF_DENSITY 308
#define REG_F_CORR_VOL_FLOW 310
#define REG_F_PRESSURE 312
#define REG_F_DRIVEGAIN 323

/* summators */
#define REG_F_SUM1 708
#define REG_F_SUM2 807
#define REG_F_SUM3 757
#define REG_F_SUM4 857

/* MODBUS Config registres */
/*FIXME such notation is misleading: from _F_ it's expected
FLOAT data type, like values above, but according to
documentation, these addresses stores INTEGER values */
#define REG_F_DEVADDR 430
#define REG_F_BAUDRATE 431
#define REG_F_PARITY 432
#define REG_F_HW_WP 433
#define REG_F_MBUS_MODE 436
#define REG_F_FLOAT_BYTEORDER 437

/* System registers */
#define REG_F_HWVER 400
#define REG_F_HWMOD 401
#define REG_F_SWVER 402
#define REG_F_RDAY 403
#define REG_F_RMONTH 404
#define REG_F_RYEAR 405

/* System units group */
#define REG_RW_I_UNIT_MASS_FLOW 314
#define REG_RW_I_UNIT_VOLUME_FLOW 315
#define REG_RW_I_UNIT_DENSITY 316
#define REG_RW_I_UNIT_TEMPERATURE 317
#define REG_RW_I_UNIT_CORRECTED_VOLUME_FLOW 318
#define REG_RW_I_UNIT_REFERENCE_DENSITY 319

#define REG_RW_I_UNIT_PRESSURE 439


/* Parameters of certain components*/
#define REG_RO_F_TARGET_DENSITY 350
#define REG_RO_F_CARRIER_DENSITY 352
#define REG_RO_F_TARGET_MASS_FLOW 354
#define REG_RO_F_CARRIER_MASS_FLOW 356
#define REG_RO_F_TARGET_VOLUME_FLOW 358
#define REG_RO_F_CARRIER_VOLUME_FLOW 360
#define REG_RO_F_TARGET_MASS_CONCENTRATION 362
#define REG_RO_F_CARRIER_MASS_CONCENTRATION 364
#define REG_RO_F_TARGET_VOLUME_CONCENTRATION 366
#define REG_RO_F_CARRIER_VOLUME_CONCENTRATION 368

/* Display settings*/
#define REG_RW_I_CONTRAST 600
#define REG_RW_I_IMAGE_ORIENTATION 601
#define REG_RW_I_LANGUAGE 602
#define REG_RW_I_QUICK_TOTAL_MENU 603

#define REG_RW_I_F1_ASSIGN 604
#define REG_RW_I_F1_FORMAT 605
#define REG_RW_I_F1_UNIT 606
#define REG_RW_F_F1_V100 607
#define REG_RW_I_F1MUX_ASSIGN 609
#define REG_RW_I_F1MUX_FORMAT 610
#define REG_RW_I_F1MUX_UNIT 611
#define REG_RW_F_F1MUX_V100 612

#define REG_RW_I_F2_ASSIGN 614
#define REG_RW_I_F2_FORMAT 615
#define REG_RW_I_F2_UNIT 616
#define REG_RW_F_F2_V100 617
#define REG_RW_I_F2MUX_ASSIGN 619
#define REG_RW_I_F2MUX_FORMAT 620
#define REG_RW_I_F2MUX_UNIT 621
#define REG_RW_F_F2MUX_V100 622


/* Summators block 4 of documentation */
#define REG_RW_I_SUM_FAILSAFE_MODE 700

#define REG_RW_I_SUM_ASSIGN1 701
#define REG_RW_I_SUM_UNIT1 702
#define REG_RW_I_SUM_STATE1 703
#define REG_RW_I_SUM_MODE1 704
#define REG_RW_I_SUM_RESET1 705
    #define REG_RO_F_SUM_OVERFLOW1 706
    #define REG_RO_F_SUM1 708
    #define REG_RO_F_SUM_TIME1 710

#define REG_RW_I_SUM_ASSIGN2 800
#define REG_RW_I_SUM_UNIT2 801
#define REG_RW_I_SUM_STATE2 802
#define REG_RW_I_SUM_MODE2 803
#define REG_RW_I_SUM_RESET2 804
    #define REG_RO_F_SUM_OVERFLOW2 805
    #define REG_RO_F_SUM2 807
    #define REG_RO_F_SUM_TIME2 809

#define REG_RW_I_SUM_ASSIGN3 750
#define REG_RW_I_SUM_UNIT3 751
#define REG_RW_I_SUM_STATE3 752
#define REG_RW_I_SUM_MODE3 753
#define REG_RW_I_SUM_RESET3 754
    #define REG_RO_F_SUM_OVERFLOW3 755
    #define REG_RO_F_SUM3 757
    #define REG_RO_F_SUM_TIME3 759

#define REG_RW_I_SUM_ASSIGN4 850
#define REG_RW_I_SUM_UNIT4 851
#define REG_RW_I_SUM_STATE4 852
#define REG_RW_I_SUM_MODE4 853
#define REG_RW_I_SUM_RESET4 854
    #define REG_RO_F_SUM_OVERFLOW4 855
    #define REG_RO_F_SUM4 857
    #define REG_RO_F_SUM_TIME4 859

/*  block 5 TBD */
/*  block 6 TBD */
/* base functions block 7 TBD */

/* Concentration  block 8  TBD*/

/* Service parameters block 9 */
#define REG_RW_I_SERIAL_NUMBER_DEVICE 0
#define REG_RW_F_ACCURACY_DEVICE 1
#define REG_RW_I_CERTIFICATION 3
#define REG_RW_I_CALIBRATION_DD 4
#define REG_RW_I_CALIBRATION_MM 5
#define REG_RW_I_CALIBRATION_YY 6
#define REG_RW_F_SENSOR_MAX_QM_T_H 7
#define REG_RW_I_SENSOR_TYPE 9
#define REG_RW_I_SENSOR_DIAMETER 10
#define REG_RW_F_SENSOR_T_MAX 11
#define REG_RW_F_SENSOR_T_MIN 13
#define REG_RW_F_MAX_PRESSURE 45
#define REG_RW_F_GAS_FLUID_BORDER 53
#define REG_RO_I_SN_MEASURE_MODUL 208
#define REG_RO_I_MM_HARD_REV 211
#define REG_RO_I_MM_MODIFICATION 212
#define REG_RO_I_MM_SOFT_REV 213
#define REG_RO_I_MM_CALIBRATION_DAY 214
#define REG_RO_I_MM_CALIBRATION_MONTH 215
#define REG_RO_I_MM_CALIBRATION_YEAR 216

/* technologicalparameters block 10*/
#define REG_RW_I_DISPLAY_MODE 34

#define FLOMAK_MAX_USED_REG__ 1795
#define FLOMAK_MAX_SCAN_REG 1900

#endif
