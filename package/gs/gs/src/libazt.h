
#ifndef SRC_LIBAZT_H
#define SRC_LIBAZT_H

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include "libazt_ll.h"

#define TRK_DEFAULT_ADDRESS 0x01

#define COMPLIMENTARY(a)(0x7F & (~a))

#define AZT_PROTOCOL_VERSION            0x02

#define AZT_RQST_DEL_SYMBOL             0x7F
#define AZT_RQST_STX_SYMBOL             0x02
#define AZT_RQST_ENDING_SYMBOL          0x03

#define AZT_ACK_SYMBOL                  0x06
#define AZT_CAN_SYMBOL                  0x18
#define AZT_NAK_SYMBOL                  0x15

#define AZT_REQUEST_PARAMS_MAX_AMOUNT   10
#define AZT_RESPONCE_MAX_LENGTH         100

#define AZT_REQUEST_TRK_STATUS_REQUEST              0x31
#define AZT_REQUEST_TRK_AUTHORIZATION               0x32
#define AZT_REQUEST_TRK_RESET                       0x33
#define AZT_REQUEST_CURRENT_FUEL_CHARGE_VALUE       0x34
#define AZT_REQUEST_FULL_FUELING_VALUE              0x35
#define AZT_REQUEST_SUMMATORS_VALUE                 0x36
#define AZT_REQUEST_TRK_TYPE                        0x37
#define AZT_REQUEST_RECORD_CONFIRMATION             0x38
#define AZT_REQUEST_PROTOCOL_VERSION                0x50
#define AZT_REQUEST_PRICE_PER_LITER_SETUP           0x51
#define AZT_REQUEST_VALVE_DISABLING_THRESHOLD_SETUP 0x52
#define AZT_REQUEST_FUELING_DOSE_IN_RUBLES          0x53
#define AZT_REQUEST_FUELING_DOSE_IN_LITERS          0x54
//#define AZT_REQUEST_ADDITIONAL_DOSE                 ?
#define AZT_REQUEST_UNCONDITIONAL_START             0x56
#define AZT_REQUEST_TRK_ADDRESS_CHANGE              0x5D
#define AZT_REQUEST_COMMON_PARAMETERS_SETUP         0x57
#define AZT_REQUEST_CURRENT_TRANSACTION             0x59
#define AZT_REQUEST_READ_PARAMS                     0x4E
#define AZT_REQUEST_WRITE_PARAMS                    0x4F
#define AZT_REQUEST_CURRENT_DOSE_READING            0x58


#define AZT_PARAM_TRK_MODE                  0x32  // 0x31 - left, 0x32 - right, 0x33 - special
#define AZT_PARAM_TRK_MODE_VAL_NULL         0x30
#define AZT_PARAM_TRK_MODE_VAL_LEFT         0x31
#define AZT_PARAM_TRK_MODE_VAL_RIGHT        0x32
#define AZT_PARAM_TRK_MODE_VAL_SPECIAL      0x33

#define AZT_PARAM_TRK_DOSE_SETUP_TYPE                               0x31
#define AZT_PARAM_TRK_DOSE_SETUP_TYPE_VAL_SYSTEM_CONTROL_ONLY       0x30
#define AZT_PARAM_TRK_DOSE_SETUP_TYPE_VAL_SYSTEM_AND_LOCAL_CONTROL  0x31

//#define AZT_STATUS_TRK_DISABLED_RK_INSTALLED                0x30
//#define AZT_STATUS_TRK_DISABLED_RK_TAKEN_OFF                0x31
//#define AZT_STATUS_TRK_AUTHORIZATION_CMD                    0x32
//#define AZT_STATUS_TRK_ENABLED_FUEL_DISCHARGING             0x33
//#define AZT_STATUS_TRK_DISABLED_FUEL_DISCHARGING_FINISHED   0x34
//#define AZT_STATUS_TRK_DISABLED_LOCAL_CONTROL_UNIT_DOSE     0x38
//#define AZT_STATUS_TRK_

#define AZT_TRK_TYPE                        0x3B  // 6 digits of liters, 6 digits of price per liter, 8 digits of total price

#define AZT_BYTE_SHIFT                      0x30

#define ASCII_ZERO                          0x30
#ifdef AZTLIB

#define AZT_STARTING_SYMBOLS_AMOUNT     15
const char AZT_STARTING_SYMBOLS[] = {0x02, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D,
                                     0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14 };
const int AZT_ADDRESS_SHIFTS[] = {0, 15, 30, 45, 60, 75, 90, 105, 120, 135,
                                  150, 165, 180, 195, 210 };


#define AZT_RELATED_ADDRESS_BASE        0x20
#define AZT_RELATED_ADDRESSES_AMOUNT    15
const int AZT_RELATED_ADDRESS[] = {0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
                                   0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F };

#endif  // AZTLIB

typedef struct {
    int address;
    int cmd;
    char params[AZT_REQUEST_PARAMS_MAX_AMOUNT];
    int params_cnt;
} azt_request_t;

int azt_init(void);
int azt_rx_handler(void);
int azt_tx(char* data, int cnt);
int azt_tx_nak(void);
int azt_tx_ack(void);
int azt_tx_can(void);
azt_request_t* azt_request(void);
void azt_deinit();

#endif //SRC_LIBAZT_H