/*
 * Input parsing methods are based on
 * http://www.gammon.com.au/serial
 */

#ifdef DEBUGING
#define DEBUGING_COMLINK
// #define DEBUGING_COMLINK_PARS
#endif

#define COMLINK_MAX_INPUT_LENGTH 50
#define COMLINK_CHUNK_SIZE 5
#define COMLINK_WORK_ARRAY_SIZE 10

byte comlink_work_array[COMLINK_WORK_ARRAY_SIZE + 1];

void handle_serial_input() {
  while (Serial.available () > 0) {
    processIncomingByte(Serial.read());
  }
}

void processIncomingByte(const byte inByte) {
  static char input_line [COMLINK_MAX_INPUT_LENGTH];
  static unsigned int input_pos = 0;

  switch (inByte)
  {
    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte

      // terminator reached! process input_line here ...
      process_data(input_line);

      // reset buffer for next time
      input_pos = 0;
      break;

    case '\r':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (COMLINK_MAX_INPUT_LENGTH - 1))
        input_line [input_pos++] = inByte;
      break;

  }  // end of switch

} // end of processIncomingByte

// TODO: send Modules, sendModuleStates and sendModuleStages could be one function
void sendModules() {
  Serial.print(MODULE_MAX_COUNT);
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    Serial.print(" ");
    Serial.print(module_types[i]);
  }
  Serial.println();
}

void sendModuleStates() {
  Serial.print(MODULE_MAX_COUNT);
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    Serial.print(" ");
    Serial.print(module_status[i]);
  }
  Serial.println();
}

void sendModuleStages() {
  Serial.print(MODULE_MAX_COUNT);
  for (int i = 0; i < MODULE_MAX_COUNT; i++) {
    Serial.print(" ");
    Serial.print(module_stage[i]);
  }
  Serial.println();
}

void sendHeader() {
  Serial.print(F("KTaNE RealKit v"));
  Serial.print(vers);
#ifdef DEBUGING
  Serial.print(F(" DBG"));
#endif
  Serial.println();
}


/**
  Function parses a number 0-255 from charArray and store it to variable value.
  Returns index after last digit of parsed number.
  If no number is found, returns -1
*/
int parseByte(byte* value, char* charArray) {
  *value = 0;
  int pos = 0;
  boolean digitParsed = false;

#ifdef DEBUGING_COMLINK_PARSE
  Serial.print(debug_print_char);
  Serial.print("PB: ");
#endif

  while (charArray[pos] != '\0' && (!isDigit(charArray[pos]))) {
#ifdef DEBUGING_COMLINK_PARSE
    Serial.print(charArray[pos]);
    Serial.print(" ");
#endif
    pos++;
  }

#ifdef DEBUGING_COMLINK_PARSE
  Serial.println();
  Serial.print(debug_print_char);
  Serial.print("PA: ");
#endif

  while (charArray[pos] != '\0') {
#ifdef DEBUGING_COMLINK_PARSE
    Serial.print(charArray[pos]);
    Serial.print(" ");
#endif
    if (isDigit(charArray[pos])) {
      digitParsed = true;
      *value *= 10;
      *value += charArray[pos] - '0';
      pos++;
    } else {
      pos++;
      break;
    }
  }

  if (!digitParsed) {
    pos = -1;
  }

#ifdef DEBUGING_COMLINK_PARSE
  Serial.println();
  Serial.print(debug_print_char);
  Serial.print(*value);
  Serial.print(" ");
  Serial.println(pos);
#endif

  return pos;
}


boolean parseBytesToWorkArray(char* data, byte count) {
  byte value;
  int pos = 0;
  for (int i = 0; i < COMLINK_WORK_ARRAY_SIZE; i++) {
    if (i >= count) {
#ifdef DEBUGING_COMLINK
      Serial.print(debug_print_char);
      for (int j = 0; j < COMLINK_WORK_ARRAY_SIZE; j++) {
        Serial.print(comlink_work_array[j]);
        Serial.print(" ");
      }
      Serial.println();
#endif
      return true;
    }
    data += pos;
    pos = parseByte(&value, data);
    if (pos == -1) {
      return false;
    } else {
      comlink_work_array[i] = value;
    }
  }
  return false;
}

void parseTest(char* data) {
  byte value;
  int pos = 0;
  while (pos != -1) {
    data += pos;
    pos = parseByte(&value, data);
    // process value
    Serial.print(debug_print_char);
    Serial.print(F("Parsed: "));
    Serial.print(value);
    Serial.print(" ");
    Serial.println(pos);
  }
}

void sendStrikes() {
  Serial.print(strikes);
  Serial.print(" ");
  Serial.println(max_strikes);
}

void setStrikes(char* data) {
  if (!parseBytesToWorkArray(data, 2)) {
    sendErrorMessage();
    return;
  }
  strikes = comlink_work_array[0];
  max_strikes = comlink_work_array[1];

  sendSuccesMessage();
}


