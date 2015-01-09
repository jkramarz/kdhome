#include "providers_defines.h"

// #define OUTPUT_PROVIDER_ENABLED
#define INPUT_PROVIDER_ENABLED
// #define IR_PROVIDER_ENABLED
// #define TEMP_PROVIDER_ENABLED

#define INPUTS_COUNT 8
#define OUTPUTS_COUNT 8

#define TEMP_SENSORS_COUNT 8

#define TEMP_SENSORS_MODES { INTFRAC, INTFRAC, INTFRAC, INTFRAC, \
	INTFRAC, INTFRAC, INTFRAC, INTFRAC }

#include <stdio.h>
#define PROVIDER_DEBUG(x,...) printf(x, ##__VA_ARGS__)
