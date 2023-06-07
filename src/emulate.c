#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emulatesetup.h"
#include "emulateio.h"
#include "emulateinstructions.h"

int main(int argc, char **argv) {

  if (argc == 2 || argc == 3) {
    // Initialise the state of the memory
    initialise_memory();

    // Load binary file into memory
    read_from_file(argv[1]);

    // Process all of the read-in instructions
    process_instructions();

    if (argc == 2) {
      // Output the state of the memory
      print_output();
    } else {
      // Write the state of the memory to the file
      write_to_file(argv[2]);
    }

    return EXIT_SUCCESS;
  } else {
    printf("Usage: ./emulate <input-filename> <output-filename>\n");
    return EXIT_FAILURE;
  }
}