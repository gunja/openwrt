#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

#include "libgs.h"

#define DEFAULT_BAUD 9600
#define DEFAULT_ADDRESS 1
#define DEFAULT_DEVICE "/dev/ttymxc4"

int perform_flomac_dump(modbus_t *);
int perform_mmi_dump(modbus_t *);
int rude_raw_dump(modbus_t *, int, int);

int dump_region(modbus_t *, int, int);

int main(int argc, char * const argv[])
{
    int baud, opt, ret;
    int address;
    uint16_t device_mode;

    const char * device = DEFAULT_DEVICE;

    struct timeval tv;
    modbus_t *ctx;

    baud = DEFAULT_BAUD;
    address = DEFAULT_ADDRESS;

    while ((opt = getopt(argc, argv, "a:d:b:h")) != -1) {
        switch (opt) {
            case 'h': default:
                printf("%s [-d /dev/ttymxc4] [-b 9600] [-a 1] [-h]\n", argv[0]);
                exit(EXIT_SUCCESS);
            case 'a':
                address = atoi(optarg);
                break;
            case 'd':
                device = optarg;
                break;
            case 'b':
                baud = atoi(optarg);
                break;
        }
    }
    gettimeofday(&tv, NULL);
    struct tm rz;
    localtime_r(&tv.tv_sec, &rz);
    printf("# Starting registers dump on %d/%02d/%02d at %02d:%02d:%02d\n",
            rz.tm_year + 1900, rz.tm_mon, rz.tm_mday,
            rz.tm_hour, rz.tm_min, rz.tm_sec);
    printf("# dimp is done for bus \"%s\" at baud %04d device at address %d\n", device, baud, address);
    
    ctx = modbus_new_rtu(device, baud, 'N', 8, 1);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        exit(EXIT_FAILURE);
    }

    modbus_set_response_timeout(ctx, 0, 100000);
    modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP);
    ret = modbus_rtu_set_serial_mode(ctx, MODBUS_RTU_RS485);
    if (!ret)
    {
        fprintf(stderr, "Error setting mode to RS485!\n");
        modbus_free(ctx);
        exit(EXIT_FAILURE);
    }

    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        exit(EXIT_FAILURE);
    }

    modbus_set_slave(ctx, address);

    if (gs_read_reg(ctx, R_MMI_MBUS_MAP, 1, &device_mode) == -1)
    {
        fprintf(stderr, "Failed to read %d register. Exiting\n",
                R_MMI_MBUS_MAP);
        modbus_free(ctx);
        exit(EXIT_FAILURE);
    }
    printf("# received device mode = %d\n", device_mode);

    if(perform_flomac_dump(ctx) < 0)
    {
        fprintf(stderr, "Some error occured while dumping flomac registers.\nExiting\n");
        /*modbus_close(ctx);
        modbus_free(ctx);
        exit(EXIT_FAILURE);
        */
    }

    printf("RAW consequent dump\n");
    rude_raw_dump(ctx, 0, FLOMAK_MAX_SCAN_REG); 

    if(perform_mmi_dump(ctx) < 0)
    {
        fprintf(stderr, "Some error occured while dumping mmi registers.\nExiting\n");
        /*modbus_close(ctx);
        modbus_free(ctx);
        exit(EXIT_FAILURE);*/
    }
    printf("RAW consequent dump\n");
    rude_raw_dump(ctx, C_MMI_ENABLE_PCOMP, MMI_MAX_USED_REG); 

device_mode = Mode_MMI;
    gs_write_reg(ctx, R_MMI_MBUS_MAP, 1, &device_mode);
    //to force some delay before following closing methods
    gs_read_reg(ctx, R_MMI_MBUS_MAP, 1, &device_mode);
    
    modbus_close(ctx);
    modbus_free(ctx);
    exit(EXIT_SUCCESS);
}

