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
#define LABEL_CAPACITY 1024
#define DEFERED_OPERANDS_CAPACITY 1024

typedef uint64_t Inst_Addr;

typedef union {
    uint64_t as_u64;
    int64_t as_i64;
    double as_f64;
    void *as_ptr;
} Word;

static_assert(sizeof(Word) == 8,
              "The LVM's Word is expected to be 64 bits");

typedef enum {
  ERR_OK = 0,
  ERR_STACK_OVERFLOW,
  ERR_STACK_UNDERFLOW,
  ERR_ILLEGAL_INST,
  ERR_ILLEGAL_INST_ACCESS,
  ERR_ILLEGAL_OPERAND,
  ERR_DIV_BY_ZERO,
} Err;

typedef enum {
  INST_NOP = 0,
  INST_PUSH,
  INST_DUP,
  INST_SWAP,
  INST_PLUSI,
  INST_MINUSI,
  INST_MULTI,
  INST_DIVI,
  INST_PLUSF,
  INST_MINUSF,
  INST_MULTF,
  INST_DIVF,
  INST_JMP,
  INST_JMP_IF,
  INST_EQ,
  INST_HALT,
  INST_NOT,
  INST_GEF,
  INST_PRINT_DEBUG,
  NUMBER_OF_INSTS,
} Inst_Type;

// Designated Initializers
// [INDEX] = value：指定初始化器（C99 特性）

const char *inst_name(Inst_Type type);
int inst_has_operand(Inst_Type type);


const char *inst_name(Inst_Type type)
{
    switch (type) {
    case INST_NOP:         return "nop";
    case INST_PUSH:        return "push";
    case INST_DUP:         return "dup";
    case INST_PLUSI:       return "plusi";
    case INST_MINUSI:      return "minusi";
    case INST_MULTI:       return "multi";
    case INST_DIVI:        return "divi";
    case INST_PLUSF:       return "plusf";
    case INST_MINUSF:      return "minusf";
    case INST_MULTF:       return "multf";
    case INST_DIVF:        return "divf";
    case INST_JMP:         return "jmp";
    case INST_JMP_IF:      return "jmp_if";
    case INST_EQ:          return "eq";
    case INST_HALT:        return "halt";
    case INST_PRINT_DEBUG: return "print_debug";
    case INST_SWAP:        return "swap";
    case INST_NOT:         return "not";
    case INST_GEF:         return "gef";
    case NUMBER_OF_INSTS:
    default: assert(0 && "inst_name: unreachable");
    }
}

int inst_has_operand(Inst_Type type)
{
    switch (type) {
    case INST_NOP:         return 0;
    case INST_PUSH:        return 1;
    case INST_DUP:         return 1;
    case INST_PLUSI:       return 0;
    case INST_MINUSI:      return 0;
    case INST_MULTI:       return 0;
    case INST_DIVI:        return 0;
    case INST_PLUSF:       return 0;
    case INST_MINUSF:      return 0;
    case INST_MULTF:       return 0;
    case INST_DIVF:        return 0;
    case INST_JMP:         return 1;
    case INST_JMP_IF:      return 1;
    case INST_EQ:          return 0;
    case INST_HALT:        return 0;
    case INST_PRINT_DEBUG: return 0;
    case INST_SWAP:        return 1;
    case INST_NOT:         return 0;
    case INST_GEF:         return 0;
    case NUMBER_OF_INSTS:
    default: assert(0 && "inst_name: unreachable");
    }
}


const char *err_as_cstr(Err err);
const char *inst_type_as_cstr(Inst_Type type);

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


