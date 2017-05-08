#ifdef DEBUGING
#define DEBUGING_MAZE
#endif

#include "LedControl.h"

/*
  SETUP FOR A MAZE
*/

#define INPUT_MASK_MAZE 0b11110000 // must be in reversed order
#define MAZE_DISARM_LED 1

#define MAZE_OUTPUT_DATA_PIN 6
#define MAZE_OUTPUT_LOAD_PIN 7
// TODO: can be optimized - use clock from shift registers
#define MAZE_OUTPUT_CLOCK_PIN 8

#define MAZE_MAX_MODULE_COUNT 1

#define MAZE_DATA_ST 0
#define MAZE_DATA_FIN 1
#define MAZE_DATA_PT1 2
#define MAZE_DATA_PT2 3
#define MAZE_DATA_MZ 4

LedControl lc = LedControl(MAZE_OUTPUT_DATA_PIN, MAZE_OUTPUT_CLOCK_PIN, MAZE_OUTPUT_LOAD_PIN, MAZE_MAX_MODULE_COUNT);

/*
   Getting and setting data
*/

byte getMazeStartPosition(byte module_number) {
  return module_data[module_number][MAZE_DATA_ST];
}

void setMazeStartPosition(byte module_number, byte value) {
  module_data[module_number][MAZE_DATA_ST] = value;
}

byte getMazeFinishPosition(byte module_number) {
  return module_data[module_number][MAZE_DATA_FIN];
}

void setMazeFinishPosition(byte module_number, byte value) {
  module_data[module_number][MAZE_DATA_FIN] = value;
}

byte getMazePT1Position(byte module_number) {
  return module_data[module_number][MAZE_DATA_PT1];
}

void setMazePT1Position(byte module_number, byte value) {
  module_data[module_number][MAZE_DATA_PT1] = value;
}

byte getMazePT2Position(byte module_number) {
  return module_data[module_number][MAZE_DATA_PT2];
}

void setMazePT2Position(byte module_number, byte value) {
  module_data[module_number][MAZE_DATA_PT2] = value;
}



/*
  RANDOM MAZE GENERATOR
*/

void maze_randomize_position(byte module_number) {



}

void maze_generate(byte module_number) {



}


void test_maze_output(byte module_number) {

#ifdef DEBUGING_MAZE
  Serial.println(F("T:M"));
#endif

  int delayTime = 500;

  byte pos = SRoffsetsOutput[module_number];

  // test LED matrix

  lc.setRow(0, 0, 0xFF);
  for (int row = 1; row < 8; row++) {
    delay(delayTime);
    lc.setRow(0, row - 1, 0);
    lc.setRow(0, row, 0xFF);
  }

  delay(delayTime);
  lc.clearDisplay(0);
  delay(delayTime);

  lc.setColumn(0, 0, 0xFF);
  for (int column = 1; column < 8; column++) {
    delay(delayTime);
    lc.setColumn(0, column - 1, 0);
    lc.setColumn(0, column, 0xFF);
  }

  delay(delayTime);
  lc.clearDisplay(0);

  shift_register_output[pos] = 1 << MAZE_DISARM_LED;
  write_to_output_shift_register();
  delay(delayTime);

}

void setup_maze(byte module_number) {
  // for simplicity only one module MAZE can be connected

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);

}

void update_maze(byte module_number) {

  byte pos = SRoffsetsOutput[module_number];

  if (module_status[module_number] == MODULE_DISARMED) {
    shift_register_output[pos] = 1 << MAZE_DISARM_LED;
    return;
  }

  if (module_status[module_number] == MODULE_TESTING) {
    test_maze_output(module_number);
    test_simon_input(module_number);

    // at the end of test module disarm itself
    module_status[module_number] = 1 << MAZE_DISARM_LED;
    return;
  }

}



