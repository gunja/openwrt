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
#include <string.h>

#include "libgs.h"


struct _configuration_prop
{
    Flomac_Mode_t attribution;
    uint8_t isCoil;
    uint16_t address;
    uint16_t value;
};

#define DEFAULT_MODBUS_PATH "/dev/ttymxc4"
#define DEFAULT_DEVICE_ADDR 1
#define MB_HIGH_BAUDRATE    115200
#define MB_NMI_BAUDRATE 9600

int g_verbosity = 100;
#define DEGUB_VERB 10

int open_connect_modbus(modbus_t **, const char *, int);
int settingsSortDesc(const void *, const void *);

int print_help(const char *name)
{
    printf("Calling conversion: %s [-d device_file] [-a address] {[-s]|-r}\n", name);
    printf("where:  -d device_file\t\tselects which file will be opened for communucation with device. Default is %s\n", DEFAULT_MODBUS_PATH);
    printf("where:  -a address\t\tsets which address of slave device will be used. Default is %d. Note!!! none boundaries are checked. On responsibility of a caller\n", DEFAULT_DEVICE_ADDR);
    printf("        -s            \t\tselects straight connection. This is default and can be ommitted\n");
    printf("        -r            \t\tselects reversed connection\n");
    return 0;
}

int main(int argc, char * const argv[])
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

        struct _configuration_prop SETTINGS[]=
        {
            // TODO reserve placeholders to alter in case of direct/inverse settings
            { Mode_Flomac, 1, REG_RW_I_SUM_MODE1, 1 }
            ,{ Mode_Flomac, 1, REG_RW_I_SUM_MODE2, 1 }
            ,{ Mode_Flomac, 1, REG_RW_I_SUM_MODE3, 1 }
            , { Mode_Flomac, 1, REG_RW_I_SUM_MODE4, 1 }
            , { Mode_Flomac, 1, REG_RW_I_UNIT_MASS_FLOW, 3 }
            , { Mode_MMI, 0, R_MMI_MASSFLOW_U, 73 }
            , { Mode_Flomac, 1, REG_RW_I_UNIT_VOLUME_FLOW, 6 }
            , { Mode_MMI, 0, R_MMI_VOLFLOW_U, 24 }
            // there's no MMI registers for SUM
            , { Mode_Flomac, 1, REG_RW_I_SUM_FAILSAFE_MODE, 2 }
            , { Mode_Flomac, 1, REG_RW_I_SUM_STATE1, 0}
            , { Mode_Flomac, 1, REG_RW_I_SUM_STATE2, 0}
            , { Mode_Flomac, 1, REG_RW_I_SUM_STATE3, 0}
            , { Mode_Flomac, 1, REG_RW_I_SUM_STATE4, 0}
            , { Mode_Flomac, 1, REG_RW_I_SUM_UNIT3, 1}
            , { Mode_MMI, 0, R_MMI_MASS_U, MASS_UNIT_KG}
            , { Mode_Flomac, 0, REG_RW_I_SUM_RESET3, 1}
            , { Mode_Flomac, 1, REG_RW_I_SUM_ASSIGN4, 2}
            , { Mode_Flomac, 1, REG_RW_I_SUM_UNIT4, 1}
            , { Mode_MMI, 0, R_MMI_VOLUME_U, 41}
            , { Mode_Flomac, 0, REG_RW_I_SUM_RESET4, 1}
            // NOTE in example it's set to 2, from image should be 1
            , { Mode_Flomac, 1, REG_RW_I_ASS_LOW_FLOW_CUTOFF, 1}
            , { Mode_Flomac, 1, REG_RW_I_ASS_LOW_FLOW_UNIT, 3}
            //already set earlier  , { Mode_MMI, 0, R_MMI_MASSFLOW_U, 73 }
            , { Mode_Flomac, 1, REG_RW_F_DENSITY_CUTOFF, 0xCCCD}
            , { Mode_Flomac, 1, REG_RW_F_DENSITY_CUTOFF + 1, 0x3E4C}
            , { Mode_MMI, 0, REG_MMI_RW_F_CUTOFF_FOR_DENSITY, 0xCCCD}
            , { Mode_MMI, 0, REG_MMI_RW_F_CUTOFF_FOR_DENSITY + 1, 0x3E4C}
            , { Mode_Flomac, 1, REG_RW_I_SLUG_FLOW_ENABLE, 1}
            , { Mode_Flomac, 1, REG_RW_F_SLUG_FLOW_BREAK_TIME, 0}
            , { Mode_Flomac, 1, REG_RW_F_SLUG_FLOW_BREAK_TIME + 1, 0}
            , { Mode_MMI, 0, REG_MMI_RW_F_SLUG_FLOW_DURATION, 0}
            , { Mode_MMI, 0, REG_MMI_RW_F_SLUG_FLOW_DURATION + 1, 0}
            , { Mode_Flomac, 1, REG_RW_F_SLUG_FLOW_LO_LIMIT, 0}
            , { Mode_Flomac, 1, REG_RW_F_SLUG_FLOW_LO_LIMIT + 1, 0}
            , { Mode_MMI, 0, REG_MMI_RW_F_SLUG_FLOW_LOW_LIMIT, 0}
            , { Mode_MMI, 0, REG_MMI_RW_F_SLUG_FLOW_LOW_LIMIT + 1, 0}
            , { Mode_Flomac, 1, REG_RW_F_SLUG_FLOW_HI_LIMIT, 0}
            , { Mode_Flomac, 1, REG_RW_F_SLUG_FLOW_HI_LIMIT + 1, 0x40A0}
            , { Mode_MMI, 0, REG_MMI_RW_F_SLUG_FLOW_HIGH_LIMIT, 0}
            , { Mode_MMI, 0, REG_MMI_RW_F_SLUG_FLOW_HIGH_LIMIT + 1, 0x40A0}
            , { Mode_Flomac, 1, REG_RW_I_FIX_PRESSURE_UNIT, 1}
            , { Mode_MMI, 0, R_MMI_PRESSURE_U, 7}
            , { Mode_Flomac, 1, REG_RW_F_FIX_PRESSURE_INPUT, 0x6128}
            , { Mode_Flomac, 1, REG_RW_F_FIX_PRESSURE_INPUT + 1, 0x4004}
            , { Mode_MMI, 0, REG_MMI_RW_F_EXTERNAL_PRESSURE_VALUE, 0x6128}
            , { Mode_MMI, 0, REG_MMI_RW_F_EXTERNAL_PRESSURE_VALUE+1, 0x4004}
            , { Mode_Flomac, 1, REG_RW_I_UNIT_PRESSURE, 1}
            // already set , { Mode_MMI, 0, R_MMI_PRESSURE_U, 7}
        };
        if ( is_straight)
        {
            // TODO replace placeholders in SETTINGS to match for straight
            SETTINGS[0].value = 1;
            SETTINGS[1].value = 1;
            SETTINGS[2].value = 1;
            SETTINGS[3].value = 1;
        } else {
            // TODO replace placeholders in SETTINGS to match for reversed
            SETTINGS[0].value = 2;
            SETTINGS[1].value = 2;
            SETTINGS[2].value = 2;
            SETTINGS[3].value = 2;
        }

        int i;
        int settings_size= sizeof(SETTINGS)/sizeof(struct _configuration_prop);

        // 0.  records in "settings table": sort by attribution: Flomac settings go first
        qsort(SETTINGS, settings_size,
            sizeof(struct _configuration_prop), settingsSortDesc);
        // for each record in "settings table" with Flomac attribution
        //      apply setting
        for(i=0; i < settings_size &&
            SETTINGS[i].attribution == Mode_Flomac; ++i)
        {
            printf("Flomac setting %d to %04X -> ", SETTINGS[i].address,
                    SETTINGS[i].value);
            ret = gs_write_reg(ctx, SETTINGS[i].address, 1,
                &SETTINGS[i].value);
            printf("%d\n", ret);
        }
        // switch mapping to MMI
        uint16_t mmi_mode = Mode_MMI;
        ret = gs_write_reg(ctx, REG_F_MBUS_MODE, 1, &mmi_mode);
        //  for each record in "settings table" with MMI attribution:
        //      apply setting
        for(; i < settings_size && SETTINGS[i].attribution == Mode_MMI
            && SETTINGS[i].isCoil != 0; ++i)
        {
            printf("MMI coils setting %d to %s->", SETTINGS[i].address,
                SETTINGS[i].value?"TRUE": "FALSE");
            ret=modbus_write_bit(ctx, SETTINGS[i].address, SETTINGS[i].value);
            printf("%d\n", ret);
        }
        for(; i < settings_size && SETTINGS[i].attribution == Mode_MMI; ++i)
        {
            printf("MMI register setting %d to %04X->", SETTINGS[i].address,
                SETTINGS[i].value);
            ret=modbus_write_register(ctx, SETTINGS[i].address,
                SETTINGS[i].value);
            printf("%d\n", ret);
        }
        // here should be: all settings written, speed is 9600, mode: MMI

        modbus_close(ctx);
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

int settingsSortDesc(const void *a, const void *b)
{
    struct _configuration_prop * left = (struct _configuration_prop*)a;
    struct _configuration_prop * right = (struct _configuration_prop*)b;
    if(left->attribution != right->attribution)
    {
        if( left->attribution < right->attribution)
            return -1;
        return 1;
    }
    if( left->isCoil != right->isCoil)
    {
        if (left->isCoil)
            return -1;
        return 1;
    }
    if( left->address != right->address)
    {
        return left->address - right->address;
    }

    return left - right;
}