const char *inst_type_as_cstr(Inst_Type type)
{
  switch (type) {
  case INST_NOP:		return "INST_NOP";
  case INST_PUSH:		return "INST_PUSH";
  case INST_PLUSI:		return "INST_PLUSI";
  case INST_MINUSI:		return "INST_MINUSI";
  case INST_MULTI:		return "INST_MULTI";
  case INST_DIVI:		return "INST_DIVI";
  case INST_PLUSF:		return "INST_PLUSF";
  case INST_MINUSF:		return "INST_MINUSF";
  case INST_MULTF:		return "INST_MULTF";
  case INST_DIVF:		return "INST_DIVF";
  case INST_JMP:		return "INST_JMP";
  case INST_HALT:		return "INST_HALT";
  case INST_JMP_IF:		return "INST_JMP_IF";
  case INST_EQ:			return "INST_EQ";
  case INST_PRINT_DEBUG:	return "INST_PRINT_DEBUG";
  case INST_DUP:		return "INST_DUP";
  case INST_SWAP:		return "INST_SWAP";
  case INST_NOT:		return "INST_NOT";
  case INST_GEF:		return "INST_GEF";
  case NUMBER_OF_INSTS:
  default: assert(0 && "inst_type_as_cstr: unreachable");
  }
}

typedef struct {
  Inst_Type type;
  Word operand;
} Inst;

typedef struct {
    Word stack[LVM_STACK_CAPCITY];
    uint64_t stack_size;
    
    Inst program[LVM_PROGRAM_CAPACITY];
    uint64_t program_size;
    Inst_Addr pc;
    
    int halt;
} LVM;


Err lvm_execute_inst(LVM* lvm);
Err lvm_execute_program(LVM *lvm, int limit);
void lvm_dump_stack(FILE* stream, const LVM* lvm);

void lvm_load_program_from_memory(LVM* lvm, Inst * program,size_t program_size);
void lvm_load_program_from_file(LVM* lvm, const char* file_path);
void lvm_save_program_to_file(const LVM* lvm, const char* file_path);

