#include "./lvm.h"

Label_Table lt = {0};

char *shift(int *argc, char ***argv);
void usage(FILE *stream, const char *program);

char *shift(int *argc, char ***argv)
{
  assert(*argc > 0);
  char *result = **argv;
  *argv += 1;
  *argc -= 1;
  return result;
}

void usage(FILE *stream, const char *program)
{
  fprintf(stream, "Usage: %s <input.lasm> <output.lvm>\n",program);
}


int main(int argc, char **argv)
{
  const char* program = shift(&argc, &argv);

  if (argc == 0) {
    usage(stderr, program);
    fprintf(stderr, "ERROR: expected input\n");
    exit(1);
  }
  
  const char *input_file_path = shift(&argc, &argv);

  if (argc == 0) {
    usage(stderr, program);
    fprintf(stderr, "ERROR: expected output\n");
    exit(1);
  }
  const char *output_file_path = shift(&argc, &argv);

  String_View source = slurp_file(input_file_path);

  lvm_translate_source(source,&lvm,&lt);

  lvm_save_program_to_file(&lvm, output_file_path);

  return 0;
}
