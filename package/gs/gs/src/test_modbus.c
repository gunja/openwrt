#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <stdlib.h>

//#include "libgs.h"
#include "libazt.h"
#include "rk.h"
#include "buttons.h"

#define DEFAULT_MAX_SCAN_ADDR 3
#define DEFAULT_BAUD_RATE 9600


// 1st inst
gs_conninfo_t gs1_conn =
{
	.port = 4,
	.baudrate = 9600,
	.devaddr = DEF_ADDR
};

// 2nd inst
gs_conninfo_t gs2_conn =
{
	.port = 5,
	.baudrate = 9600,
	.devaddr = DEF_ADDR
};

int print_help()
{
    printf("Calling conversion:\n");
    printf("test_modbus [-a 3] [-b 9600] [-h]\n");
    printf("Where:\n");
    printf("\t-a max_addr \t\tset maximal address used while scanning (default is 3)\n");
    printf("\t-b baudrate \t\tset baudrate of communication. Supported bauds are 9600, 19200, 38400, 57600, 115200. 9600 is default\n");
    printf("\t-h          \t\t prints this help and exits\n");
    return 0;
};

int main(int argc, char* argv[])
{
    int ret;
    int max_addr = DEFAULT_MAX_SCAN_ADDR, a_cand;
    int baud = DEFAULT_BAUD_RATE, b_cand;

    int opt;

    while ((opt = getopt(argc, argv, "ha:b:")) != -1) {
        switch(opt)
        {
            case 'h':
                print_help();
                exit(EXIT_SUCCESS);
            case 'a':
                a_cand = atoi(optarg);
                if(a_cand > 0 && a_cand <= 247)
                    max_addr = a_cand;
                break;
            case 'b':
                b_cand = atoi(optarg);
                if ( (b_cand == 9600)  || (b_cand == 19200) || ( b_cand == 38400 )
                    || (b_cand == 57600 ) || (b_cand == 115200) )
                    baud = b_cand;
                break;
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }

    gs1_conn.baudrate = baud;
    gs2_conn.baudrate = baud;

    ret = gs_scan_ext(&gs2_conn, max_addr);
    printf("gs2_conninfo discovered slave device address: %d\r\n", ret);

    ret = gs_scan_ext(&gs1_conn, max_addr);
    printf("gs1_conninfo discovered slave device address: %d\r\n", ret);
	return 0;
}