Err lvm_execute_inst(LVM* lvm) {
  if (lvm->pc >= lvm->program_size) {
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
  case INST_PLUSI:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 2].as_u64 += lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_MINUSI:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 2].as_u64 -= lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_DIVI:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    if (lvm->stack[lvm->stack_size -1].as_u64 == 0) {
      return ERR_DIV_BY_ZERO;
    }
    lvm->stack[lvm->stack_size - 2].as_u64 /= lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_MULTI:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 2].as_u64 *= lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_PLUSF:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_f64 += lvm->stack[lvm->stack_size - 1].as_f64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;
  case INST_MINUSF:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_f64 -= lvm->stack[lvm->stack_size - 1].as_f64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;
  case INST_MULTF:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_f64 *= lvm->stack[lvm->stack_size - 1].as_f64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;
  case INST_DIVF:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_f64 /= lvm->stack[lvm->stack_size - 1].as_f64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;
  case INST_JMP:
    lvm->pc = inst.operand.as_u64;
    break;
  case INST_HALT:
    lvm->halt = 1;
    break;
  case INST_EQ:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 2].as_u64 = lvm->stack[lvm->stack_size - 1].as_u64 == lvm->stack[lvm->stack_size - 2].as_u64;
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_GEF:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_u64 = lvm->stack[lvm->stack_size - 1].as_f64 >= lvm->stack[lvm->stack_size - 2].as_f64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;
  case INST_JMP_IF:
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    if (lvm->stack[lvm->stack_size -1].as_u64) {
      lvm->pc = inst.operand.as_u64;
    }else {
      lvm->pc += 1;
    }
    lvm->stack_size -= 1;
    break;
  case INST_PRINT_DEBUG:
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    fprintf(stdout, "  u64: %lu, i64: %ld, f64: %lf, ptr: %p\n",
            lvm->stack[lvm->stack_size - 1].as_u64,
            lvm->stack[lvm->stack_size - 1].as_i64,
            lvm->stack[lvm->stack_size - 1].as_f64,
            lvm->stack[lvm->stack_size - 1].as_ptr);
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_DUP:
    if (lvm->stack_size >= LVM_STACK_CAPCITY) {
      return ERR_STACK_OVERFLOW;
    }
    if (lvm->stack_size - inst.operand.as_u64 <=0) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size] = lvm->stack[lvm->stack_size - 1 - inst.operand.as_u64];
    lvm->stack_size += 1;
    lvm->pc += 1;
    break;
  case INST_SWAP:
    if (inst.operand.as_u64 >= lvm->stack_size) {
      return ERR_STACK_UNDERFLOW;
    }
    const uint64_t a = lvm->stack_size - 1;
    const uint64_t b = lvm->stack_size - 1 - inst.operand.as_u64;
    Word t = lvm->stack[a];
    lvm->stack[a] = lvm->stack[b];
    lvm->stack[b] = t;
    lvm->pc += 1;
    break;
  case INST_NOT:
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size - 1].as_u64 = !lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->pc += 1;
    break;
  case NUMBER_OF_INSTS:
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
    for (Inst_Addr i = 0; i < lvm->stack_size; ++i) {
      fprintf(stream, "  u64: %lu, i64: %ld, f64: %lf, ptr: %p\n",
              lvm->stack[i].as_u64,
              lvm->stack[i].as_i64,
              lvm->stack[i].as_f64,
              lvm->stack[i].as_ptr);
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

typedef struct {
    String_View name;
    Inst_Addr addr;
} Label;

typedef struct {
    Inst_Addr addr;
    String_View label;
} Defered_Operand;

typedef struct {
    Label labels[LABEL_CAPACITY];
    size_t labels_size;
    Defered_Operand defered_operands[DEFERED_OPERANDS_CAPACITY];
    size_t defered_operands_size;
} Lasm;

Word number_literal_as_word(String_View sv);

Inst_Addr label_table_find(const Lasm *lt, String_View name);
void label_table_push(Lasm *lt, String_View name, Inst_Addr addr);
void label_table_push_defered_operand(Lasm *lt, Inst_Addr addr, String_View label);

void lvm_translate_source(String_View source, LVM *lvm, Lasm *lt);

String_View cstr_as_sv(const char *cstr);
String_View sv_trim_left(String_View sv);
String_View sv_trim_right(String_View sv);
String_View sv_trim(String_View sv);
String_View sv_chop_by_delim(String_View *sv, char delim);
int sv_eq(String_View a, String_View b);
int sv_to_int(String_View sv);

void lvm_translate_source(String_View source,
                          LVM *lvm, Lasm *lt);
String_View slurp_file(const char *file_path);


Word number_literal_as_word(String_View sv)
{
    assert(sv.count < 1024);
    char cstr[sv.count + 1];
    char *endptr = 0;

    memcpy(cstr, sv.data, sv.count);
    cstr[sv.count] = '\0';

    Word result = {0};

    result.as_u64 = strtoull(cstr, &endptr, 10);
    if ((size_t) (endptr - cstr) != sv.count) {
        result.as_f64 = strtod(cstr, &endptr);
        if ((size_t) (endptr - cstr) != sv.count) {
            fprintf(stderr, "ERROR: `%s` is not a number literal\n", cstr);
            exit(1);
        }
    }

    return result;
}


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

//将str split，返回delim之前的str部分
String_View sv_chop_by_delim(String_View *sv, char delim) {
  size_t i = 0;
  while (i < sv->count && sv->data[i] != delim) {
    i += 1;
  }

  String_View result = {
    .count = i,
    .data = sv->data,
  };

  // 处理剩余部分offset
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

void lvm_translate_source(String_View source,
                          LVM *lvm, Lasm *lt){
  lvm->program_size = 0;
  
  while (source.count > 0) {
    assert(lvm->program_size < LVM_PROGRAM_CAPACITY);
    String_View line = sv_trim(sv_chop_by_delim(&source, '\n'));
    if (line.count > 0 && *line.data != '#') {
      String_View token = sv_chop_by_delim(&line, ' ');
      
      if (token.count > 0 && token.data[token.count - 1] == ':') {
        String_View label = {
          .count = token.count - 1,
          .data = token.data
        };

        label_table_push(lt, label, lvm->program_size);
	token = sv_trim(sv_chop_by_delim(&line, ' '));
      }
      if (token.count > 0) {
	// 处理 # 和 inst 在同一行，且 #在尾部的情况
	String_View operand = sv_trim(sv_chop_by_delim(&line, '#'));
	if (sv_eq(token, cstr_as_sv(inst_name(INST_NOP)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_NOP,
	  };
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_PUSH)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_PUSH,
            .operand = number_literal_as_word(operand)
          };
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_DUP)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_DUP,
            .operand = { .as_i64 = sv_to_int(operand) }
          };
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_PLUSI)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_PLUSI
          };
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_MINUSI)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_MINUSI
          };
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_DIVI)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_DIVI
          };
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_MULTI)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_MULTI
          };
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_JMP)))) {
	  if (operand.count > 0 && isdigit(*operand.data)) {
	    lvm->program[lvm->program_size++] = (Inst) {
              .type = INST_JMP,
	      .operand = { .as_i64 = sv_to_int(operand) },
            };
	  }else {
            label_table_push_defered_operand(lt, lvm->program_size, operand);
            lvm->program[lvm->program_size++] = (Inst) {
              .type = INST_JMP
            };
	  }
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_JMP_IF)))) {
          if (operand.count > 0 && isdigit(*operand.data)) {
            lvm->program[lvm->program_size++] = (Inst) {
              .type = INST_JMP_IF,
              .operand = { .as_i64 = sv_to_int(operand) },
            };
          } else {
            label_table_push_defered_operand(lt, lvm->program_size, operand);
            lvm->program[lvm->program_size++] = (Inst) {
              .type = INST_JMP_IF,
            };
          }
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_PLUSF)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_PLUSF
          };
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_MINUSF)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_MINUSF
          };
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_DIVF)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_DIVF
          };
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_MULTF)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_MULTF
          };
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_SWAP)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_SWAP,
	    .operand = { .as_i64 = sv_to_int(operand) },
          };
        } else if (sv_eq(token, cstr_as_sv(inst_name(INST_HALT)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_HALT
          };
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_EQ)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_EQ,
          };
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_GEF)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_GEF,
          };
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_NOT)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_NOT,
          };
	} else if (sv_eq(token, cstr_as_sv(inst_name(INST_PRINT_DEBUG)))) {
          lvm->program[lvm->program_size++] = (Inst) {
            .type = INST_PRINT_DEBUG,
          };
	} else {
          fprintf(stderr, "ERROR: unknown instruction `%.*s`\n",
                  (int) token.count, token.data);
          exit(1);
	}
      }
    }
  }

  for (size_t i = 0; i < lt->defered_operands_size;i++) {
    Inst_Addr addr = label_table_find(lt, lt->defered_operands[i].label);
    //inst 从0开始， 替换jmp指令的地址为解析label的inst地址
    lvm->program[lt->defered_operands[i].addr].operand.as_u64 = addr;
  }
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


Inst_Addr label_table_find(const Lasm *lt, String_View name)
{
    for (size_t i = 0; i < lt->labels_size; ++i) {
        if (sv_eq(lt->labels[i].name, name)) {
            return lt->labels[i].addr;
        }
    }

    fprintf(stderr, "ERROR: label `%.*s` does not exist\n",
            (int) name.count, name.data);
    exit(1);
}

void label_table_push(Lasm *lt, String_View name, Inst_Addr addr)
{
    assert(lt->labels_size < LABEL_CAPACITY);
    lt->labels[lt->labels_size++] = (Label) {.name = name, .addr = addr};
}

void label_table_push_defered_operand(Lasm *lt, Inst_Addr addr, String_View label)
{
    assert(lt->defered_operands_size < DEFERED_OPERANDS_CAPACITY);
    lt->defered_operands[lt->defered_operands_size++] =
        (Defered_Operand) {.addr = addr, .label = label};
}

#endif
