#ifdef DEBUGING
#define DEBUGING_DISPLAY
#endif

#define DISPLAY_DIGITS_OFFSET 1

int display_previous_second = 0;


byte getStrikesDisplayByte(byte max_strikes, byte strikes) {
  if (max_strikes == 0) {
    return 0;
  }

  if (max_strikes == 1) {
    if (strikes == 0) {
      return 16;
    } else {
      return 8;
    }
  }

  if (max_strikes == 2) {
    if (strikes == 0) {
      return 80;
    }
    if (strikes == 1) {
      return 72;
    }
    if (strikes == 2) {
      return 40;
    }

  }
  // undefined state
  return 8 + 16 + 32 + 64;
}


void test_display(byte module_number) {

  int delayTime = 600;

  // test strikes
  for (int k = 0; k < 3; k++) {
    for (int j = 0; j < 3; j++) {
      for (int i = 0; i < 3; i++) {
        byte pos = SRoffsetsOutput[module_number];
        shift_register_output[pos] = getStrikesDisplayByte(j, i);
        write_to_output_shift_register();
        delay(delayTime);
      }
    }
  }
  // test 4digits 7segment display
  // TODO:

}

void show_strikes(byte module_number) {
  shift_register_output[SRoffsetsOutput[module_number]] = getStrikesDisplayByte(max_strikes, strikes);
}

void showDigit(byte minutes, byte seconds, byte module_number) {
  // segment_digit is global variable for all modules

  byte data = 0;
  switch (segment_digit) {
    case 0:
      data = ~(DIGIT_DEFINITION[(minutes / 10) & 0x7f]);
      break;

    case 1:
      data = ~(DIGIT_DEFINITION[(minutes % 10) & 0x7f]);
      break;

    case 2:
      data = ~(DIGIT_DEFINITION[(seconds / 10) & 0x7f]);
      break;

    case 3:
      data = ~(DIGIT_DEFINITION[(seconds % 10) & 0x7f]);
      break;

    default:
      ;
  }

  shift_register_output[SRoffsetsOutput[module_number] + DISPLAY_DIGITS_OFFSET + 1] = data;
  shift_register_output[SRoffsetsOutput[module_number] + DISPLAY_DIGITS_OFFSET] = 8 >> segment_digit;

}

void show_time(byte module_number) {
  long temptime = remainingTime;
  int heremillis = temptime % 1000l;

  temptime = (temptime - heremillis) / 1000;

  byte seconds = temptime % 60;
  temptime = (temptime - seconds) / 60;

  byte minutes = temptime % 60;

#ifdef DEBUGING_DISPLAY
  if (display_previous_second != seconds) {
    Serial.print(minutes);
    Serial.print(":");
    Serial.println(seconds);
    display_previous_second = seconds;
  }
#endif

  if (remainingTime >= 60 * 1000l) {
    // cas je nad jednu minutu, ukazujeme minuty a sekundy
    showDigit(minutes, seconds, module_number);
  } else {
    // ukazujeme sekundy a 2 cifry milisekund
    showDigit(seconds, heremillis / 10, module_number);
  }

}


void setup_display(byte module_number) {
  // no setup needed

}

void update_display(byte module_number, boolean output_only) {

  if (output_only) {
    ; // display is as the whole output only, need no special care here
  }

  if (module_status[module_number] == MODULE_TESTING) {
#ifdef DEBUGING_DISPLAY
    Serial.println(F("Testing display"));
#endif

    test_display(module_number);

    // at the end of test module disarm itself
    module_status[module_number] = MODULE_DISARMED;
    return;
  }

  if (module_status[module_number] != MODULE_DISARMED) {
    // this module should be allways disarmed
    module_status[module_number] = MODULE_DISARMED;
  }

  /*
    DISPLAY OUTPUT
  */

  show_strikes(module_number);

  show_time(module_number);

}




