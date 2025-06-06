#ifndef LVM_H
#define LVM_H
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

// 1. designated init
// 2. c99 c11区别
// 3. gcc switch-enum： -Wswitch-enum 是一个 编译警告选项，用于在 switch 语句处理枚举类型（enum）时，检查是否覆盖了该枚举类型的所有可能值
// 4. memchr

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define LVM_STACK_CAPCITY 1024
#define LVM_PROGRAM_CAPACITY 1024
#define LVM_EXECUTION_LIMIT 128

typedef int64_t Word;

typedef enum {
  ERR_OK = 0,
  ERR_STACK_OVERFLOW,
  ERR_STACK_UNDERFLOW,
  ERR_ILLEGAL_INST,
  ERR_ILLEGAL_INST_ACCESS,
  ERR_ILLEGAL_OPERAND,
  ERR_DIV_BY_ZERO,
} Err;

const char *err_as_cstr(Err err)
{
  switch (err) {
  case ERR_OK:
    return "ERR_OK";
  case ERR_STACK_OVERFLOW:
    return "ERR_STACK_OVERFLOW";
  case ERR_STACK_UNDERFLOW:
    return "ERR_STACK_UNDERFLOW";
  case ERR_ILLEGAL_INST:
    return "ERR_ILLEGAL_INST";
  case ERR_ILLEGAL_OPERAND:
    return "ERR_ILLEGAL_OPERAND";
  case ERR_ILLEGAL_INST_ACCESS:
    return "ERR_ILLEGAL_INST_ACCESS";
  case ERR_DIV_BY_ZERO:
    return "ERR_DIV_BY_ZERO";
  default:
    assert(0 && "err_as_cstr: Unreachable");
  }
}

typedef enum {
  INST_NOP = 0,
  INST_PUSH,
  INST_DUP,
  INST_PLUS,
  INST_MINUS,
  INST_MULT,
  INST_DIV,
  INST_JMP,
  INST_JMP_IF,
  INST_EQ,
  INST_HALT,
  INST_PRINT_DEBUG,
} Inst_Type;

const char *inst_type_as_cstr(Inst_Type type)
{
  switch (type) {
  case INST_NOP:         return "INST_NOP";
  case INST_PUSH:        return "INST_PUSH";
  case INST_PLUS:        return "INST_PLUS";
  case INST_MINUS:       return "INST_MINUS";
  case INST_MULT:        return "INST_MULT";
  case INST_DIV:         return "INST_DIV";
  case INST_JMP:         return "INST_JMP";
  case INST_HALT:        return "INST_HALT";
  case INST_JMP_IF:      return "INST_JMP_IF";
  case INST_EQ:          return "INST_EQ";
  case INST_PRINT_DEBUG: return "INST_PRINT_DEBUG";
  case INST_DUP:         return "INST_DUP";
  default: assert(0 && "inst_type_as_cstr: unreachable");
  }
}

typedef struct {
  Inst_Type type;
  Word operand;
} Inst;

typedef struct {
    Word stack[LVM_STACK_CAPCITY];
    Word stack_size;
    
    Inst program[LVM_PROGRAM_CAPACITY];
    Word program_size;
    Word pc;
    
    int halt;
} LVM;

#define MAKE_INST_PUSH(value) {.type = INST_PUSH, .operand = (value)}
#define MAKE_INST_PLUS        {.type = INST_PLUS}
#define MAKE_INST_MINUS       {.type = INST_MINUS}
#define MAKE_INST_MULT        {.type = INST_MULT}
#define MAKE_INST_DIV         {.type = INST_DIV}
#define MAKE_INST_JMP(addr)   {.type = INST_JMP, .operand = (addr)}
#define MAKE_INST_DUP(addr)   {.type = INST_DUP, .operand = (addr)}
#define MAKE_INST_HALT        {.type = INST_HALT, .operand = (addr)}


