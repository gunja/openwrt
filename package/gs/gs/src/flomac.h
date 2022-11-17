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

#endif
