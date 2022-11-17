#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

//#include "libgs.h"
//#include "libazt.h"
//#include "rk.h"
//#include "buttons.h"
#include <sys/ioctl.h>
#include <pthread.h>


int fd3;
int fd5;
int fd6;

pthread_t thread6;
pthread_t thread5;
pthread_t thread2;

int status;
int ret;

char read_buf6[5] = {0};
char read_buf[5] = {0};
char read_buf2[5] = {0};
char read_buf3[5] = {0};
char read_buf5[5] = {0};

void init(int fd);

int read_ttymxc6(void) {
	memset(read_buf6, 0, sizeof(read_buf6));
	while(1) {
		int ret = read(fd6, &read_buf6, sizeof(read_buf6));
		if(ret > 0 ) {
		if( (read_buf6[0] == 'B') && (read_buf2[0]=='F')) {
			printf("UART6 test passed\r\n");
			exit(0);
		}
		}
	}
}

int read_ttymxc5(void) {
	memset(read_buf5, 0, sizeof(read_buf5));
	while(1) {
		int ret = read(fd5, &read_buf5, sizeof(read_buf5));
		if(ret > 0 ) {
		if( (read_buf5[0] == 'B') && (read_buf3[0]=='F')) {
			printf("UART5 test passed\r\n");
			exit(0);
		}
		}
	}
}

int main(int argc, char* argv[])
{

    fd3 = open("/dev/ttymxc2",O_RDWR );//Open Serial Port
    fd5 = open("/dev/ttymxc4",O_RDWR  | O_NONBLOCK);//Open Serial Port
    fd6 = open("/dev/ttymxc5",O_RDWR  | O_NONBLOCK);//Open Serial Port

    init(fd3);
    init(fd5);
    init(fd6);

    /*****************************************************************/
    /*************** UART3 <--> UART6 ********************************/
    /*****************************************************************/


  	pthread_create(&thread6, NULL, read_ttymxc6, NULL);
 	/************** backward data flow ********************/
    ioctl(fd6,TIOCMGET,&status);//Set RTS pin
 	status |= TIOCM_RTS;
 	ioctl(fd6, TIOCMSET, &status);
 	usleep(100000);

 	ret = write(fd6, "F", 1);
 	memset(read_buf2, 0, sizeof(read_buf2));
 	ret = read(fd3, &read_buf2, sizeof(read_buf2));

    /************* forward data flow ********************/
    ioctl(fd6,TIOCMGET,&status);//Set RTS pin
  	status &= ~TIOCM_RTS;
  	ioctl(fd6, TIOCMSET, &status);
 	usleep(100000);

  	ret = write(fd3, "B", 1);

    /*****************************************************************/
    /*************** UART3 <--> UART5 ********************************/
    /*****************************************************************/

  	pthread_create(&thread5, NULL, read_ttymxc5, NULL);
    /************** backward data flow ********************/
    ioctl(fd5,TIOCMGET,&status);//Set RTS pin
    status |= TIOCM_RTS;
    ioctl(fd5, TIOCMSET, &status);
    usleep(100000);

    ret = write(fd5, "F", 1);
    memset(read_buf3, 0, sizeof(read_buf3));
    ret = read(fd3, &read_buf3, sizeof(read_buf3));

    /************* forward data flow ********************/
    ioctl(fd5,TIOCMGET,&status);//Set RTS pin
 	status &= ~TIOCM_RTS;
 	ioctl(fd5, TIOCMSET, &status);
	usleep(100000);

 	ret = write(fd3, "B", 1);

 	while(1);
    return 0;
}
	

void init(int fd) {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        return 1;
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity
//    tty.c_cflag &= ~PARODD; // Clear ODD parity bit, therefore even parity to be used
    tty.c_cflag &= ~CSTOPB; // Clear stop field, one stop bits
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte
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
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        return 1;
    }

}
