#define AZTLIB
#include "libazt.h"


void azt_request_parser(char* rx_buf, int rx_buf_len);
static int azt_calculate_address(char* rx_buf, int starting_symbol_idx);
static int azt_validate_complimentary_bytes(char* rx_buf, int starting_symbol_idx, int ending_symbol_idx);
static azt_validate_crc(char* rx_buf, int starting_symbol_idx, int ending_symbol_idx, int crc_symbol_idx);
static int azt_calculate_crc(char* buf, int starting_symbol_idx, int ending_symbol_idx);
static void azt_req_extract(azt_request_t* azt_req, char* rx_buf, int azt_request_cmd_idx, int ending_symbol_idx, int address);
static azt_req_clean(azt_request_t* req);

char azt_request_params[AZT_REQUEST_PARAMS_MAX_AMOUNT] = {0};
int azt_request_params_cnt = 0;

azt_request_t azt_req;

void azt_request_parser(char* rx_buf, int rx_buf_len) {
//    printf("\n");
//    for(int i = 0; i<rx_buf_len; i++) {
//        printf("%02X ", rx_buf[i]);
//    }
//    printf("\n");

    // Check for starting byte idx
    int starting_symbol_idx = -1;
    for(int i = 0; i<rx_buf_len; i++) {
        if(rx_buf[i] == AZT_RQST_DEL_SYMBOL) {
            starting_symbol_idx = i+1;
            break;
        }
    }
    if(starting_symbol_idx == -1) { // No DEL symbol detected, skip the whole packet
        return;
    }


    // Check for ending byte idx
    int ending_symbol_idx = -1;
    for(int i = starting_symbol_idx+1; i<rx_buf_len; i++) {
        if((rx_buf[i] == AZT_RQST_ENDING_SYMBOL) && (rx_buf[i+1] == AZT_RQST_ENDING_SYMBOL)) {
            ending_symbol_idx = i;
            break;
        }
    }
    if(ending_symbol_idx == -1) {
        return;  // Ending symbols not found
    }

    int crc_symbol_idx = ending_symbol_idx+2;
    if(crc_symbol_idx >= rx_buf_len) {
        return;
    }

    int ret = azt_validate_complimentary_bytes(rx_buf, starting_symbol_idx, ending_symbol_idx);
    if(ret == -1) {
        return;  // Broken packet. Wrong complimentary bytes
    }
    ret = azt_validate_crc(rx_buf, starting_symbol_idx, ending_symbol_idx, crc_symbol_idx);
    if(ret == -1) {
        return;  // Broken packet. Wrong complimentary bytes
    }

    int address = azt_calculate_address(rx_buf, starting_symbol_idx);
    if(address == -1) {
        return;
    }

    int azt_request_cmd_idx = starting_symbol_idx+3;

    memset(azt_request_params, 0, AZT_REQUEST_PARAMS_MAX_AMOUNT);
    azt_request_params_cnt = 0;

    azt_req_clean(&azt_req);
    azt_req_extract(&azt_req, rx_buf, azt_request_cmd_idx, ending_symbol_idx, address);

}







int azt_init(void) {
    int ret = azt_port_init(azt_request_parser);
    if (ret == -1) {
        printf("Azt port initialization failed:  %d \n", ret);
        return -1;
    }
}

void azt_deinit(void) {
    azt_port_close();
}

int azt_rx_handler(void) {
    int ret = azt_port_read();
    if(ret == 1) {
        return 1;
    }
    return 0;
}

azt_request_t* azt_request(void) {
    return &azt_req;
}


char tx_buf[256];
int tx_buf_len = 0;

int azt_tx_can(void) {
    int cur_pos = 0;
    memset(tx_buf, 0, sizeof(tx_buf));
    tx_buf_len = 0;

    tx_buf[cur_pos] = AZT_RQST_DEL_SYMBOL;
    cur_pos++;
    tx_buf[cur_pos] = AZT_CAN_SYMBOL;
    cur_pos++;
    tx_buf_len = cur_pos;
    azt_port_write(tx_buf, tx_buf_len);
}

int azt_tx_ack(void) {
    int cur_pos = 0;
    memset(tx_buf, 0, sizeof(tx_buf));
    tx_buf_len = 0;

    tx_buf[cur_pos] = AZT_RQST_DEL_SYMBOL;
    cur_pos++;
    tx_buf[cur_pos] = AZT_ACK_SYMBOL;
    cur_pos++;
    tx_buf_len = cur_pos;
    azt_port_write(tx_buf, tx_buf_len);
}

int azt_tx_nak(void) {
    int cur_pos = 0;
    memset(tx_buf, 0, sizeof(tx_buf));
    tx_buf_len = 0;

    tx_buf[cur_pos] = AZT_RQST_DEL_SYMBOL;
    cur_pos++;
    tx_buf[cur_pos] = AZT_NAK_SYMBOL;
    cur_pos++;
    tx_buf_len = cur_pos;
    azt_port_write(tx_buf, tx_buf_len);
}

