#include "lvm.h"
#include <stdio.h>

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
    fprintf(stream, "Usage: %s -i <input.lvm> [-l <limit>] [-h]\n", program);
}


int main(int argc, char *argv[])
{
  const char *program = shift(&argc, &argv);
  const char *input_file_path = NULL;
  int limit = -1;

  while (argc > 0) {
    const char *flag = shift(&argc, &argv);

    if (strcmp(flag, "-i") == 0) {
      if (argc == 0) {
        usage(stderr, program);
        fprintf(stderr, "ERROR: No argument is provided for flag `%s`\n", flag);
        exit(1);
      }

      input_file_path = shift(&argc, &argv);
    } else if (strcmp(flag, "-l") == 0) {
      if (argc == 0) {
        usage(stderr, program);
        fprintf(stderr, "ERROR: No argument is provided for flag `%s`\n", flag);
        exit(1);
      }

      limit = atoi(shift(&argc, &argv));
    } else if (strcmp(flag, "-h") == 0) {
      usage(stdout, program);
      exit(0);
    } else {
      usage(stderr, program);
      fprintf(stderr, "ERROR: Unknown flag `%s`\n", flag);
      exit(1);
    }
  }

  if (input_file_path == NULL) {
    usage(stderr, program);
    fprintf(stderr, "ERROR: input was not provided\n");
    exit(1);
  }
  
  lvm_load_program_from_file(&lvm, argv[1]);
  Err err = lvm_execute_program(&lvm,limit);
  
  lvm_dump_stack(stdout,&lvm);
  if (err != ERR_OK) {
    fprintf(stderr, "ERROR: %s\n", err_as_cstr(err));
    exit(1);
  }
  return 0;
}
