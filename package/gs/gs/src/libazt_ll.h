//
// Created by al on 22.07.22.
//

#ifndef SRC_LIBAZT_LL_H
#define SRC_LIBAZT_LL_H

#include <stdio.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define DEF_AZT_PORT "/dev/ttymxc2"  // HW dependent config so hard coded

#define DEF_AZT_RTS_GPIO "/sys/class/gpio/gpio129/value"

int azt_port_init(void (*f)(char* rx_buf, int rx_buf_len));
int azt_port_read(void);
int azt_port_write(char* tx_buf, int tx_buf_len);
void azt_port_close(void);

#endif //SRC_LIBAZT_LL_H
