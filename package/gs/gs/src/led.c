#include "led.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

static int led_indicate_thread(led_t* led);

int led_init(int idx, led_t* led, int32_t* error_flag, int* normal_flag) {

    char filename[FILENAME_MAX_SIZE];

    // enabled
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_LED_ENABLED, idx);

    int ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    led->enabled = ret;

    // normal led port
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_NORMAL_LED_PORT, idx);

    ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    led->normal_led_port = ret;

    // normal led pin
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_NORMAL_LED_PIN, idx);

    ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    led->normal_led_pin = ret;

    led->normal_led_number = (led->normal_led_port - 1) * 32 + led->normal_led_pin;

    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, NORMAL_LED_FILE, led->normal_led_number);
    memset(led->normal_led_filename, 0, sizeof(led->normal_led_filename));
    strcpy(led->normal_led_filename, filename);

    // error led port
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_ERROR_LED_PORT, idx);

    ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    led->error_led_port = ret;

    // error led pin
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_ERROR_LED_PIN, idx);

    ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    led->error_led_pin = ret;

    led->error_led_number = (led->error_led_port - 1) * 32 + led->error_led_pin;

    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, NORMAL_LED_FILE, led->error_led_number);
    memset(led->error_led_filename, 0, sizeof(led->error_led_filename));
    strcpy(led->error_led_filename, filename);

    led->error_flag = error_flag;
    led->normal_flag = normal_flag;

    led->normal_led_state = off;
    led->error_led_state = off;
    if(led->enabled) {
    	pthread_create(&led->thread_id, NULL, led_indicate_thread, led);
    }
    return 0;
}

static int led_indicate_thread(led_t* led) {
//	printf("LED NORMAL gpio filename: %s\r\n", led->normal_led_filename);
//	printf("LED ERROR gpio filename: %s\r\n", led->error_led_filename);
    while(1) {
    	if(*led->error_flag) {
    		//printf("LED. Error: %d\r\n", *led->error_flag);
    		if(led->normal_led_state == on) {
    			led->normal_led_state = off;
    			set_config(led->normal_led_filename, "0", 1);
    		}

    		if(led->error_led_state == off) {
    			//printf("LED. Error. ON.\r\n");
    			led->error_led_state = on;
    			set_config(led->error_led_filename, "1", 1);
    		} else {
    			//printf("LED. Error. OFF.\r\n");
    			led->error_led_state = off;
    			set_config(led->error_led_filename, "0", 1);
    		}
    	} else {
    		if(led->error_led_state == on) {
    			led->error_led_state = off;
    			set_config(led->error_led_filename, "0", 1);
    		}

    		if(*led->normal_flag) {
//        		printf("LED. Fueling: %d\r\n", *led->normal_flag);
				if(led->normal_led_state == off) {
//					printf("LED. Fueling. ON.\r\n");
					led->normal_led_state = on;
					set_config(led->normal_led_filename, "1", 1);
				} else {
//					printf("LED. Fueling. OFF.\r\n");
					led->normal_led_state = off;
					set_config(led->normal_led_filename, "0", 1);
				}
    		} else {
    			if(led->normal_led_state == off) {
    				led->normal_led_state = on;
    				set_config(led->normal_led_filename, "1", 1);
    			}
    		}
    	}
        sleep(1);
    }
}
