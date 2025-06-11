#include "lvm.h"
#include <stdio.h>

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
    fprintf(stream, "Usage: %s -i <input.lvm> [-l <limit>] [-h] [-d]\n", program);
}


static Err lvm_alloc(LVM *lvm)
{
    if (lvm->stack_size < 1) {
        return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 1].as_ptr = malloc(lvm->stack[lvm->stack_size - 1].as_u64);

    return ERR_OK;
}

static Err lvm_free(LVM *lvm)
{
    if (lvm->stack_size < 1) {
        return ERR_STACK_UNDERFLOW;
    }

    free(lvm->stack[lvm->stack_size - 1].as_ptr);
    lvm->stack_size -= 1;

    return ERR_OK;
}

static Err lvm_print_f64(LVM *lvm)
{
    if (lvm->stack_size < 1) {
        return ERR_STACK_UNDERFLOW;
    }

    printf("%lf\n", lvm->stack[lvm->stack_size - 1].as_f64);
    lvm->stack_size -= 1;
    return ERR_OK;
}


static Err lvm_print_i64(LVM *lvm)
{
    if (lvm->stack_size < 1) {
        return ERR_STACK_UNDERFLOW;
    }

    printf("%" PRId64 "\n", lvm->stack[lvm->stack_size - 1].as_i64);
    lvm->stack_size -= 1;
    return ERR_OK;
}

static Err lvm_print_u64(LVM *lvm)
{
    if (lvm->stack_size < 1) {
        return ERR_STACK_UNDERFLOW;
    }

    printf("%" PRIu64 "\n", lvm->stack[lvm->stack_size - 1].as_u64);
    lvm->stack_size -= 1;
    return ERR_OK;
}

static Err lvm_print_ptr(LVM *lvm)
{
    if (lvm->stack_size < 1) {
        return ERR_STACK_UNDERFLOW;
    }

    printf("%p\n", lvm->stack[lvm->stack_size - 1].as_ptr);
    lvm->stack_size -= 1;
    return ERR_OK;
}


int main(int argc, char *argv[])
{
  const char *program = shift(&argc, &argv);
  const char *input_file_path = NULL;
  int limit = -1;
  int debug = 0;

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
    } else if (strcmp(flag, "-d") == 0) {
      debug = 1;
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
  
  lvm_load_program_from_file(&lvm, input_file_path);
  lvm_push_native(&lvm, lvm_alloc);     // 0
  lvm_push_native(&lvm, lvm_free);      // 1
  lvm_push_native(&lvm, lvm_print_f64); // 2
  lvm_push_native(&lvm, lvm_print_i64); // 3
  lvm_push_native(&lvm, lvm_print_u64); // 4
  lvm_push_native(&lvm, lvm_print_ptr); // 5
  
  if (!debug) {
    Err err = lvm_execute_program(&lvm,limit);
    //lvm_dump_stack(stdout,&lvm);
    if (err != ERR_OK) {
      fprintf(stderr, "ERROR: %s\n", err_as_cstr(err));
      exit(1);
    }    
  }else {
    while (limit != 0 && !lvm.halt) {
      lvm_dump_stack(stdout, &lvm);
      printf("Instruction: %s %" PRIu64 "\n",
             inst_name(lvm.program[lvm.pc].type),
	     lvm.program[lvm.pc].operand.as_u64);
      getchar();
      Err err = lvm_execute_inst(&lvm);
      if (err != ERR_OK) {
        fprintf(stderr, "ERROR: %s\n", err_as_cstr(err));
        return 1;
      }
      if (limit > 0) {
        --limit;
      }
    }
  }

  return 0;
}