int perform_flomac_dump(modbus_t *ctx)
{
    uint16_t fm_mode = Mode_Flomac;
    int i, read_out_size;

    if(gs_write_reg(ctx, R_MMI_MBUS_MAP, 1, &fm_mode) < 0)
    {
        fprintf(stderr, "Failed to set Flomac mode %d\n", __LINE__);
        return -1;
    }
    printf("# reading Flomac mode\n");
    // NB regs 300...312[313]  are READ access modified. may be holding
    // registers are used, but not documented anywhere
    printf("# Reading float-s from base address %d\n", REG_F_MASS_FLOW);
    read_out_size = REG_F_PRESSURE - REG_F_MASS_FLOW + 2;
    if( dump_region(ctx, REG_F_MASS_FLOW, read_out_size) < 0)
    {
        fprintf(stderr, "Failed to read float group of MASS_FLOW\n");
        return -2;
    }
    printf("# Reading drivegain at %d\n", REG_F_DRIVEGAIN);
    if( dump_region(ctx, REG_F_DRIVEGAIN, 2)  < 0)
    {
        fprintf(stderr, "Failed to read Drive Gain\n");
        //return -3;
    }
    printf("# Reading REG_F_SUM1 at %d\n",REG_F_SUM1 );
    if( dump_region(ctx, REG_F_SUM1, 2)  < 0)
    {
        fprintf(stderr, "Failed to read Summator 1 \n");
        //return -4;
    }
    printf("# Reading REG_F_SUM2 at %d\n", REG_F_SUM2 );
    if( dump_region(ctx, REG_F_SUM2, 2)  < 0)
    {
        fprintf(stderr, "Failed to read Summator 2 \n");
        //return -5;
    }
    printf("# Reading REG_F_SUM3 at %d\n", REG_F_SUM3 );
    if( dump_region(ctx, REG_F_SUM3, 2)  < 0)
    {
        fprintf(stderr, "Failed to read Summator 3 \n");
        //return -6;
    }
    printf("# Reading REG_F_SUM4 at %d\n", REG_F_SUM4 );
    if( dump_region(ctx, REG_F_SUM4, 2)  < 0)
    {
        fprintf(stderr, "Failed to read Summator 4 \n");
        //return -5;
    }
    printf("# Reading configs starting REG_F_DEVADDR at %d\n", REG_F_DEVADDR);
    if( dump_region(ctx, REG_F_DEVADDR, 4)  < 0)
    {
        fprintf(stderr, "Failed to read 4 starting REG_F_DEVADDR \n");
        //return -6;
    }
    printf("# Reading configs starting REG_F_MBUS_MODE at %d\n", REG_F_MBUS_MODE);
    if( dump_region(ctx, REG_F_MBUS_MODE, 1)  < 0)
    {
        fprintf(stderr, "Failed to read REG_F_MBUS_MODE \n");
        //return -7;
    }
    printf("# Reading configs starting REG_F_FLOAT_BYTEORDER at %d\n", REG_F_FLOAT_BYTEORDER);
    if( dump_region(ctx, REG_F_FLOAT_BYTEORDER, 1)  < 0)
    {
        fprintf(stderr, "Failed to read REG_F_FLOAT_BYTEORDER \n");
        //return -8;
    }
    printf("# Reading configs starting REG_F_HWVER at %d\n", REG_F_HWVER);
    if( dump_region(ctx, REG_F_HWVER, 6)  < 0)
    {
        fprintf(stderr, "Failed to read REG_F_HWVER \n");
        //return -8;
    }
    
    // TODO dump remaining registers
    return 9;
}

