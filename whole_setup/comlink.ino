#ifdef DEBUGING
#define DEBUGING_COMLINK
#endif

void handle_input_serial() {

  byte in1;
  byte in2;

  // read serial if available
  if (Serial.available() > 1) {
#ifdef DEBUGING_COMLINK
    Serial.print(debug_print_char);
    Serial.println("READING");
#endif

    in1 = Serial.read();
    in2 = Serial.read();

#ifdef DEBUGING_COMLINK
    Serial.print(in1);
    Serial.println(in2);
#endif


    if (in1 == 'G') {
      // getters

      switch (in2) {
        case 'M': sendModules();
          break;
        default:
          ;
      }

    }

  }

}


void sendModules() {
  Serial.print((char) MODULE_MAX_COUNT);
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    Serial.print((char) module_types[i]);
  }
  Serial.println();
}


