#ifndef RELAY_H
#define RELAY_H

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

#include "config.h"
#include <pthread.h>

#define CONFIG_FILENAME_MAX_LENGTH      64


typedef struct relay_t relay_t;

typedef enum {
    relay_off = 0x00,
	relay_on = 0x01
} relay_state_t;

struct relay_t {
    int relay_low_number;  		// K1 - left, 	K4 - right
    int relay_middle_number;  	// K2 - left, 	K5 - right
    int relay_high_number;  	// K3 - left, 	K6 - right
    relay_state_t relay_low_state;
    relay_state_t relay_middle_state;
    relay_state_t relay_high_state;
    char relay_low_filename[CONFIG_FILENAME_MAX_LENGTH];
    char relay_middle_filename[CONFIG_FILENAME_MAX_LENGTH];
    char relay_high_filename[CONFIG_FILENAME_MAX_LENGTH];
};

#define CONFIG_FILE_RELAY_LOW_NUMBER        	"/mnt/gs/%d/relay_low_number"
#define CONFIG_FILE_RELAY_MIDDLE_NUMBER         "/mnt/gs/%d/relay_middle_number"
#define CONFIG_FILE_RELAY_HIGH_NUMBER           "/mnt/gs/%d/relay_high_number"

#define RELAY_LOW_FILE							"/sys/class/gpio/gpio%d/value"
#define RELAY_MIDDLE_FILE						"/sys/class/gpio/gpio%d/value"
#define RELAY_HIGH_FILE							"/sys/class/gpio/gpio%d/value"

int relay_init(int idx, relay_t* relay);
int relay_low_on(relay_t* relay);
int relay_low_off(relay_t* relay);
int relay_low_is_on(relay_t* relay);

int relay_middle_on(relay_t* relay);
int relay_middle_off(relay_t* relay);
int relay_middle_is_on(relay_t* relay);

int relay_high_on(relay_t* relay);
int relay_high_off(relay_t* relay);
int relay_high_is_on(relay_t* relay);

#endif  // RELAY_H

