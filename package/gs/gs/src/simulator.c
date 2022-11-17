#include "config.h"

#ifdef SIMULATION

int simulation_valve_opened_flag = 0;

float simulation_mass_inventory_value = 0.00;
float simulation_mass_rate_value = 0.00;

float simulate_mass_inventory_value() {

	if(simulation_valve_opened_flag) {
		simulation_mass_inventory_value += 0.01;
		simulation_mass_rate_value -= 1.0;

	}
	return simulation_mass_inventory_value;
}

void simulation_open_valve() {
	simulation_valve_opened_flag = 1;
	simulation_mass_rate_value = 121.0;
}

void simulation_close_valve() {
	simulation_valve_opened_flag = 0;
	simulation_mass_rate_value = 0.0;
}

void simulation_open_high_valve() {
//	simulation_valve_opened_flag = 1;
	simulation_mass_rate_value += 121.0;
}

void simulation_close_high_valve() {
//	simulation_valve_opened_flag = 0;
//	simulation_mass_rate_value = 0.0;
}

#endif
