#include "./lvm.h"

//LVM lvm = {0};

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: ./delsm <input.lvm>\n");
        fprintf(stderr, "ERROR: no input is provided\n");
        exit(1);
    }

    const char *input_file_path = argv[1];

    lvm_load_program_from_file(&lvm, input_file_path);

    for (Inst_Addr i = 0; i < lvm.program_size; ++i) {
      printf("%s", inst_name(lvm.program[i].type));
      if (inst_has_operand(lvm.program[i].type)) {
            printf(" %ld", lvm.program[i].operand.as_i64);
	}
	printf("\n");
    }

    return 0;
}
