#ifdef DEBUGING
#define DEBUGING_DISPLAY
#endif

#define DISPLAY_DIGITS_OFFSET 1
#define DISPLAY_DIGITS_COUNT 4
#define DISPLAY_SHORT_BLINK 250

int display_previous_second = 0;

boolean dot_blinked = false;
boolean show_boomtime = false;

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

void showOneDigit(byte digit, byte digit_position, boolean has_dot, byte module_number) {
  byte data = ~(DIGIT_DEFINITION[digit & 0x7f]);
  if (has_dot) {
    data &= 0x7f; // add dot = remove it's bit
  }

  shift_register_output[SRoffsetsOutput[module_number] + DISPLAY_DIGITS_OFFSET + 1] = data;
  shift_register_output[SRoffsetsOutput[module_number] + DISPLAY_DIGITS_OFFSET] = 8 >> digit_position;
}

boolean displayShouldHaveDot(int heremillis) {
  // TODO: this function and similiar for a beeper could be joined

  // blink speed according to remaining time
  int blinkSpeedInversion = 1000;
  if (remainingTime < BEEPER_SPEED_LEVEL_ONE_SECONDS * 1000) {
    blinkSpeedInversion = BEEPER_SPEED_LEVEL_ONE_INV;
  }

  if (remainingTime < BEEPER_SPEED_LEVEL_TWO_SECONDS * 1000) {
    blinkSpeedInversion = BEEPER_SPEED_LEVEL_TWO_INV;
  }

  return !(clockTicking) || (heremillis % blinkSpeedInversion < blinkSpeedInversion / 2);
}

void showTimePart(byte minutes, byte seconds, int heremillis, byte module_number) {
  // segment_digit is global variable for all modules

  switch (segment_digit) {
    case 0:
      showOneDigit(minutes / 10, segment_digit, false, module_number);
      break;
    case 1:
      showOneDigit(minutes % 10, segment_digit, displayShouldHaveDot(heremillis), module_number);
      break;
    case 2:
      showOneDigit(seconds / 10, segment_digit, false, module_number);
      break;
    case 3:
      showOneDigit(seconds % 10, segment_digit, false, module_number);
      break;
    default:
      ;
  }
}

void show_empty_display(byte module_number) {

  shift_register_output[SRoffsetsOutput[module_number] + DISPLAY_DIGITS_OFFSET + 1] = 255;
  shift_register_output[SRoffsetsOutput[module_number] + DISPLAY_DIGITS_OFFSET] = 0;

}

void show_time(long remainingTime, byte module_number) {
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
    showTimePart(minutes, seconds, heremillis, module_number);
  } else {
    // ukazujeme sekundy a 2 cifry milisekund
    showTimePart(seconds, heremillis / 10, heremillis, module_number);
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
  if (remainingTime > 0) {
    show_time(remainingTime, module_number);
  } else {
    if (millis() % DISPLAY_SHORT_BLINK > DISPLAY_SHORT_BLINK / 2.5) {
      show_empty_display(module_number);
    } else {
      show_time(boomTime, module_number);
    }
  }

}




