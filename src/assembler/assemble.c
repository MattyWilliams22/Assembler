#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "assembleinstructions.h"
#include "tokenizer.h"

int main(int argc, char **argv) {
  // Open the input file
  FILE* fp = fopen(argv[1], "rt");
	if (fp == NULL) {
		perror("Could not open input file.");
		exit(EXIT_FAILURE);
	}

  // Get number of lines in input file
  int nlines = count_lines(fp);

  // Variable to store the number of lines which contain assembly instructions
  int ntokenlines = 0;

  // Convert lines of file to an array of token_lines
  token_line *token_lines = read_assembly(fp, nlines, &ntokenlines);
  
  // Convert token_lines to binary_lines
  binary binary_lines[ntokenlines];
  for (int i = 0; i < ntokenlines; i++) {
    binary_lines[i] = assemble_line(token_lines[i]);
  }

  // Writes binary_lines to output file
  write_to_binary_file(argv[2], binary_lines, ntokenlines, token_lines);

  return EXIT_SUCCESS;
}
