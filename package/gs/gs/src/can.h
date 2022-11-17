#ifndef CAN_H
#define CAN_H

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/can.h>

#include "settings.h"

#define CONFIG_FILENAME_MAX_LENGTH      64

typedef struct can_t can_t;

struct can_t {
    int busNumber;
    char bus[4];
    int deviceAddress;
    char config_filename_bus_number[CONFIG_FILENAME_MAX_LENGTH];
    char config_filename_device_address[CONFIG_FILENAME_MAX_LENGTH];
    int fd;
    struct can_frame frame;
    int (*transmit)(can_t* self, float volume, float price, float totalPrice);
    int (*transmit_half)(can_t* self, float volume, float totalPrice);
};

#define DEF_CONFIG_FILE_CAN_BUS_NUMBER              "/etc/gs/%d/can_busNumber"
#define DEF_CONFIG_FILE_CAN_DEVICE_ADDRESS          "/etc/gs/%d/can_deviceAddress"

#define CONFIG_FILE_CAN_BUS_NUMBER                  "/mnt/gs/%d/can_busNumber"
#define CONFIG_FILE_CAN_DEVICE_ADDRESS              "/mnt/gs/%d/can_deviceAddress"

int CAN_init(int idx, can_t* can);

#endif  // CAN_H
