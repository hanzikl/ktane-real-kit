#ifdef DEBUGING
#define DEBUGING_MAZE
#endif

#include "LedControl.h"

/*
  SETUP FOR A MAZE
*/

#define INPUT_MASK_MAZE 0b11110000 // must be in reversed order
#define MAZE_DISARM_LED 7

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

#define MAZE_TOP 1
#define MAZE_RIGHT 2
#define MAZE_BOTTOM 4
#define MAZE_LEFT 8

#define MAZE_BLINK_DELAY 300

const byte maze_input_connection[] = {16, 32, 64, 128};

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

byte getMazeWallAt(byte module_number, byte pos) {
  return module_data[module_number][MAZE_DATA_MZ + pos];
}

void setMazeWallAt(byte module_number, byte pos, byte value) {
  module_data[module_number][MAZE_DATA_MZ + pos] = value;
}


/*
  RANDOM MAZE GENERATOR - for early experiments without maze from serial communication
*/

void maze_randomize_position(byte module_number) {

  byte start_pos = random(6 * 6);
  byte finish_pos = random(6 * 6);

  while (finish_pos == start_pos) {
    start_pos = random(6 * 6);
  }

  setMazeFinishPosition(module_number, start_pos);
  setMazeStartPosition(module_number, finish_pos);

}

void maze_generate(byte module_number) {

  for (byte p = 0; p < 6 * 6; p++) {
    byte wall = 0;
    if (random(5) < 3) {
      wall = random(2 * MAZE_LEFT);
    }
    setMazeWallAt(module_number, p, wall);
    // setMazeWallAt(module_number, p, 0);
  }

  maze_randomize_position(module_number);

}

// debugging function
void print_maze(byte module_number) {

  byte wall = 0;
  for (byte y = 0; y < 6; y++) {
    Serial.print(debug_print_char);
    // TOP
    for (byte x = 0; x < 6; x++) {
      byte p = y * 6 + x;
      wall = getMazeWallAt(module_number, p);
      if (wall & MAZE_TOP) {
        Serial.print(F("-=-"));
      } else {
        Serial.print(F("- -"));
      }
    }
    Serial.println();
    Serial.print(debug_print_char);
    // SIDES AND CENTER
    for (byte x = 0; x < 6; x++) {
      byte p = y * 6 + x;
      wall = getMazeWallAt(module_number, p);
      if (wall & MAZE_LEFT) {
        Serial.print(F("|"));
      } else {
        Serial.print(F(" "));
      }

      // center
      if (p == getMazeStartPosition(module_number)) {
        Serial.print(F("S"));
      } else if (p == getMazeFinishPosition(module_number)) {
        Serial.print(F("F"));
      } else {
        Serial.print(F(" "));
      }

      if (wall & MAZE_RIGHT) {
        Serial.print(F("|"));
      } else {
        Serial.print(F(" "));
      }
    }
    Serial.println("");
    Serial.print(debug_print_char);
    // BOTTOM
    for (byte x = 0; x < 6; x++) {
      byte p = y * 6 + x;
      wall = getMazeWallAt(module_number, p);
      if (wall & MAZE_BOTTOM) {
        Serial.print(F("-=-"));
      } else {
        Serial.print(F("- -"));
      }
    }
    Serial.println("");
  }

}


void test_maze_output(byte module_number) {

#ifdef DEBUGING_MAZE
  Serial.print(debug_print_char);
  Serial.println(F("T:M"));
#endif

  int delayTime = 500;

  byte pos = SRoffsetsOutput[module_number];
  shift_register_output[pos] = 1 << MAZE_DISARM_LED;

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

void test_maze_input(byte module_number) {
  int delayTime = 50; // ms

  boolean have[5] = {false, false, false, false};

  byte pos = SRoffsetsOutput[module_number];
  shift_register_output[pos] = 0;

  while (true) {

    update_shift_registers();
    delay(delayTime);

    byte input_value = get_module_input(module_number, INPUT_MASK_MAZE, true);

    // figure out, which button is on
    for (int i = 0; i < 4; i++) {
      if (input_value == maze_input_connection[i]) {
        have[i] = true;
        switch (i) {
          case 0: // TOP
            lc.setRow(0, 7, 0b00011000);
            break;
          case 1: // RIGHT
            lc.setColumn(0, 0, 0b00011000);
            break;
          case 2: // BOTTOM
            lc.setRow(0, 0, 0b00011000);
            break;
          case 3: // LEFT
            lc.setColumn(0, 7, 0b00011000);
            break;
        }
      }
    }

    if (have[0] && have[1] && have[2] && have[3]) {
      lc.clearDisplay(0);
      break;
    }

  }

}

byte move_via_button(byte module_number, byte button) {
  byte p = getMazeStartPosition(module_number);
  byte newpos = p;
  byte wall = getMazeWallAt(module_number, p);

  if (button & wall) {
    // runned into a wall
#ifdef DEBUGING_MAZE
    Serial.print(debug_print_char);
    Serial.println(F("WALL!"));
#endif
    return newpos;
  }

  switch (button) {
    case MAZE_TOP:
      newpos -= 6;
      break;
    case MAZE_BOTTOM:
      newpos += 6;
      break;
    case MAZE_LEFT:
      newpos -= 1;
      break;
    case MAZE_RIGHT:
      newpos += 1;
      break;

  }

  return newpos;

}


void setup_maze(byte module_number) {
  // for simplicity only one module MAZE can be connected
  // TODO: allow more maze modules - should not be hard

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);


  maze_generate(module_number);
  print_maze(module_number);

}

