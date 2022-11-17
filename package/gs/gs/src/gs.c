#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

#include "config.h"
//#include "libgs.h"
#include "libazt.h"
#include "rk.h"
#include "buttons.h"

/*
* Example button handler
* Define won handlers for all keycodes from button.c
*/
void button_handler(int code){
	printf("Handled button: 0x%04x (%d)\n", (int)code, (int)code);
}

int main(int argc, char* argv[])
{
	int ret = azt_init();
	if (ret == -1) {
		return 1;
	}

    rk_t left_rk, right_rk;
    rk_init(1, &left_rk);
    rk_init(2, &right_rk);
	buttons_init(&left_rk,
                 &right_rk);

    int azt_req_flag = 0;
	while(1) {
		buttons_handler(&left_rk, &right_rk);
        azt_req_flag = azt_rx_handler();
        if(azt_req_flag) {
            left_rk.azt_req_hndl(azt_request(), &left_rk);
            right_rk.azt_req_hndl(azt_request(), &right_rk);
        }

        if(left_rk.enabled) {
			if(left_rk.is_not_fault(&left_rk)) {
				left_rk.process(&left_rk);
			} else {
				left_rk.stop(&left_rk);
			}
        }

        if(right_rk.enabled) {
			if(right_rk.is_not_fault(&right_rk)) {
				right_rk.process(&right_rk);
			} else {
				right_rk.stop(&right_rk);
			}
        }
    }
    azt_deinit();

	return 0;

}