int azt_tx(char* data, int cnt) {
    int cur_pos = 0;
    memset(tx_buf, 0, sizeof(tx_buf));
    tx_buf_len = 0;

    tx_buf[cur_pos] = AZT_RQST_DEL_SYMBOL;
    cur_pos++;
    int starting_symbol_idx = cur_pos;

    tx_buf[cur_pos] = AZT_RQST_STX_SYMBOL;
    cur_pos++;

    if(cnt > AZT_RESPONCE_MAX_LENGTH) {
        return -1;
    }

    for(int i=0; i<cnt; i++) {
        tx_buf[cur_pos] = data[i];
        cur_pos++;
        tx_buf[cur_pos] = COMPLIMENTARY(data[i]);
        cur_pos++;
    }

    tx_buf[cur_pos] = AZT_RQST_ENDING_SYMBOL;
    int ending_symbol_idx = cur_pos;
    cur_pos++;

    tx_buf[cur_pos] = AZT_RQST_ENDING_SYMBOL;
    cur_pos++;

    tx_buf[cur_pos] = azt_calculate_crc(tx_buf, starting_symbol_idx, ending_symbol_idx);
    cur_pos++;
    tx_buf_len = cur_pos;

//    for(int i=0; i<tx_buf_len; i++) {
//        printf("%02X ", tx_buf[i]);
//    }
//    printf("\n");

    azt_port_write(tx_buf, tx_buf_len);
//    azt_port_write("hello\r\n", 7);

}
//////////////////////////////////////////////////////////////////////////////////////
/////////// local functions  /////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
static azt_req_clean(azt_request_t* req) {
    req->cmd = 0;
    req->params_cnt = 0;
    memset(req->params, 0, AZT_REQUEST_PARAMS_MAX_AMOUNT);
}

static void azt_req_extract(azt_request_t* azt_req, char* rx_buf, int azt_request_cmd_idx, int ending_symbol_idx, int address)
{
    int j=0;
    for(int i=azt_request_cmd_idx+2; i<ending_symbol_idx; i+=2) {
        azt_req->params[j] = rx_buf[i];
        j++;
    }
    azt_req->params_cnt = j;
    azt_req->cmd = rx_buf[azt_request_cmd_idx];
    azt_req->address = address;
}

static int azt_calculate_address(char* rx_buf, int address_shift_byte_idx)
{
    char address_shift_code = rx_buf[address_shift_byte_idx];
    char related_address_code = rx_buf[address_shift_byte_idx+1];

    int address_shift = -1;
    for(int i=0; i<AZT_STARTING_SYMBOLS_AMOUNT; i++) {
        if(AZT_STARTING_SYMBOLS[i] == address_shift_code) {
            address_shift = AZT_ADDRESS_SHIFTS[i];
            break;
        }
    }
    if(address_shift == -1) {
        return -1;  // Wrong shift address byte
    }

    int related_address = -1;
    for(int i=0; i<AZT_RELATED_ADDRESSES_AMOUNT; i++) {
        if(AZT_RELATED_ADDRESS[i] == related_address_code) {
            related_address = AZT_RELATED_ADDRESS[i] - AZT_RELATED_ADDRESS_BASE;
            break;
        }
    }

    if(related_address == -1) {
        return -1;  // Wrong related address byte
    }

    int address = address_shift + related_address;
    return address;
}

static int azt_validate_complimentary_bytes(char* rx_buf, int starting_symbol_idx, int ending_symbol_idx)
{
    for(int i=starting_symbol_idx+1; i<ending_symbol_idx; i+=2) {
        if(rx_buf[i+1] != COMPLIMENTARY(rx_buf[i])) {
            printf("AZT. Complimentary error. %02X != %02X\n", rx_buf[i+1], COMPLIMENTARY(rx_buf[i]));
            return -1;
        }
    }
    return 0;
}

static azt_validate_crc(char* rx_buf, int starting_symbol_idx, int ending_symbol_idx, int crc_symbol_idx) {
    int res = 0;
    for(int i=starting_symbol_idx+1; i<=ending_symbol_idx; i+=2) {
        res ^= rx_buf[i];
    }
    res = res | 0x40;

    if(rx_buf[crc_symbol_idx] != res) {
        printf("AZT. Crc error. %02X != %02X\n", res, rx_buf[crc_symbol_idx]);
        return -1;
    }
    return 0;
}

static int azt_calculate_crc(char* buf, int starting_symbol_idx, int ending_symbol_idx) {
    int res = 0;
    for(int i=starting_symbol_idx+1; i<=ending_symbol_idx; i+=2) {
        res ^= buf[i];
    }
    res = res | 0x40;

    return res;
}
