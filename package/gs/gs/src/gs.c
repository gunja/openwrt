#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>

#include "config.h"
#include "libazt.h"
#include "rk.h"
#include "buttons.h"

static int delay_to_display_fw_version(rk_t* left_rk, rk_t* right_rk);

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

      	if(delay_to_display_fw_version(&left_rk, &right_rk)) {
        	continue;
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

int delay_to_display_fw_version(rk_t* left_rk, rk_t* right_rk) {
	static int show_time_cnt = 0;
	static int show_time_flag = 0;
    if(show_time_flag != 2) {
    	show_time_cnt++;
    	if(show_time_flag == 0) {
			if(show_time_cnt > 500) {
				show_time_flag = 1;
				show_time_cnt = 0;
				if(left_rk->enabled) {
					left_rk->can_bus.transmit(&left_rk->can_bus, 0.00, 0.00, 0.00);
				}
				if(right_rk->enabled) {
					right_rk->can_bus.transmit(&right_rk->can_bus, 0.00, 0.00, 0.00);
				}
			}
			return 1;
    	} else if(show_time_flag == 1) {
    		if(show_time_cnt > 50) {
				printf("displaying FW version finished\r\n");
    			show_time_flag = 2;
    			show_time_cnt = 0;
    		}
    		return 1;
    	}
    } else {
    	return 0;
    }
}
