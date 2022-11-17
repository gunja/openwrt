//
// Created by al on 23.07.22.
//

#ifndef SRC_RK_H
#define SRC_RK_H

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include "libazt.h"
#include "settings.h"
#include "can.h"
#include "in_4_20_ma.h"
#include "error.h"
#include "libgs.h"
#include "led.h"
#include "relay.h"
#include "delay_counter.h"

typedef enum { right, left } rk_side;

typedef enum {
    trk_disabled_rk_installed = 0x30,
    trk_disabled_rk_taken_off = 0x31,
    trk_authorization_cmd = 0x32,
    trk_enabled_fueling_process = 0x33,
    trk_disabled_fueling_finished = 0x34,
    trk_disabled_local_control_unit_dose = 0x38,
	trk_enabled_fueling_process_local = 0x39
} trk_state_t;

typedef enum {
    trk_state_issue_less_or_equal_dose = 0x30,
    trk_state_issue_overdose = 0x31
} trk_state_issue_t;


#define SINGLE_VALVE	1
#define TWO_VALVE		2

#define CONFIG_FILENAME_MAX_LENGTH      64

#define PRICE_PER_LITER_DIGITS      6
#define VOLUME_DIGITS               6
#define PRICE_DIGITS                8

typedef struct rk_t rk_t;
struct rk_t {
    int enabled;
    rk_side side;
    trk_state_t state;
    int fueling_process_flag;
    trk_state_issue_t state_issue;
    int address;
    int pagz_mode_enabled;
    float summator_volume;
    float summator_price;
    float prev_summator_volume;
    float prev_summator_price;
    int (*azt_req_hndl)(azt_request_t* req, rk_t* self);
    int (*is_not_fault)(rk_t* self);
    int (*process)(rk_t* self);
    int (*stop)(rk_t* self);
    char config_filename_is_enabled[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_is_left[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_address[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_is_local_control_enabled[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_summator_price[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_summator_volume[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_price_per_liter[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_gas_density[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_relay_cut_off_timing[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_mass_flow_rate_threshold_value[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_valves_amount[CONFIG_FILENAME_MAX_LENGTH];
    float fueling_dose_in_liters;  // Заданная с АРМ доза к заправке в литрах
    float fueling_dose_in_rubles;  // Заданная с АРМ доза к заправке в рублях
    float fueling_price_per_liter;  // Заданная с АРМ стоимость топлива за литр
    float fueling_interrupted_volume;
    float fueling_interrupted_price;
    float fueling_current_volume;   // Текущий объем заправленного топлива в текущем цикле заправки
    float fueling_current_price;    // Текущая стоимость заправленного топлива в текущем цикле заправки
    float flomac_inv_mass_starting_value;
    float flomac_inv_mass;
    float flomac_mass_flowrate;
    int pressure_4_20ma_raw;
    float pressure_4_20ma;
    int fueling_current_finished_flag;
    can_t can_bus;
    in_4_20_t in_4_20;
    error_t error_state;
    gs_conninfo_t modbus;
    void (*btn_clbk_start)(int);
    void (*btn_clbk_stop)(int);
    int stop_button_pressed_flag;
    int reset_command_received_flag;
    led_t led;
    relay_t relay;
    float gas_density;
    float relay_cut_off_timing;	// упреждающее количества объема, оставшееся к заправке в бак, при котором происходит закрытие клапана
    float mass_flow_rate_threshold_value;  // пороговое значение скорости расхода газа, ниже которого считается, что бак полный и заправка завершена
    int valves_amount;  // количество клапанов, используемых в схеме заправки (1, 2). Если 2 клапана, - то один - клапан нижнего давления, второй - высокого давления
    delay_counter_t counter_stop_btn;  // счетчик выжидания результатов расходомера при остановке заправки кнопкой Stop
    delay_counter_t counter_reset_cmd;  // счетчик выжидания результатов расходомера при остановке по команде с GasKit
    int cnt;
    int store_prev_summators_flag;
};

#define DEF_CONFIG_FILE_IS_ENABLED                  "/etc/gs/%d/isEnabled"
#define DEF_CONFIG_FILE_IS_LEFT                     "/etc/gs/%d/isLeft"
#define DEF_CONFIG_FILE_ADDRESS                     "/etc/gs/%d/address"
#define DEF_CONFIG_FILE_IS_PAGZ_MODE_ENABLED    	"/etc/gs/%d/isPAGZmodeEnabled"
#define DEF_CONFIG_FILE_SUMMATOR_PRICE              "/etc/gs/%d/summatorPrice"
#define DEF_CONFIG_FILE_SUMMATOR_VOLUME             "/etc/gs/%d/summatorVolume"
#define DEF_CONFIG_FILE_PRICE_PER_LITER_MASK        "/etc/gs/%d/pricePerLiter"
#define DEF_CONFIG_FILE_GAS_DENSITY		            "/etc/gs/%d/setting_gas_density"
#define DEF_CONFIG_FILE_RELAY_CUT_OFF_TIMING		"/etc/gs/%d/setting_relay_cut_off_timing"
#define DEF_CONFIG_FILE_MASS_FLOW_RATE_THRESHOLD_VALUE  "/etc/gs/%d/setting_mass_flow_rate_threshold_value"
#define DEF_CONFIG_FILE_VALVES_AMOUNT  				"/etc/gs/%d/setting_valves_amount"


#define CONFIG_FILE_IS_ENABLED                  "/mnt/gs/%d/isEnabled"
#define CONFIG_FILE_IS_LEFT                     "/mnt/gs/%d/isLeft"
#define CONFIG_FILE_ADDRESS                     "/mnt/gs/%d/address"
#define CONFIG_FILE_IS_PAGZ_MODE_ENABLED    	"/mnt/gs/%d/isPAGZmodeEnabled"
#define CONFIG_FILE_SUMMATOR_PRICE              "/mnt/gs/%d/summatorPrice"
#define CONFIG_FILE_SUMMATOR_VOLUME             "/mnt/gs/%d/summatorVolume"
#define CONFIG_FILE_PRICE_PER_LITER_MASK        "/mnt/gs/%d/pricePerLiter"
#define CONFIG_FILE_GAS_DENSITY		            "/mnt/gs/%d/setting_gas_density"
#define CONFIG_FILE_RELAY_CUT_OFF_TIMING        	"/mnt/gs/%d/setting_relay_cut_off_timing"
#define CONFIG_FILE_MASS_FLOW_RATE_THRESHOLD_VALUE  "/mnt/gs/%d/setting_mass_flow_rate_threshold_value"
#define CONFIG_FILE_VALVES_AMOUNT  					"/mnt/gs/%d/setting_valves_amount"

int rk_init(int idx, rk_t* rk);

#endif //SRC_RK_H
