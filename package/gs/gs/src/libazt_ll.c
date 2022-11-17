//
// Created by al on 22.07.22.
//
#include "libazt_ll.h"

static int azt_port_fd = 0;
static int azt_rts_fd = 0;

char read_buf[256];
int read_buf_len = 0;

char packet_buf[64];
int packet_buf_len = 0;

void (*request_parser)(char*, int);

int azt_port_init(void (*f)(char* rx_buf, int rx_buf_len)) {

    request_parser = f;

//    azt_rts_fd = open(DEF_AZT_RTS_GPIO, O_WRONLY | O_SYNC);
//    write(azt_rts_fd, "0", 1);  // Low by default to enable reading from RS-485
    usleep(100000);

    azt_port_fd = open(DEF_AZT_PORT, O_RDWR);
    if (azt_port_fd < 0) {
        printf("Error %i trying to open AZT port: %s\n", errno, strerror(errno));
    }

    struct termios tty;
    if (tcgetattr(azt_port_fd, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    tty.c_cflag |= PARENB; // Set parity bit, enabling parity
    tty.c_cflag &= ~PARODD; // Clear ODD parity bit, therefore even parity to be used
    tty.c_cflag |= CSTOPB; // Set stop field, two stop bits
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS7; // 7 bits per byte
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 1;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Save tty settings, also checking for error
    if (tcsetattr(azt_port_fd, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    memset(&read_buf, 0, sizeof(read_buf));
    memset(&packet_buf, 0, sizeof(packet_buf));
}

int azt_port_read(void) {
    read_buf_len = read(azt_port_fd, &read_buf, sizeof(read_buf));

    if (read_buf_len < 0) {
        printf("Error reading: %s", strerror(errno));
        return -1;
    }
    else if (read_buf_len == 0) {
        if( packet_buf_len > 0) {

            (*request_parser)(packet_buf, packet_buf_len);
//            char test_buf[11] = {0x7F, 0x02, 0x21, 0x5E, 0x4E, 0x31, 0x32, 0x4D, 0x03, 0x03, 0x5E};  // request param trk type
//            char test_buf[9] = {0x7F, 0x02, 0x23, 0x5C, 0x4E, 0x31, 0x03, 0x03, 0x6E};  // request params
//            char test_buf[9] = {0x7F, 0x02, 0x23, 0x5C, 0x50, 0x2F, 0x03, 0x03, 0x70};  // protocol version request
//            char test_buf[9] = {0x7F, 0x02, 0x23, 0x5C, 0x31, 0x4E, 0x03, 0x03, 0x51};  // trk status request
//            char test_buf[9] = {0x7F, 0x02, 0x23, 0x5C, 0x37, 0x48, 0x03, 0x03, 0x57};  // trk type
//            char test_buf[9] = {0x7F, 0x02, 0x23, 0x5C, 0x36, 0x49, 0x03, 0x03, 0x56};  // get counter
//            char test_buf[9] = {0x7F, 0x02, 0x23, 0x5C, 0x36, 0x49, 0x03, 0x03, 0x56};  // get counter
//            char test_buf[17] = {0x7F, 0x02, 0x23, 0x5C, 0x51, 0x2E, 0x31, 0x4E, 0x30, 0x4F, 0x30, 0x4F, 0x30, 0x4F, 0x03, 0x03, 0x70};  // set price
//            char test_buf[19] = {0x7F, 0x02, 0x23, 0x5C, 0x54, 0x2B, 0x30, 0x4F, 0x30, 0x4F, 0x33, 0x4C, 0x30, 0x4F, 0x30, 0x4F, 0x03, 0x03, 0x47};  // set fuel volume
//            char test_buf[9] = {0x7F, 0x02, 0x23, 0x5C, 0x32, 0x4D, 0x03, 0x03, 0x52};  // auth command
//            char test_buf[9] = {0x7F, 0x02, 0x23, 0x5C, 0x35, 0x4A, 0x03, 0x03, 0x55 };  // get full data
//            char test_buf_len = sizeof(test_buf);
//            (*request_parser)(test_buf, test_buf_len);

            memset(&packet_buf, 0, sizeof(packet_buf));
            packet_buf_len = 0;
            return 1;
        }
    }
    else {
        memcpy(packet_buf+packet_buf_len, read_buf, read_buf_len);
        packet_buf_len += read_buf_len;
    }
    return 0;
}

int azt_port_write(char* tx_buf, int tx_buf_len) {
//    write(azt_rts_fd, "1", 1);
//    usleep(20000);

    int ret = write(azt_port_fd, tx_buf, tx_buf_len);
    if(ret<0) {
        printf("Error writing: %s\n", strerror(errno));
        write(azt_rts_fd, "0", 1);
        return -1;
    } else if(ret != tx_buf_len) {
        printf("Error. Writen %d bytes instead of %d\n", ret, tx_buf_len);
    }
//    usleep(60000);
//    write(azt_rts_fd, "0", 1);
    return 0;
}

void azt_port_close(void) {
    close(azt_port_fd);
}
