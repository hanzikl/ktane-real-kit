#ifdef DEBUGING
#define DEBUGING_SIMON
#endif

/*
  SETUP FOR SIMON SAYS
*/

byte simon_previous_reading = 0x00;

#define SIMON_STAGE_COUNT 4
#define SIMON_BUTTON_COUNT 4
#define INPUT_MASK_SIMON 0xFF

byte simon_rules[SIMON_BUTTON_COUNT] = {1, 2, 3, 0};

byte simon_stage = 0;

void setup_simon(byte module_number) {
  
}

void update_simon(byte module_number) {
  
}