int perform_mmi_dump(modbus_t *ctx)
{
        uint16_t fm_mode =Mode_MMI;
    int i, read_out_size;
    int ret, ret2;
    uint8_t coil, icoil;

    if(gs_write_reg(ctx, R_MMI_MBUS_MAP, 1, &fm_mode) < 0)
    {
        fprintf(stderr, "Failed to set MMI mode %d\n", __LINE__);
        return -1;
    }
    printf("# reading MMI mode\n");
    // dump coils
    // @1[4] ; @55[2]; @81[1]
    int coil_addresses[] = {  C_MMI_STARTSTOP,
            C_MMI_RESET_TOTALS,  C_MMI_RESET_INVENT,
            C_MMI_START_ZEROING, C_MMI_RESET_MASS_TOTAL,
            C_MMI_RESET_VOLUME_TOTAL, C_MMI_ENABLE_PCOMP
    };
    printf("# reading MMI coils/input bits\n");
    for(i=0; i < sizeof(coil_addresses)/sizeof(int); ++i)
    {
        usleep(MODBUG_DELAY_US);
        ret = modbus_read_bits(ctx, coil_addresses[i], 1, &coil);
        usleep(MODBUG_DELAY_US);
        ret2 = modbus_read_input_bits(ctx, coil_addresses[i], 1, &icoil);
        printf("%03d->\t", coil_addresses[i]); 
        if (ret > 0) { printf("%s\t", coil?"TRUE":"FALSE");}
        else { printf("XXXX\t");}
        if(ret2 > 0) { printf("%s\n", icoil?"TRUE":"FALSE");}
        else { printf("XXXX\n");}
    }
    // dump all required registers
    // ro 15; 119; 120; 67; 68; 69; 70 -- already declared
    printf("# Reading SW ver %d\n", R_MMI_SW_VER);
    if( dump_region(ctx, R_MMI_SW_VER, 1)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_SW_VER \n");
        return -3;
    }
    printf("# Reading DEVTYPE_CODE (should 40) MANUF_CODE (should 31) %d\n", R_MMI_DEVTYPE_CODE);
    if( dump_region(ctx, R_MMI_DEVTYPE_CODE, 2)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_DEVTYPE_CODE \n");
        return -4;
    }
    printf("# Reading 4 from R_MMI_HART_T01 %d\n", R_MMI_HART_T01);
    if( dump_region(ctx, R_MMI_HART_T01, 4)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_HART_T01 \n");
        return -5;
    }
    printf("# Reading 4 from R_MMI_HART_T01 %d\n", R_MMI_HART_T01);
    if( dump_region(ctx, R_MMI_HART_T01, 4)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_HART_T01 \n");
        return -6;
    }
    // rw @312; 436; 520, -- declared
    printf("# Reading from R_MMI_MBUS_ADDR  %d\n", R_MMI_MBUS_ADDR);
    if( dump_region(ctx, R_MMI_MBUS_ADDR, 1)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_MBUS_ADDR \n");
        return -7;
    }
    printf("# Reading from R_MMI_MBUS_MAP  %d\n", R_MMI_MBUS_MAP);
    if( dump_region(ctx,R_MMI_MBUS_MAP , 1)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_MBUS_MAP  \n");
        return -8;
    }
    printf("# Reading from R_MMI_FPBO  %d\n", R_MMI_FPBO);
    if( dump_region(ctx, R_MMI_FPBO, 1)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_FPBO  \n");
        return -9;
    }
    // ro @246-265 [?20] -- decl main params;
    printf("# Reading from R_MMI_MEASUREMENTS_BASE  %d\n",
            R_MMI_MEASUREMENTS_BASE);
    if( dump_region(ctx, R_MMI_MEASUREMENTS_BASE, 20)  < 0)
    {
        fprintf(stderr, "Failed to read \n");
        return -10;
    }

    // ro @284-293 [?10] -- already decl ; @124, @125; @418; -- corrected
    printf("# Reading from R_MMI_DIAG_BASE  %d\n", R_MMI_DIAG_BASE);
    if( dump_region(ctx,R_MMI_DIAG_BASE , 10)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_DIAG_BASE \n");
        return -11;
    }
    printf("# Reading from R_MMI_STATE_BITMASK1  %d\n",
                R_MMI_STATE_BITMASK1 );
    if( dump_region(ctx, R_MMI_STATE_BITMASK1, 2)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_STATE_BITMASK1 \n");
        return -12;
    }
    printf("# Reading from  R_MMI_STATE_BITMASK3 %d\n", 
            R_MMI_STATE_BITMASK3);
    if( dump_region(ctx, R_MMI_STATE_BITMASK3, 1)  < 0)
    {
        fprintf(stderr, "Failed to read R_MMI_STATE_BITMASK3\n");
        return -13;
    }
    // rw @38-45 -- declared;
    printf("# Reading from  R_MMI_UNITS_BASE %d\n", R_MMI_UNITS_BASE);
    if( dump_region(ctx, R_MMI_UNITS_BASE, 8)  < 0)
    {
        fprintf(stderr, "Failed to read 8 from R_MMI_UNITS_BASE\n");
        return -14;
    }
    // rw  @148[2]; @194[4 + 4]; @450[2]; @140[2]; -- appended
    printf("# Reading from REG_MMI_RW_F_CUTOFF_FOR_DENSITY  %d\n",
            REG_MMI_RW_F_CUTOFF_FOR_DENSITY);
    if( dump_region(ctx, REG_MMI_RW_F_CUTOFF_FOR_DENSITY, 2)  < 0)
    {
        fprintf(stderr,
            "Failed to read 2 from REG_MMI_RW_F_CUTOFF_FOR_DENSITY\n");
        return -15;
    }
    printf("# Reading from REG_MMI_RW_F_MASS_FLOW_CUTOFF  %d\n",
            REG_MMI_RW_F_MASS_FLOW_CUTOFF);
    if( dump_region(ctx,REG_MMI_RW_F_MASS_FLOW_CUTOFF , 8)  < 0)
    {
        fprintf(stderr,
            "Failed to read 8 from REG_MMI_RW_F_MASS_FLOW_CUTOFF \n");
        return -16;
    }
    printf("# Reading from REG_MMI_RW_F_EXTERNAL_PRESSURE_VALUE  %d\n",
            REG_MMI_RW_F_EXTERNAL_PRESSURE_VALUE);
    if( dump_region(ctx, REG_MMI_RW_F_EXTERNAL_PRESSURE_VALUE, 2)  < 0)
    {
        fprintf(stderr,
          "Failed to read 2 from REG_MMI_RW_F_EXTERNAL_PRESSURE_VALUE\n");
        return -17;
    }
    printf("# Reading from REG_MMI_RW_F_SLUG_FLOW_DURATION  %d\n",
        REG_MMI_RW_F_SLUG_FLOW_DURATION);
    if( dump_region(ctx, REG_MMI_RW_F_SLUG_FLOW_DURATION, 2)  < 0)
    {
        fprintf(stderr,
          "Failed to read 2 from REG_MMI_RW_F_SLUG_FLOW_DURATION\n");
        return -18;
    }
    return 19;
}

