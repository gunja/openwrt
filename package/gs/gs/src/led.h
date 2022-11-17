#ifndef LED_H
#define LED_H

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "settings.h"

#include <pthread.h>

#define CONFIG_FILENAME_MAX_LENGTH      64

#define INPUT_NOT_CONNECTED_THRESHOLD_VALUE     100

typedef struct led_t led_t;

typedef enum {
    off = 0x30,
    on = 0x31
} led_state_t;

struct led_t {
	int enabled;
    int normal_led_port;
    int normal_led_pin;
    int normal_led_number;
    led_state_t normal_led_state;
    char normal_led_filename[CONFIG_FILENAME_MAX_LENGTH];
    int32_t* normal_flag;
    int error_led_port;
    int error_led_pin;
    int error_led_number;
    led_state_t error_led_state;
    char error_led_filename[CONFIG_FILENAME_MAX_LENGTH];
    int32_t* error_flag;
    pthread_t thread_id;
};

#define VALUE_FILENAME                  "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw"

#define CONFIG_FILE_LED_ENABLED              "/mnt/gs/%d/led_enabled"
#define CONFIG_FILE_NORMAL_LED_PORT              "/mnt/gs/%d/led_normal_port"
#define CONFIG_FILE_NORMAL_LED_PIN              "/mnt/gs/%d/led_normal_pin"
#define CONFIG_FILE_ERROR_LED_PORT              "/mnt/gs/%d/led_error_port"
#define CONFIG_FILE_ERROR_LED_PIN              "/mnt/gs/%d/led_error_pin"

#define NORMAL_LED_FILE							"/sys/class/gpio/gpio%d/value"
#define ERROR_LED_FILE							"/sys/class/gpio/gpio%d/value"

int led_init(int idx, led_t* led, int32_t* error_flag, int* normal_flag);

#endif  // LED_H

