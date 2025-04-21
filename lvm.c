#include "lvm.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
  if (argc< 2) {
    printf(stderr,"Usage: ./lvm <input.lvm>\n");
    printf(stderr,"Error: need input\n");
    exit(1);
  }
  lvm_load_program_from_file(&lvm, argv[1]);
  for (int i =0; i < LVM_EXECUTION_LIMIT && ! lvm.halt; i++) {
    Err err = lvm_execute_inst(&lvm);
    lvm_dump_stack(stdout,&lvm);
    if (err != ERR_OK) {
      fprintf(stderr, "ERROR: %s\n", err_as_cstr(err));
      exit(1);
    }
  }
  return 0;
}
