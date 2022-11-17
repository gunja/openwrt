#ifndef BUTTONS_H
#define BUTTONS_H

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <linux/input.h>

#include "rk.h"

#define INPUT_DEV "/dev/input/event0"

void buttons_init(rk_t* left, rk_t* right);
int buttons_handler(rk_t* left, rk_t* right);

#endif