Err lvm_execute_inst(LVM* lvm) {
  if (lvm->pc < 0 || lvm->pc >= lvm->program_size) {
    return ERR_ILLEGAL_INST_ACCESS;
  }

  Inst inst = lvm->program[lvm->pc];

  switch (inst.type) {
  case INST_NOP: 
    lvm->pc += 1;
    break;
  
  case INST_PUSH:
    if (lvm->stack_size >= LVM_STACK_CAPCITY) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size++]= inst.operand;
    lvm->pc += 1;
    break;
  case INST_PLUS:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 2] += lvm->stack[lvm->stack_size - 1];
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_MINUS:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 2] -= lvm->stack[lvm->stack_size - 1];
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_DIV:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    if (lvm->stack[lvm->stack_size -1] == 0) {
      return ERR_DIV_BY_ZERO;
    }
    lvm->stack[lvm->stack_size - 2] /= lvm->stack[lvm->stack_size - 1];
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_MULT:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 2] *= lvm->stack[lvm->stack_size - 1];
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_JMP:
    lvm->pc = inst.operand;
    break;
  case INST_HALT:
    lvm->halt = 1;
    break;
  case INST_EQ:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 2] = lvm->stack[lvm->stack_size - 1] == lvm->stack[lvm->stack_size - 2];
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_JMP_IF:
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    if (lvm->stack[lvm->stack_size -1]) {
      lvm->stack_size -= 1;
      lvm->pc = inst.operand;
    }else {
      lvm->pc += 1;
    }
    break;
  case INST_PRINT_DEBUG:
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    printf("%ld\n",lvm->stack[lvm->stack_size - 1]);
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_DUP:
    if (lvm->stack_size >= LVM_STACK_CAPCITY) {
      return ERR_STACK_OVERFLOW;
    }
    if (lvm->stack_size - inst.operand <=0) {
      return ERR_STACK_UNDERFLOW;
    }
    if (inst.operand < 0) {
      return ERR_ILLEGAL_INST;
    }
    lvm->stack[lvm->stack_size] = lvm->stack[lvm->stack_size - 1 - inst.operand];
    lvm->stack_size += 1;
    lvm->pc += 1;
    break;
  default:
    return ERR_ILLEGAL_INST;
  }
  return ERR_OK;
}

Err lvm_execute_program(LVM *lvm, int limit) {
  while (limit !=0 &&  ! lvm->halt) {
    Err err = lvm_execute_inst(lvm);
    if (err != ERR_OK) {
      return err;
    }
    if (limit > 0) {
      --limit;
    }
  }

  return ERR_OK;
}


void lvm_dump_stack(FILE* stream, const LVM* lvm) {
  fprintf(stream, "Stack:\n");
  if (lvm->stack_size > 0) {
    for (Word i =0; i < lvm->stack_size; i++) {
      fprintf(stream, "  %ld\n", lvm->stack[i]);
    }
  }else {
    fprintf(stream, "  [empty]\n");
  }
}

void lvm_load_program_from_memory(LVM* lvm, Inst * program,size_t program_size) {
  assert(program_size < LVM_PROGRAM_CAPACITY);
  memcpy(lvm->program,program,sizeof(program[0])* program_size);
  lvm->program_size = program_size;
}

void lvm_load_program_from_file(LVM* lvm, const char* file_path) {
  FILE* f = fopen(file_path, "rb");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cound not open file `%s` : %s\n",
	    file_path,strerror(errno));
    exit(1);
  }

  //seek to end
  if (fseek(f, 0, SEEK_END) < 0) {
    fprintf(stderr, "ERROR: Cound not read file `%s` : %s\n",
	    file_path,strerror(errno));
    exit(1);
  }

  long m = ftell(f);
  if (m<0) {
    fprintf(stderr, "ERROR: Cound not read file `%s` : %s\n",
	    file_path,strerror(errno));
    exit(1);    
  }

  assert(m % sizeof(lvm->program[0]) == 0);
  assert((size_t) m <= LVM_PROGRAM_CAPACITY * sizeof(lvm->program[0]));

  if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
                file_path, strerror(errno));
        exit(1);
    }

    lvm->program_size = fread(lvm->program, sizeof(lvm->program[0]), m / sizeof(lvm->program[0]), f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
                file_path, strerror(errno));
        exit(1);
    }

    fclose(f);
}

void lvm_save_program_to_file(const LVM* lvm, const char* file_path) {
  FILE* f = fopen(file_path, "wb");
  if (f==NULL) {
    fprintf(stderr, "ERROR: Cound not open file `%s` : %s\n",
	    file_path,strerror(errno));
    exit(1);
  }

   fwrite(lvm->program, sizeof(lvm->program[0]), lvm->program_size, f);

    if (ferror(f)) {
        fprintf(stderr, "ERROR: Could not write to file `%s`: %s\n",
                file_path, strerror(errno));
        exit(1);
    }

    fclose(f);
}

