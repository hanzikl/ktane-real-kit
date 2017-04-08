
#define BIO_CORRECT_BUTTON 1

void setup_buttons_in_order() {

  /*
    SETUP FOR BUTTONS IN ORDER
  */

  pinMode(BIO_PIN_1, INPUT);
  pinMode(BIO_PIN_2, INPUT);

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

  if (bio_status == MODULE_FAILED_TO_DISARM) {
    // not react until all buttons are off
    if (reading == 0) {
      bio_status = MODULE_ARMED;
      Serial.println("REARMING ButtonsInOrder MODULE");
    }
    return;
  }

  switch (reading) {
    case READING_ERROR:
      break;
    case 0:
      break;
    default:
      if (reading == BIO_CORRECT_BUTTON) {
        bio_status = MODULE_DISARMED;
      } else {
        bio_status = MODULE_FAILED_TO_DISARM;
      }
  }

  if (bio_status == MODULE_FAILED_TO_DISARM) {
    addStrike();
  }

}


