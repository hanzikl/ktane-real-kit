

/**
   each module has two functions:
   setup_{module_name}(module_number)
   update_{module_name}(module_number)

   in this file we call correct functions for given module according to it's type
   Practically we are reimplementing inheritance from OOP.
*/

void call_module_setup(byte module_number) {
  switch (module_types[module_number]) {
    case MODULE_TYPE_SIMON:
      setup_simon(module_number);
      break;
    case MODULE_TYPE_SYMBOLS:
      setup_symbols(module_number);
      break;
    case MODULE_TYPE_TEST_OUTPUT:
      setup_testmodule_output(module_number);
      break;
    default:
      ;
  }
}

void call_module_update(byte module_number) {
  switch (module_types[module_number]) {
    case MODULE_TYPE_SIMON:
      update_simon(module_number);
      break;
    case MODULE_TYPE_SYMBOLS:
      update_symbols(module_number);
      break;
    case MODULE_TYPE_TEST_OUTPUT:
      update_testmodule_output(module_number);
      break;
    default:
      ;
  }
}