void sendErrorMessage() {
  Serial.println(F("IOERROR"));
}

void sendSuccesMessage() {
  Serial.println(F("OK"));
}

void setModuleVariable(char* data) {
  char varTypeDiscr = data[0];

  if (!parseBytesToWorkArray(data, 2)) {
    sendErrorMessage();
    return;
  }

  byte module_number = comlink_work_array[0];

  if (module_number >= MODULE_MAX_COUNT) {
    sendErrorMessage();
    return;
  }

  byte value = comlink_work_array[1];

  switch (varTypeDiscr) {
    case 'T':
      module_types[module_number] = value;
      break;
    case 'S':
      module_status[module_number] = value;
      break;
    case 'G':
      module_stage[module_number] = value;
      break;
    default:
      sendErrorMessage();
  }

  sendSuccesMessage();

}

void sendModuleData(char* data) {
  if (!parseBytesToWorkArray(data, 1)) {
    sendErrorMessage();
    return;
  }

  byte module_number = comlink_work_array[0];
  if (module_number >= MODULE_MAX_COUNT) {
    sendErrorMessage();
    return;
  }

  Serial.print(MODULE_DATA_SIZE);
  for (int i = 0; i < MODULE_DATA_SIZE; i++) {
    Serial.print(" ");
    Serial.print(module_data[module_number][i]);
  }
  Serial.println();
}


/**
 * Sets data of module in given chunk.
 * Chunk is simply part of module data. It is used because sending all data in one line
 * would not fit to the buffer used by input parser.
 */
void setModuleDataChunk(char* data) {
  if (!parseBytesToWorkArray(data, COMLINK_CHUNK_SIZE + 2)) {
    sendErrorMessage();
    return;
  }

  byte module_number = comlink_work_array[0];
  if (module_number >= MODULE_MAX_COUNT) {
    sendErrorMessage();
    return;
  }

  byte chunk_number = comlink_work_array[1];

  byte pos;
  for (int i = 0; i < COMLINK_CHUNK_SIZE; i++) {
    pos = i + chunk_number * COMLINK_CHUNK_SIZE;
    if (pos >= MODULE_DATA_SIZE) {
      break;
    }
    module_data[module_number][pos] = comlink_work_array[i + 2];
  }

  sendSuccesMessage();
}

void sendRemainingTime() {
  Serial.print(remainingTime);
}

void setRemainingTime(char* data) {
  if (!parseBytesToWorkArray(data, 4)) {
    sendErrorMessage();
    return;
  }

  unsigned long value = 0;
  // Most Significant BYTE (not bit) FIRST
  value = (value << 8) + comlink_work_array[0];
  value = (value << 8) + comlink_work_array[1];
  value = (value << 8) + comlink_work_array[2];
  value = (value << 8) + comlink_work_array[3];
  remainingTime = value;
  sendSuccesMessage();
}

void setClockTicking(char* data) {
  if (!parseBytesToWorkArray(data, 1)) {
    sendErrorMessage();
    return;
  }
  clockTicking = (remainingTime > 0) && (comlink_work_array[0] > 0);
  sendSuccesMessage();
}

void process_data(char* data)
{
#ifdef DEBUGING_COMLINK
  Serial.print(debug_print_char);
  Serial.print(F("GOT DATA: "));
  Serial.println(data);
#endif

  if (data[0] == 'G') {
    // getters
    if (data[1] == 'M') {
      switch (data[2]) {
        case 'T': sendModules();
          break;
        case 'S': sendModuleStates();
          break;
        case 'G': sendModuleStages();
          break;
        case 'D': sendModuleData(data);
          break;
        default:
          ;
      }
    }

    if (data[1] == 'S') {
      sendStrikes();
    }

    if (data[1] == 'H') {
      sendHeader();
    }

    if (data[1] == 'R') {
      Serial.println(remainingTime);
    }

    if (data[1] == 'B') {
      Serial.println(boomTime);
    }

    if (data[1] == 'C' && data[2] == 'S') {
      Serial.println(COMLINK_CHUNK_SIZE);
    }

  }

  if (data[0] == 'S') {
    if (data[1] == 'M') {
      if (data[2] == 'T' || data[2] == 'S' || data[2] == 'G') {
        setModuleVariable(data + 2);
      }

      if (data[2] == 'D') {
        setModuleDataChunk(data + 2);
      }
    }

    if (data[1] == 'S') {
      setStrikes(data);
    }

    if (data[1] == 'R') {
      setRemainingTime(data);
    }

    if (data[1] == 'T') {
      setClockTicking(data);
    }
  }

  if (data[0] == 'I') {
    // TODO: run initializations

  }

  if (data[0] == 'T') {
    // TODO: run tests
    initModulesTest();

  }

#ifdef DEBUGING_COMLINK
  if (data[0] == 'P') {
    parseTest(data);
  }
#endif


}  // end of process_data