void update_maze(byte module_number) {

  byte pos = SRoffsetsOutput[module_number];

  if (module_status[module_number] == MODULE_DISARMED) {
    shift_register_output[pos] = 1 << MAZE_DISARM_LED;
    return;
  }
  shift_register_output[pos] = 0;

  if (module_status[module_number] == MODULE_TESTING) {
    test_maze_output(module_number);
    test_maze_input(module_number);

    // at the end of test module disarm itself
    module_status[module_number] = MODULE_DISARMED;
    return;
  }

  byte reading = get_module_sanitized_input(module_number, INPUT_MASK_MAZE, true);

  if (module_status[module_number] == MODULE_FAILED_TO_DISARM
      || module_status[module_number] == MODULE_DISARMING_IN_PROGRESS)  {
    if (reading == 0) {
      module_status[module_number] = MODULE_ARMED;
      shift_register_output[pos] = 0;
#ifdef DEBUGING_MAZE
      Serial.print(debug_print_char);
      Serial.println(F("RE-ARMING MAZE"));
#endif
    }
    return;
  }

  boolean blinked = (millis() % MAZE_BLINK_DELAY) > (MAZE_BLINK_DELAY / 2);

  if (!clockTicking) {
    // do not update input and output
    return;
  }

  // simple output

  byte p = getMazeFinishPosition(module_number);
  lc.setLed(0, 1 + (36 - 1 - p) / 6, 1 + (36 - 1 - p) % 6, true);

  p = getMazeStartPosition(module_number);
  // start = actual position is blinking
  if (blinked) {
    lc.setLed(0, 1 + (36 - 1 - p) / 6, 1 + (36 - 1 - p) % 6, true);
  } else {
    lc.setLed(0, 1 + (36 - 1 - p) / 6, 1 + (36 - 1 - p) % 6, false);
  }


  /*
     MAZE INPUT
  */

  switch (reading) {
    case READING_ERROR:
      break;
    case 0:
      break;
    default:
      {
        byte button = 0;
        // figure out, which button is on
        for (byte i = 0; i < 4; i++) {
          if (reading == maze_input_connection[i]) {
            button = 1 << i;
            break;
          }
        }

#ifdef DEBUGING_MAZE
        Serial.print(debug_print_char);
        Serial.print("M");
        Serial.print(module_number);
        Serial.print(" P");
        Serial.print(getMazeStartPosition(module_number));
        Serial.print(" F");
        Serial.print(getMazeFinishPosition(module_number));
        Serial.print(" B");
        Serial.print(button);
        Serial.print(" W");
        Serial.println(getMazeWallAt(module_number, getMazeStartPosition(module_number)));
#endif

        byte newpos = move_via_button(module_number, button);

        if (newpos != p) {
          setMazeStartPosition(module_number, newpos);
          module_status[module_number] = MODULE_DISARMING_IN_PROGRESS;
          // turn off LED on previous position
          lc.setLed(0, 1 + (36 - 1 - p) / 6, 1 + (36 - 1 - p) % 6, false);
          p = newpos;
          // turn on LED on new position
          lc.setLed(0, 1 + (36 - 1 - p) / 6, 1 + (36 - 1 - p) % 6, true);
        } else {
          module_status[module_number] = MODULE_FAILED_TO_DISARM;
          addStrike();
        }

        if (newpos == getMazeFinishPosition(module_number)) {
          module_status[module_number] = MODULE_DISARMED;
        }

      }
  }
}

