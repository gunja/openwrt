#include "settings.h"

#define RX_BUF_SIZE         64

int parse_integer_config(char* filename) {
    int fd;
    char rx_buf[RX_BUF_SIZE];
    memset(rx_buf, 0, RX_BUF_SIZE);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error %i trying to open RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    int ret = read(fd, rx_buf, RX_BUF_SIZE);
    close(fd);
    if(ret == -1) {
        printf("Error %i trying to read RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    int res = strtol(rx_buf, NULL, 10);
    return res;
}

int parse_true_false_config(char* filename) {
    int fd;
    char rx_buf[RX_BUF_SIZE];
    memset(rx_buf, 0, RX_BUF_SIZE);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error %i trying to open RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    int ret = read(fd, rx_buf, 1);
    close(fd);
    if(ret != 1) {
        printf("Error trying to read RK config file %s. Expected 1 byte. Gotten %d bytes: %s\n", filename, ret, rx_buf);
        return -1;
    }

    int isEnabled = DIGIT_CHAR_TO_NUM(rx_buf[0]);

    if((isEnabled == TRUE) || (isEnabled == FALSE)) {
        return isEnabled;
    } else {
        printf("Error unknown config param in %s: %d\n", filename, isEnabled);
        return -1;
    }
}

int parse_float_config(char* filename, float* res) {
    int fd;
    char rx_buf[RX_BUF_SIZE];
    memset(rx_buf, 0, RX_BUF_SIZE);

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        printf("Error %i trying to open RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    int ret = read(fd, rx_buf, RX_BUF_SIZE);
    close(fd);
    if(ret == -1) {
        printf("Error %i trying to read RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    *res = strtof(rx_buf, NULL);
//    printf("float: %.2f\n", res);

    return 0;
}

int set_config(char* filename, char* data, int len) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC);
    if (fd < 0) {
        printf("Error %i trying to open RK config file (to overwrite it) %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }

    int ret = write(fd, data, len);
    if (ret == -1) {
        printf("Error %i trying to write to RK config file %s: %s\n", errno, filename, strerror(errno));
        return -1;
    }
    close(fd);
    return 0;
}