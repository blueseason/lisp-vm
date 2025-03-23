#include "./lvm.c"

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: ./lasm <input.lasm> <output.lvm>\n");
        fprintf(stderr, "ERROR: expected input and output\n");
        exit(1);
    }

    const char *input_file_path = argv[1];
    const char *output_file_path = argv[2];

    String_View source = slurp_file(input_file_path);

    lvm.program_size = lvm_translate_source(source,
                                          lvm.program,
                                          LVM_PROGRAM_CAPACITY);

    lvm_save_program_to_file(lvm.program, lvm.program_size, output_file_path);

    return 0;
}
