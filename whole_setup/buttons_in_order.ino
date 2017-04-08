#ifdef DEBUGING
#define DEBUGING_BIO
#endif

/*
  SETUP FOR BUTTONS IN ORDER
*/

#define BIO_PIN_1 2
#define BIO_PIN_2 3

byte bio_previous_reading = 0x00;

#define BIO_CORRECT_BUTTON 1

#define BIO_STAGE_COUNT 4

byte correct_buttons[BIO_STAGE_COUNT] = {1, 2, 1, 2};
byte stage = 0;

void setup_buttons_in_order() {

  /*
    SETUP FUNCTION FOR BUTTONS IN ORDER
  */

  pinMode(BIO_PIN_1, INPUT);
  pinMode(BIO_PIN_2, INPUT);

  stage = 0;

}

byte bio_get_readings() {

  byte reading;

  // read buttons
  reading = digitalRead(BIO_PIN_1);
  reading = reading << 1;

  reading += digitalRead(BIO_PIN_2);

  if (reading == bio_previous_reading) {
    // reading is confirmed and accepted (no noise)
    bio_previous_reading = reading;
    return reading;
  } else {
    bio_previous_reading = reading;
    return READING_ERROR;
  }

}


void check_buttons_in_order() {

  if (bio_status == MODULE_DISARMED) {
    return;
  }

  byte reading = bio_get_readings();
  // Serial.println(reading);

  if (bio_status == MODULE_FAILED_TO_DISARM || bio_status == MODULE_DISARMING_IN_PROGRESS) {
    // do not react until all buttons are off
    if (reading == 0) {
      bio_status = MODULE_ARMED;
#ifdef DEBUGING_BIO
      Serial.println("RE-ARMING MODULE Buttons In Order");
#endif
    }
    return;
  }

  switch (reading) {
    case READING_ERROR:
      break;
    case 0:
      break;
    default:
      if (reading == correct_buttons[stage]) {
#ifdef DEBUGING_BIO
        Serial.print("Stage ");
        Serial.print(stage);
        Serial.println(" OK");
#endif
        bio_status = MODULE_DISARMING_IN_PROGRESS;
        stage++;
      } else {
        bio_status = MODULE_FAILED_TO_DISARM;
        addStrike();
        stage = 0;
      }
  }

  if (stage >= BIO_STAGE_COUNT) {
    bio_status = MODULE_DISARMED;
  }

}


