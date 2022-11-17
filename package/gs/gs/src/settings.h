//
// Created by al on 23.07.22.
//

#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#define DIGIT_CHAR_TO_NUM(a)(a -= 0x30)

#define TRUE    1
#define FALSE   0

int parse_integer_config(char* filename);
int parse_true_false_config(char* filename);
int parse_float_config(char* filename, float* res);
int set_config(char* filename, char* data, int len);

#endif // SETTINGS_H
