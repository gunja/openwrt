#include "error.h"

static void error_calculate_message(error_t* storage);

void error_init(error_t* storage)
{
    storage->code = 0;
    storage->message = 0;
}

void error_set(error_t* storage, errorCodes code)
{
    ERROR_SET(storage->code, code);
    error_calculate_message(storage);
}

void error_clear(error_t* storage, errorCodes code)
{
    ERROR_CLEAR(storage->code, code);
    error_calculate_message(storage);
}

int error_is_set(error_t* storage, errorCodes code)
{
    if(ERROR_IS_SET(storage->code, code)) {
        return 1;
    } else {
        return 0;
    }
}

int error_is_clear(error_t* storage, errorCodes code)
{
    if(ERROR_IS_CLEAR(storage->code, code)) {
        return 1;
    } else {
        return 0;
    }
}

static void error_calculate_message(error_t* storage) {
    if(storage->code) {
        storage->message = (int) ERROR_MESSAGE_DEFAULT_DISPLAY_STATE + storage->code;
    } else {
        storage->message = 0.00;
    }
}
