#ifdef DEBUGING
#define DEBUGING_TESTMODULE_OUTPUT
#endif

/*
  SETUP FOR TESMODULE_OUTPUT
*/


void test_testmodule_output(byte module_number) {

  int delayTime = 600;

  for (int j = 0; j < 2; j++) {

    for (int shift = 0; shift < 2; shift++) {

      byte pos = SRoffsetsOutput[module_number] + shift;

      shift_register_output[pos] = 0;
      update_shift_registers();
      delay(delayTime);
      shift_register_output[pos] = 1;
      update_shift_registers();
      delay(delayTime);
      for (int i = 1; i < 8; i++) {
        shift_register_output[pos] = shift_register_output[pos] << 1;
        write_to_output_shift_register();
        delay(delayTime);
      }

    }

  }

}

void setup_testmodule_output(byte module_number) {

}

void update_testmodule_output(byte module_number) {
  if (module_status[module_number] == MODULE_TESTING) {
    test_testmodule_output(module_number);
    // at the end of test module disarm itself
    module_status[module_number] = MODULE_DISARMED;
  }
  else {
    // reset output to zeros
    for (int shift = 0; shift < 2; shift++) {

      byte pos = SRoffsetsOutput[module_number] + shift;
      shift_register_output[pos] = 0;
    }

  }
}

