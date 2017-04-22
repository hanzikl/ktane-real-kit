#ifdef DEBUGING
#define DEBUGING_TESTING
#endif

byte module_tested = 0;

void initModulesTest() {

  // zapneme testovani
  modules_testing = true;

  // vypneme vsechny moduly
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    module_status[i] = MODULE_DISARMED;
  }

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


boolean testModule(byte module_number) {
  if (module_number < MODULE_MAX_COUNT) {
#ifdef DEBUGING_TESTING
    Serial.print(F("Testing module: "));
    Serial.println(module_tested);
#endif
    module_status[module_number] = MODULE_TESTING;
    module_stage[module_number] = 0;
    return true;
  } else {
    return false;
  }
}

void administerTests() {

  while (true) {

    if (module_types[module_tested] == MODULE_TYPE_MISSING) {
      // cannot test missing module
#ifdef DEBUGING_TESTING
      Serial.print(F("Missing module: "));
      Serial.println(module_tested);
#endif
      module_status[module_tested] = MODULE_DISARMED;
    }

    if (module_status[module_tested] == MODULE_TESTING) {
      // modul se stale testuje
      return;
    }

    module_tested++;
    if (!testModule(module_tested)) {
      break;
    }
  }

  // all modules tested, turn off module testing
  module_tested = 0;
  modules_testing = false;
#ifdef DEBUGING_TESTING
  Serial.println(F("All modules tested"));
#endif
}


