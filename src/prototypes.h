// Readable aliases for types
typedef uint32_t instruction;

// Type defs
typedef void (*funcptr)(instruction);

// Prototypes
void read_from_file(char* filename);
void write_to_file(char* filename);
void print_output(void);
funcptr decode(instruction instr);
void data_processing_immediate(instruction instr);
void data_processing_register(instruction instr);
void single_data_transfer(instruction instr);
void branch_instructions(instruction instr);
void initialise_memory(void);
void process_instructions(void);
void nop(instruction instr);