#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

//#include "libgs.h"
#include "libazt.h"
#include "rk.h"
#include "buttons.h"

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

int main(int argc, char* argv[])
{
    int ret;
	modbus_t *ctx;
    ret = gs_scan(&gs2_conn);
    printf("gs2_conninfo discovered slave device address: %d\r\n", ret);

    ret = gs_scan(&gs1_conn);
    printf("gs1_conninfo discovered slave device address: %d\r\n", ret);
	return 0;
}
