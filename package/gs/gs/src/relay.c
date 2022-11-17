#include "relay.h"

#define FILENAME_MAX_SIZE   64


int relay_init(int idx, relay_t* relay) {

    char filename[FILENAME_MAX_SIZE];

    // relay low
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_RELAY_LOW_NUMBER, idx);
    int ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    relay->relay_low_number = ret;

    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, RELAY_LOW_FILE, relay->relay_low_number);
    memset(relay->relay_low_filename, 0, sizeof(relay->relay_low_filename));
    strcpy(relay->relay_low_filename, filename);


    // relay middle
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_RELAY_MIDDLE_NUMBER, idx);
    ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    relay->relay_middle_number = ret;

    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, RELAY_MIDDLE_FILE, relay->relay_middle_number);
    memset(relay->relay_middle_filename, 0, sizeof(relay->relay_middle_filename));
    strcpy(relay->relay_middle_filename, filename);


    // relay high
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_RELAY_HIGH_NUMBER, idx);
    ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    relay->relay_high_number = ret;

    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, RELAY_HIGH_FILE, relay->relay_high_number);
    memset(relay->relay_high_filename, 0, sizeof(relay->relay_high_filename));
    strcpy(relay->relay_high_filename, filename);

    relay->relay_low_state = relay_off;
    relay->relay_middle_state = relay_off;
    relay->relay_high_state = relay_off;

    return 0;
}

int relay_low_on(relay_t* relay)
{
	int ret = set_config(relay->relay_low_filename, "1", 1);
	if(!ret) {
		relay->relay_low_state = relay_on;
	}
	return ret;
}

int relay_low_is_on(relay_t* relay)
{
	if(relay->relay_low_state == relay_on) {
		return 1;
	} else {
		return 0;
	}
}

int relay_low_off(relay_t* relay)
{
	int ret = set_config(relay->relay_low_filename, "0", 1);
	if(!ret) {
		relay->relay_low_state = relay_off;
	}
	return ret;
}

int relay_middle_on(relay_t* relay)
{
	int ret = set_config(relay->relay_middle_filename, "1", 1);
	if(!ret) {
		relay->relay_middle_state = relay_on;
	}
#ifdef SIMULATION
	simulation_open_valve();
#endif
	return ret;
}

int relay_middle_is_on(relay_t* relay)
{
	if(relay->relay_middle_state == relay_on) {
		return 1;
	} else {
		return 0;
	}
}

int relay_middle_off(relay_t* relay)
{
	int ret = set_config(relay->relay_middle_filename, "0", 1);
	if(!ret) {
		relay->relay_middle_state = relay_off;
	}
#ifdef SIMULATION
	simulation_close_valve();
#endif
	return ret;
}

int relay_high_on(relay_t* relay)
{
	int ret = set_config(relay->relay_high_filename, "1", 1);
	if(!ret) {
		relay->relay_high_state = relay_on;
	}
#ifdef SIMULATION
	simulation_open_high_valve();
#endif
	return ret;
}

int relay_high_is_on(relay_t* relay)
{
	if(relay->relay_high_state == relay_on) {
		return 1;
	} else {
		return 0;
	}
}

int relay_high_off(relay_t* relay)
{
	int ret = set_config(relay->relay_high_filename, "0", 1);
	if(!ret) {
		relay->relay_high_state = relay_off;
	}
#ifdef SIMULATION
	simulation_close_high_valve();
#endif
	return ret;
}
