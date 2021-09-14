#include <stdint.h>

#define output_num 8
extern uint8_t outputs[output_num];
#define MAX_POWER 63
extern uint16_t outputs_timers[output_num];
extern uint8_t outputs_timers_powers[output_num];

void data_init();
