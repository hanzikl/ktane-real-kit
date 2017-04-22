#ifdef DEBUGING
#define DEBUGING_TESTING
#endif

byte module_tested = 0;

void initModulesTest() {
  // nalezneme prvni testovatelny modul
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    if (module_types[i] != MODULE_TYPE_MISSING) {
      // a spustime na nem test
      module_tested = i;
      module_status[i] = MODULE_TESTING;
      module_stage[i] = 0;
      break;
    }
  }
  administerTests();
}


void testModule(byte module_number) {
  module_status[module_number] = MODULE_TESTING;
  module_stage[module_number] = 0;
}

void administerTests() {

  while (true) {

#ifdef DEBUGING_TESTING
    Serial.print("Testing: ");
    Serial.println(module_tested);
#endif

    if (module_types[module_tested] == MODULE_TYPE_MISSING) {
      // cannot test missing module
      module_status[module_tested] = MODULE_DISARMED;
      continue;
    }

    if (module_status[module_tested] == MODULE_TESTING) {
      // modul se stale testuje
      return;
    }

    module_tested++;
    if (module_tested >= MODULE_MAX_COUNT) {
      break;
    }

    // test next module
    testModule(module_tested);
  }

  // all modules tested, turn off module testing
  module_tested = 0;
  modules_testing = false;
}


