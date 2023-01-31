/**
    \file mm_su.c  entry point of Mass-meter setup application.
    \brief This app sets up required settings within mass-meter produced by Elmetro

    There are 3 arguments for an app: port (path to device file) to be opened,
    direct or reverse selection of working
    and (optional) device address to be communicated

    First, this app parses command line arguments, after that it tries to
    connect with device at address 1 @ 115200 baudrate as flomac
    if successfull - switches to NMI and to 9600 bauds

    connects with NMI @ 9600 and stores required settings

**/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <modbus/modbus.h>
#include <modbus/modbus-rtu.h>
#include <errno.h>

#include "flomac.h"
#include "mmi.h"

#define DEFAULT_MODBUS_PATH "/dev/ttymxc4"
#define DEFAULT_DEVICE_ADDR 1
#define MB_HIGH_BAUDRATE    115200
#define MB_NMI_BAUDRATE 9600

int g_verbosity = 100;
#define DEGUB_VERB 10

int open_connect_modbus(modbus_t **, const char *, int);

int print_help(const char *name)
{
    printf("Calling conversion: %s [-d device_file] [-a address] {[-s]|-r}\n", name);
    printf("where:  -d device_file\t\tselects which file will be opened for communucation with device. Default is %s\n", DEFAULT_MODBUS_PATH);
    printf("where:  -a address\t\tsets which address of slave device will be used. Default is %s. Note!!! none boundaries are checked. On responsibility of a caller\n", DEFAULT_DEVICE_ADDR);
    printf("        -s            \t\tselects straight connection. This is default and can be ommitted\n");
    printf("        -r            \t\tselects reversed connection\n");
    return 0;
}

int main(int argc, const char * argv[])
{
    char * device_to_open= NULL;
    int is_straight = 1;
    int need_clean = 0;
    int opt;
    int address = DEFAULT_DEVICE_ADDR;
    modbus_t * ctx = NULL;
    int status = EXIT_SUCCESS;

    while ((opt = getopt(argc, argv, "hsrd:")) != -1)
    {
        switch(opt) {
            case 's':
                is_straight = 1;
                break;
            case 'r':
                is_straight = 0;
                break;
            case 'd':
                device_to_open= optarg;
            case 'a':
                address = atoi(optarg);
                if( address <= 0)
                    address = 1;
                break;
            case 'h':
                print_help(argv[0]);
                exit(status);
            default:
                print_help(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (NULL == device_to_open)
    {
        device_to_open = strdup(DEFAULT_MODBUS_PATH);
        need_clean = 1;
    }

    printf("Working on %s. Setting mode is %s\n",
            device_to_open, is_straight?"straight":"reversed");

    if (open_connect_modbus(&ctx, device_to_open, MB_HIGH_BAUDRATE) == 0)
    {
        modbus_set_slave(ctx, address);
        uint16_t bus_mapping;
        int need_speed_change = 1;
        int ret;
        // 1) can we even communicate? (is connection speed correct?)
        ret = gs_read_reg(ctx, R_MMI_MBUS_MAP, 1, &bus_mapping);
        if (ret < 0)
        {
            modbus_free(ctx); ctx = NULL;
            if(open_connect_modbus(&ctx, device_to_open, MB_NMI_BAUDRATE) == 0) {
                modbus_set_slave(ctx, address);
                ret = gs_read_reg(ctx, R_MMI_MBUS_MAP, 1, &bus_mapping);
                if (ret < 0) {
                    fprintf(stderr, "Failed to fetch MBUS MAP at any of 115200 or 9600 baud\n.Giving up\n");
                    modbus_free(ctx);
                    status = EXIT_FAILURE;
                    goto clean_up;
                }
                need_speed_change = 0;
            } else {
                fprintf(stderr, "Failed to open and connect device with %d baudrate\n.Exiting\n", MB_NMI_BAUDRATE);
                status = EXIT_FAILURE;
                goto clean_up;
            }
        }
        // 2) which register MAP is active?  R_MMI_MBUS_MAP  or REG_F_MBUS_MODE
        if( g_verbosity >= DEGUB_VERB) { printf("MBUS_MAP register value is %04X\n", bus_mapping); }
        if ( bus_mapping != Mode_Flomac)
        {
            uint16_t mode = Mode_Flomac;
            ret = gs_write_reg(ctx, R_MMI_MBUS_MAP, 1, &mode);
            if( ret == -1) {
                fprintf(stderr, "Failure on setting Mode to Flomac.\nForcing exit\n");
                modbus_free(ctx);
                status = EXIT_FAILURE;
                goto clean_up;
            }
        }
        if (need_speed_change)
        {
            uint16_t baudrate = Baudrate_9600;
            ret =  gs_write_reg(ctx, REG_F_BAUDRATE, 1, &baudrate);
            if ( -1 == ret) {
                fprintf(stderr, "Failure on setting baudrate to %d.\nForcing exit\n", baudrate);
                modbus_free(ctx);
                status = EXIT_FAILURE;
                goto clean_up;
            }
            usleep(100000);
            if( g_verbosity >= DEGUB_VERB) { printf("Starting re-connection to device\n");}
            modbus_free(ctx); ctx = NULL;
            if ( open_connect_modbus(&ctx, device_to_open,  MB_NMI_BAUDRATE) != 0)
            {
                fprintf(stderr, "Failure on re-initializing connection with %d baudrate.\nExiting\n", MB_NMI_BAUDRATE);
                modbus_free(ctx);
                status = EXIT_FAILURE;
                goto clean_up;
            }
            modbus_set_slave(ctx, address);
            need_speed_change = 0;
        }
        // 0.  records in "settings table": sort by attribution: Flomac settings go first
        // for each record in "settings table" with Flomac attribution
        //      apply setting
        // switch mapping to MMI
        //  for each record in "settings table" with MMI attribution:
        //      apply setting

        // here should be: all settings written, speed is 9600, mode: MMI

        // 3) NO NEED. no communication of not in match ||| depending on MAP, read out connection speed. is it in match? REG_F_BAUDRATE for FLOMAC,
        // 4) - II  -  DONE if we are NOT in FloMAC - switch to Flomac
        // 5) - IV  - write all recommended field before switching, switch speed
        // 6) - III - DONE reconnect with new speed
        // 7) write required fields

        modbus_free(ctx);
    } else {
        fprintf(stderr, "Unable to create the libmodbus context\n");
    }

clean_up:
    if(need_clean)
    {
        free(device_to_open);
    }
    exit(status);
};

int open_connect_modbus(modbus_t **ctx, const char *device, int baudrate)
{
    int ret;
    *ctx = modbus_new_rtu(device, baudrate, 'N', 8, 1);
    if (*ctx == NULL)
    {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return -2;
    }

    modbus_set_response_timeout(*ctx, 0, 100000);
    // modbus_set_debug(ctx, TRUE);
    modbus_rtu_set_rts(*ctx, MODBUS_RTU_RTS_UP);
    ret = modbus_rtu_set_serial_mode(*ctx, MODBUS_RTU_RS485);
    if (!ret)
    {
        fprintf(stderr, "Error setting mode to RS485!\n");
        modbus_free(*ctx);
        *ctx = NULL;
        return -3;
    }

    if (modbus_connect(*ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(*ctx);
        *ctx = NULL;
        return -4;
    }
    return 0;
}

