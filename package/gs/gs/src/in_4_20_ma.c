#include "in_4_20_ma.h"

#define FILENAME_MAX_SIZE   64
#define RX_BUF_SIZE         64

int in_4_20_ma_read_thread(in_4_20_t* in_4_20);

int in_4_20_ma_init(int idx, in_4_20_t* in_4_20) {

    char filename[FILENAME_MAX_SIZE];

    // channel_idx
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_CHANNEL_IDX, idx);

    memset(in_4_20->channel_idx_filename, 0, sizeof(in_4_20->channel_idx_filename));
    strcpy(in_4_20->channel_idx_filename, filename);
    int ret = parse_integer_config(filename);
    if(ret == -1) {
        return -1;
    }
    in_4_20->channel_idx = ret;

    // setting pressure low level error (mA)
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_PRESSURE_LOW_THRESHOLD, idx);
    float tmp;
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    in_4_20->pressure_low_threshold = tmp;
    in_4_20->pressure_low_threshold_raw = in_4_20_ma_convert_ma_to_raw(in_4_20->pressure_low_threshold);

    // setting pressure high level error (mA)
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, CONFIG_FILE_PRESSURE_HIGH_THRESHOLD, idx);
    tmp;
    ret = parse_float_config(filename, &tmp);
    if(ret == -1) {
        return -1;
    }
    in_4_20->pressure_high_threshold = tmp;
    in_4_20->pressure_high_threshold_raw = in_4_20_ma_convert_ma_to_raw(in_4_20->pressure_high_threshold);

    // value
    memset(filename, 0, FILENAME_MAX_SIZE);
    sprintf(filename, VALUE_FILENAME, in_4_20->channel_idx);

    memset(in_4_20->value_filename, 0, sizeof(in_4_20->value_filename));
    strcpy(in_4_20->value_filename, filename);

    ret = parse_integer_config(in_4_20->value_filename);
    in_4_20->value = ret;

    atomic_init(&in_4_20->value, 0);
    _Atomic int* value = (_Atomic int*)&in_4_20->value;
    atomic_store(value, ret);
    pthread_create(&in_4_20->thread_id, NULL, in_4_20_ma_read_thread, in_4_20);

    if(ret < INPUT_NOT_CONNECTED_THRESHOLD_VALUE) {
        return -1;
    }
    return 0;
}

int in_4_20_ma_check_state(in_4_20_t* in_4_20)
{
	int val = atomic_load(&in_4_20->value);

    if(val < INPUT_NOT_CONNECTED_THRESHOLD_VALUE)
    {
    	in_4_20->state = IN_4_20_NOT_CONNECTED_ERROR;
    }
    else if (val < in_4_20->pressure_low_threshold_raw)
    {
    	in_4_20->state = IN_4_20_LOW_PRESSURE_ERROR;
    }
    else if (val > in_4_20->pressure_high_threshold_raw)
    {
    	in_4_20->state = IN_4_20_HIGH_PRESSURE_ERROR;
    }
    else if( (val > in_4_20->pressure_low_threshold_raw + RAW_PRESSURE_THRESHOLD_GAP) &&
    		 (val < in_4_20->pressure_high_threshold_raw - RAW_PRESSURE_THRESHOLD_GAP) )
    {
    	in_4_20->state = IN_4_20_NO_ERROR;
    }
    return in_4_20->state;
}

int in_4_20_ma_read(in_4_20_t* in_4_20)
{
	int val = atomic_load(&in_4_20->value);
    return val;
}

int in_4_20_ma_read_thread(in_4_20_t* in_4_20) {
	_Atomic int* value = (_Atomic int*)&in_4_20->value;
    while(1) {
        int ret = parse_integer_config(in_4_20->value_filename);
        atomic_store(value, ret);
        usleep(300000);
    }
}

float in_4_20_ma_convert_raw_to_ma(int value) {
	float res = (value - 2100) / 537.5 + 4.0;
	return res;
}

float in_4_20_ma_convert_ma_to_raw(float value) {
	int res = (value - 4.0) * 537.5 + 2100;
	return res;
}