LVM lvm = {0};

typedef struct  {
  size_t count;
  const char* data;
} String_View;

// 复合字面量（Compound Literal）允许在代码中直接创建并初始化一个匿名对象
// (type_name) { initializer-list }
String_View cstr_as_sv(const char *cstr)
{
    return (String_View) {
        .count = strlen(cstr),
        .data = cstr,
    };
}
String_View sv_trim_left(String_View sv) {
  size_t i = 0;
    while (i < sv.count && isspace(sv.data[i])) {
        i += 1;
    }

    return (String_View) {
        .count = sv.count - i,
        .data = sv.data + i,
    };
}

String_View sv_trim_right(String_View sv)
{
    size_t i = 0;
    while (i < sv.count && isspace(sv.data[sv.count - 1 - i])) {
        i += 1;
    }

    return (String_View) {
        .count = sv.count - i,
        .data = sv.data
    };
}

String_View sv_trim(String_View sv)
{
    return sv_trim_right(sv_trim_left(sv));
}

String_View sv_chop_by_delim(String_View *sv, char delim) {
  size_t i = 0;
  while (i < sv->count && sv->data[i] != delim) {
    i += 1;
  }

  String_View result = {
    .count = i,
    .data = sv->data,
  };

  if (i < sv->count) {
    sv->count -= i + 1;
    sv->data  += i + 1;
  } else {
    sv->count -= i;
    sv->data  += i;
  }

  return result;
}

int sv_eq(String_View a, String_View b)
{
  if (a.count != b.count) {
    return 0;
  } else {
    return memcmp(a.data, b.data, a.count) == 0;
  }
}

int sv_to_int(String_View sv)
{
  int result = 0;

  for (size_t i = 0; i < sv.count && isdigit(sv.data[i]); ++i) {
    result = result * 10 + sv.data[i] - '0';
  }

  return result;
}

Inst lvm_translate_line(String_View line) {
  line = sv_trim_left(line);
  String_View inst_name = sv_chop_by_delim(&line, ' ');

  if (sv_eq(inst_name, cstr_as_sv("push"))) {
    line = sv_trim_left(line);
    int operand = sv_to_int(sv_trim_right(line));
    return (Inst) {.type = INST_PUSH, .operand = operand};
  }else if (sv_eq(inst_name, cstr_as_sv("dup"))) {
    line = sv_trim_left(line);
    int operand = sv_to_int(sv_trim_right(line));
    return (Inst) { .type = INST_DUP, .operand = operand };
  } else if (sv_eq(inst_name, cstr_as_sv("plus"))) {
    return (Inst) { .type = INST_PLUS };
  } else if (sv_eq(inst_name, cstr_as_sv("jmp"))) {
    line = sv_trim_left(line);
    int operand = sv_to_int(sv_trim_right(line));
    return (Inst) { .type = INST_JMP, .operand = operand };
  } else {
    fprintf(stderr, "ERROR: unknown instruction `%.*s`",
            (int) inst_name.count, inst_name.data);
    exit(1);
  }
}

size_t lvm_translate_source(String_View source,
                           Inst *program, size_t program_capacity){
    size_t program_size = 0;
    while (source.count > 0) {
        assert(program_size < program_capacity);
        String_View line = sv_trim(sv_chop_by_delim(&source, '\n'));
        if (line.count > 0) {
            program[program_size++] = lvm_translate_line(line);
        }
    }
    return program_size;
}

String_View slurp_file(const char *file_path)
{
  FILE *f = fopen(file_path, "r");
  if (f == NULL) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
    exit(1);
  }

  if (fseek(f, 0, SEEK_END) < 0) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
    exit(1);
  }

  long m = ftell(f);
  if (m < 0) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
    exit(1);
  }

  char *buffer = malloc(m);
  if (buffer == NULL) {
    fprintf(stderr, "ERROR: Could not allocate memory for file: %s\n",
            strerror(errno));
    exit(1);
  }

  if (fseek(f, 0, SEEK_SET) < 0) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
    exit(1);
  }

  size_t n = fread(buffer, 1, m, f);
  if (ferror(f)) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path, strerror(errno));
    exit(1);
  }

  fclose(f);

  return (String_View) {
    .count = n,
    .data = buffer,
  };
}

#endif
