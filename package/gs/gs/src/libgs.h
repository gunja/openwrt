#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <gpiod.h>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>
#include <pthread.h>
#include <stdatomic.h>

#include "mmi.h"
#include "flomac.h"

#include "config.h"
#include "settings.h"

#define DEF_BAUDRATE    9600
#define DEF_ADDR 1
#define CONNESTION_LOST_COUNTER_VALUE	100

#define MODBUG_DELAY_US		60000

typedef struct measure_units {
    uint16_t massflow_unit;
    uint16_t density_unit;
    uint16_t temperature_unit;
    uint16_t volumeflow_unit;
    uint16_t dummy00;
    uint16_t pressure_unit;
    uint16_t mass_unit;
    uint16_t volume_unit;
} measure_units_t;


typedef struct measurements {
    float mass_flowrate;
    float density;
    float temprature;
    float volme_flowrate;
    float dummy01;
    float pressure;
    float mass_total;
    float volume_total;
    float mass_inventory;
    float volume_inventory;
} measurements_t;

typedef struct gs_conninfo {
    uint8_t port;
    uint32_t baudrate;
    uint8_t devaddr;
    pthread_t thread_id;
    modbus_t *ctx;
    measure_units_t measure_units;
    measurements_t measurements;
    _Atomic float summator_volume;
    _Atomic float summator_mass;
    _Atomic float mass_flowrate;
    int connection_lost_flag;
    int connection_lost_counter;
    int measurements_read_counter;
    int side;
} gs_conninfo_t;

/* port handling */
//int gs_init_pthreaded(int idx, gs_conninfo_t *conninfo);
modbus_t* gs_init(gs_conninfo_t *conninfo);
int gs_scan(gs_conninfo_t *conninfo);
void gs_close(modbus_t *ctx);
int gs_check_state(gs_conninfo_t* conninfo);


/*common functions */
int gs_get_version(modbus_t *ctx);
int gs_set_modbus_baudrate(gs_conninfo_t* conninfo, Flomac_Baudrate_t baudrate_);

int gs_get_all_units(modbus_t *ctx, measure_units_t *measure_units);

int gs_get_all_measurements(modbus_t *ctx, measurements_t *measurements);

int gs_set_mass_units(modbus_t *ctx, int mass_units);
int gs_set_volume_units(modbus_t *cts, int volume_units);

int gs_start_count(modbus_t *ctx);
int gs_stop_cunt(modbus_t *ctx);

/* Reset session counters */
int gs_reset_total_counters(modbus_t *ctx);

/* Reset total counters */
int gs_reset_inventory_counters(modbus_t *ctx); // Reset total counters

#define DEF_CONFIG_MODBUS_PORT              "/etc/gs/%d/modbus_port"
#define DEF_CONFIG_MODBUS_DEVICE_ADDRESS    "/etc/gs/%d/modbus_address"
#define DEF_CONFIG_MODBUS_BAUDRATE        	"/etc/gs/%d/modbus_baudrate"

#define CONFIG_MODBUS_PORT                  "/mnt/gs/%d/modbus_port"
#define CONFIG_MODBUS_DEVICE_ADDRESS        "/mnt/gs/%d/modbus_address"
#define CONFIG_MODBUS_BAUDRATE        		"/mnt/gs/%d/modbus_baudrate"
