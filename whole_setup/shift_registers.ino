#ifdef DEBUGING
#define DEBUGING_SR
#endif


/*
  SETUP FOR INPUT/OUTPUT SHIFT REGISTERS
*/

#define SHIFT_REGISTER_INPUT_CL_EN_PIN 5
#define SHIFT_REGISTER_INPUT_PLOAD_PIN 6
#define SHIFT_REGISTER_INPUT_DATA_PIN 7
#define SHIFT_REGISTER_INPUT_CLOCK_PIN 8

#define SHIFT_REGISTER_OUTPUT_DATA_PIN 10
#define SHIFT_REGISTER_OUTPUT_LATCH_PIN 11
#define SHIFT_REGISTER_OUTPUT_CLOCK_PIN 8

void setup_shift_registers() {
  // input register
  pinMode(SHIFT_REGISTER_INPUT_PLOAD_PIN, OUTPUT);
  pinMode(SHIFT_REGISTER_INPUT_CL_EN_PIN, OUTPUT);
  pinMode(SHIFT_REGISTER_INPUT_DATA_PIN, INPUT);
  pinMode(SHIFT_REGISTER_INPUT_CLOCK_PIN, OUTPUT);

  // output register
  pinMode(SHIFT_REGISTER_OUTPUT_DATA_PIN, OUTPUT);
  pinMode(SHIFT_REGISTER_OUTPUT_LATCH_PIN, OUTPUT);
  pinMode(SHIFT_REGISTER_OUTPUT_CLOCK_PIN, OUTPUT);

}

void write_to_output_shift_register() {

  byte output_width = SRoffsetsOutput[MODULE_MAX_COUNT + 1];

#ifdef DEBUGING_SR
  Serial.print(F("SR_OUT:"));
  Serial.println(output_width);
#endif

  digitalWrite(SHIFT_REGISTER_OUTPUT_LATCH_PIN, LOW);

  for (int i = 0; i < output_width; i++) {

#ifdef DEBUGING_SR
    Serial.print(i);
    Serial.print(" d:");
    Serial.println(shift_register_output[i]);
#endif

    shiftOut(SHIFT_REGISTER_OUTPUT_DATA_PIN, SHIFT_REGISTER_OUTPUT_CLOCK_PIN, MSBFIRST,
             shift_register_output[i]);
  }

  digitalWrite(SHIFT_REGISTER_OUTPUT_LATCH_PIN, HIGH);

}

void update_shift_registers() {
  write_to_output_shift_register();
  // TODO: input shift register
}


