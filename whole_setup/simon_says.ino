#ifdef DEBUGING
#define DEBUGING_SIMON
#endif

/*
  SETUP FOR SIMON SAYS
*/

#define SIMON_SR_INPUT_WIDTH 8
#define SIMON_SR_OUTPUT_WIDTH 8

byte simon_previous_reading = 0x00;

#define SIMON_STAGE_COUNT 4

#define SIMON_BUTTON_COUNT 4

byte simon_rules[SIMON_BUTTON_COUNT] = {1, 2, 3, 0};

byte simon_stage = 0;
