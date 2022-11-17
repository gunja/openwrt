all: gs test_modbus test_modbus2

CC = gcc
CFLAGS += -Wall -Wunused -std=c11
LDFLAGS =

DEPS = $(wildcard *.h)

SRC = $(wildcard *.c)

OBJ = $(patsubst %.c, %.o, $(SRC))

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gs: $(OBJ)
	$(CC) -o gs gs.o libazt.o buttons.o rk.o can.o settings.o in_4_20_ma.o libazt_ll.o libgs.o error.o led.o relay.o simulator.o delay_counter.o $(LDFLAGS)

test_modbus: $(OBJ)
	$(CC) -o test_modbus test_modbus.o libgs.o settings.o simulator.o $(LDFLAGS)

test_modbus2: $(OBJ)
	$(CC) -o test_modbus2 test_modbus2.o $(LDFLAGS)

.PHONY: clean

clean:
	rm -f gs test_modbus *.o