int dump_region(modbus_t *ctx, int base, int read_out_size)
{
    uint16_t storage[100];
    int i;

    if( read_out_size > 100)
        return -1000;
    if( read_out_size < 0)
        return read_out_size;

    if(  gs_read_reg(ctx, base, read_out_size, storage) < 0)
    {
        fprintf(stderr, "Failed to read and dump group\n");
        return -2;
    }
    for(i = 0; i < read_out_size; ++i)
    {
        printf("%03d -> %04X\n", base + i, storage[i]);
    }
    return read_out_size;
}

int rude_raw_dump(modbus_t * ctx, int coils, int reg)
{
    int i, ret, ret2;
    uint16_t regDest, hregDest;
    uint8_t coil, icoil;
    for(i = 0; i < coils; ++i)
    {
        usleep(MODBUG_DELAY_US);
        ret = modbus_read_bits(ctx, i, 1, &coil);
        usleep(MODBUG_DELAY_US);
        ret2 = modbus_read_input_bits(ctx, i, 1, &icoil);
        printf("\tc%04d->\t", i);
        if (ret > 0) { printf("%s\t", coil?"TRUE":"FALSE");}
        else { printf("XXXX\t");}
        if(ret2 > 0) { printf("%s\n", icoil?"TRUE":"FALSE");}
        else { printf("XXXX\n");}
    }

    for(i =0; i <= reg; ++i)
    {
        usleep(MODBUG_DELAY_US);
        ret = modbus_read_input_registers(ctx, i, 1, &regDest);
        usleep(MODBUG_DELAY_US);
        ret2 = modbus_read_registers(ctx, i, 1, &hregDest);
        printf("\t%04d->\t", i);
        if (ret > 0) { printf("%04X\t", regDest);}
        else { printf("XXXX\t");}
        if(ret2 > 0) { printf("%04X\n", hregDest); }
        else { printf("XXXX\n");}
    }
}

