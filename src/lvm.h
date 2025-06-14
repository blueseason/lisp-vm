#ifndef LVM_H
#define LVM_H
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>

// 1. designated init
// 2. c99 c11区别
// 3. gcc switch-enum： -Wswitch-enum 是一个 编译警告选项，用于在 switch 语句处理枚举类型（enum）时，检查是否覆盖了该枚举类型的所有可能值
// 4. memchr

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define LVM_STACK_CAPACITY 1024
#define LVM_NATIVES_CAPACITY 1024
#define LVM_PROGRAM_CAPACITY 1024
#define LVM_EXECUTION_LIMIT 128
#define LABEL_CAPACITY 1024
#define DEFERED_OPERANDS_CAPACITY 1024
#define NUMBER_LITERAL_CAPACITY 1024

#define LASM_COMMENT_SYMBOL ';'
#define LASM_PP_SYMBOL '%'
#define LASM_MAX_INCLUDE_LEVEL 64

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
  INST_DROP,
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
  INST_RET,
  INST_CALL,
  INST_NATIVE,
  INST_HALT,
  INST_NOT,
  INST_GEF,
  INST_PRINT_DEBUG,
  NUMBER_OF_INSTS,
} Inst_Type;

// Designated Initializers
// [INDEX] = value：指定初始化器（C99 特性）

typedef struct  {
  size_t count;
  const char* data;
} String_View;

#define SV_FORMAT(sv) (int) sv.count, sv.data

String_View cstr_as_sv(const char *cstr);
String_View sv_trim_left(String_View sv);
String_View sv_trim_right(String_View sv);
String_View sv_trim(String_View sv);
String_View sv_chop_by_delim(String_View *sv, char delim);
int sv_eq(String_View a, String_View b);
int sv_to_int(String_View sv);

String_View slurp_file(String_View file_path);

const char *inst_name(Inst_Type type);
int inst_has_operand(Inst_Type type);
int inst_by_name(String_View name, Inst_Type *output);

int inst_by_name(String_View name, Inst_Type *output)
{
    for (Inst_Type type = (Inst_Type) 0; type < NUMBER_OF_INSTS; type += 1) {
        if (sv_eq(cstr_as_sv(inst_name(type)), name)) {
            *output = type;
            return 1;
        }
    }

    return 0;
}

const char *inst_name(Inst_Type type)
{
    switch (type) {
    case INST_NOP:         return "nop";
    case INST_PUSH:        return "push";
    case INST_DROP:        return "drop";
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
    case INST_RET:         return "ret";
    case INST_CALL:        return "call";
    case INST_NATIVE:      return "native";
    case NUMBER_OF_INSTS:
    default: assert(0 && "inst_name: unreachable");
    }
}

int inst_has_operand(Inst_Type type)
{
    switch (type) {
    case INST_NOP:         return 0;
    case INST_DROP:        return 0;
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
    case INST_RET:         return 0;
    case INST_CALL:        return 1;
    case INST_NATIVE:      return 1;
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

typedef struct {
  Inst_Type type;
  Word operand;
} Inst;

typedef struct LVM LVM;

typedef Err (*LVM_Native)(LVM*);

struct LVM {
    Word stack[LVM_STACK_CAPACITY];
    uint64_t stack_size;
    
    Inst program[LVM_PROGRAM_CAPACITY];
    uint64_t program_size;
    Inst_Addr pc;

    LVM_Native natives[LVM_NATIVES_CAPACITY];
    size_t natives_size;
    
    int halt;
};


Err lvm_execute_inst(LVM* lvm);
Err lvm_execute_program(LVM *lvm, int limit);
void lvm_dump_stack(FILE* stream, const LVM* lvm);

void lvm_push_native(LVM* lvm, LVM_Native native);
void lvm_load_program_from_memory(LVM* lvm, Inst * program,size_t program_size);
void lvm_load_program_from_file(LVM* lvm, const char* file_path);
void lvm_save_program_to_file(const LVM* lvm, const char* file_path);

void lvm_push_native(LVM* lvm, LVM_Native native) {
  assert(lvm->natives_size < LVM_NATIVES_CAPACITY);
  lvm->natives[lvm->natives_size++] = native;
}

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
    if (lvm->stack_size >= LVM_STACK_CAPACITY) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack[lvm->stack_size++]= inst.operand;
    lvm->pc += 1;
    break;
  case INST_DROP:
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    lvm->stack_size -= 1;
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
  case INST_RET:
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->pc = lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 1;
    break;
  case INST_CALL:
    if (lvm->stack_size >= LVM_STACK_CAPACITY) {
      return ERR_STACK_OVERFLOW;
    }

    lvm->stack[lvm->stack_size++].as_u64 = lvm->pc + 1;
    lvm->pc = inst.operand.as_u64;
    break;
  case INST_NATIVE:
    if (inst.operand.as_u64 > lvm->natives_size) {
      return ERR_ILLEGAL_OPERAND;
    }
    const Err err = lvm->natives[inst.operand.as_u64](lvm);
    if (err != ERR_OK) {
      return err;
    }
    lvm->pc += 1;
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
    fprintf(stdout, "  u64: %" PRIu64 ", i64: %" PRId64 ", f64: %lf, ptr: %p\n",
            lvm->stack[lvm->stack_size - 1].as_u64,
            lvm->stack[lvm->stack_size - 1].as_i64,
            lvm->stack[lvm->stack_size - 1].as_f64,
            lvm->stack[lvm->stack_size - 1].as_ptr);
    lvm->stack_size -=1;
    lvm->pc +=1;
    break;
  case INST_DUP:
    if (lvm->stack_size >= LVM_STACK_CAPACITY) {
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
      fprintf(stream, "  u64: %" PRIu64 ", i64: %" PRId64 ", f64: %lf, ptr: %p\n",
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
    fprintf(stderr, "ERROR: Cound not open file %s : %s\n",
	    file_path,strerror(errno));
    exit(1);
  }

  //seek to end
  if (fseek(f, 0, SEEK_END) < 0) {
    fprintf(stderr, "ERROR: Cound not set position at end of  file %s : %s\n",
	    file_path,strerror(errno));
    exit(1);
  }

  long m = ftell(f);
  if (m<0) {
    fprintf(stderr, "ERROR: Cound not determine length of file %s : %s\n",
	    file_path,strerror(errno));
    exit(1);    
  }

  assert(m % sizeof(lvm->program[0]) == 0);
  assert((size_t) m <= LVM_PROGRAM_CAPACITY * sizeof(lvm->program[0]));

  if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Could not rewind file %s : %s\n",
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


typedef struct {
  String_View name;
  Word word;
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

int number_literal_as_word(String_View sv, Word *output);

int  lasm_resolve_label(const Lasm *lt, String_View name,Word *output);
int  lasm_bind_label(Lasm *lt, String_View name, Word word);
void label_table_push_defered_operand(Lasm *lt, Inst_Addr addr, String_View label);

void lvm_translate_source(LVM *lvm, Lasm *lt, String_View input_file_path, size_t level);

int number_literal_as_word(String_View sv, Word *output)
{
    assert(sv.count < NUMBER_LITERAL_CAPACITY);
    char cstr[NUMBER_LITERAL_CAPACITY + 1];
    char *endptr = 0;

    memcpy(cstr, sv.data, sv.count);
    cstr[sv.count] = '\0';

    Word result = {0};

    result.as_u64 = strtoull(cstr, &endptr, 10);
    if ((size_t) (endptr - cstr) != sv.count) {
        result.as_f64 = strtod(cstr, &endptr);
        if ((size_t) (endptr - cstr) != sv.count) {
	  return 0;
        }
    }

    *output = result;
    return 1;
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

void lvm_translate_source(LVM *lvm, Lasm *lt,
			  String_View input_file_path, size_t level){
  String_View original_source = slurp_file(input_file_path);
  String_View source = original_source;
  
  lvm->program_size = 0;
  int line_number = 0;
  
  while (source.count > 0) {
    assert(lvm->program_size < LVM_PROGRAM_CAPACITY);
    String_View line = sv_trim(sv_chop_by_delim(&source, '\n'));
    line_number += 1;
    if (line.count > 0 && *line.data != LASM_COMMENT_SYMBOL) {
      String_View token = sv_trim(sv_chop_by_delim(&line, ' '));
      // Pre-processor
      if (token.count > 0 && *token.data == LASM_PP_SYMBOL) {
	token.count -= 1;
        token.data  += 1;
        if (sv_eq(token, cstr_as_sv("label"))) {
          line = sv_trim(line);
          String_View label = sv_chop_by_delim(&line, ' ');
          if (label.count > 0) {
            line = sv_trim(line);
            String_View value = sv_chop_by_delim(&line, ' ');
            Word word = {0};
            if (!number_literal_as_word(value, &word)) {
              fprintf(stderr,
		      "%.*s:%d: ERROR: `%.*s` is not a number",
                      SV_FORMAT(input_file_path),
                      line_number,
                      SV_FORMAT(value));

              exit(1);
            }


	    if (!lasm_bind_label(lt, label, word)) {
              // TODO: label redefinition error does not tell where the first label was already defined
              fprintf(stderr,
		      "%.*s:%d: ERROR: label `%.*s` is already defined\n",
                      SV_FORMAT(input_file_path),
                      line_number,
                      SV_FORMAT(label));

              exit(1);
            }
          } else {
            fprintf(stderr,
                    "%.*s:%d: ERROR: label name is not provided\n",
                    SV_FORMAT(input_file_path), line_number);
            exit(1);
          }
        }  else if (sv_eq(token, cstr_as_sv("include"))) {
          line = sv_trim(line);

          if (line.count > 0) {
            if (*line.data == '"' && line.data[line.count - 1] == '"') {
              line.data  += 1;
              line.count -= 2;

              if (level + 1 >= LASM_MAX_INCLUDE_LEVEL) {
                fprintf(stderr,
                        "%.*s:%d: ERROR: exceeded maximum include level\n",
                        SV_FORMAT(input_file_path), line_number);
                exit(1);
              }

              lvm_translate_source(lvm, lt, line, level + 1);
            } else {
              fprintf(stderr,
                      "%.*s:%d: ERROR: include file path has to be surrounded with quotation marks\n",
                      SV_FORMAT(input_file_path), line_number);
              exit(1);
            }
          } else {
            fprintf(stderr,
                    "%.*s:%d: ERROR: include file path is not provided\n",
                    SV_FORMAT(input_file_path), line_number);
            exit(1);
          }
        }else {
          fprintf(stderr,
		  "%.*s:%d: ERROR: unknown pre-processor directive `%.*s`\n",
                  SV_FORMAT(input_file_path),
                  line_number,
                  SV_FORMAT(token));

          exit(1);

        }
      } else {
      
	if (token.count > 0 && token.data[token.count - 1] == ':') {
          String_View label = {
            .count = token.count - 1,
            .data = token.data
          };

	  if (!lasm_bind_label(lt, label, (Word) {.as_u64 = lvm->program_size})) {
            fprintf(stderr,
		    "%.*s:%d: ERROR: label `%.*s` is already defined\n",
                    SV_FORMAT(input_file_path),
                    line_number,
                    SV_FORMAT(label));

            exit(1);
          }
	  token = sv_trim(sv_chop_by_delim(&line, ' '));
	}
	if (token.count > 0) {
	  // 处理 # 和 inst 在同一行，且 #在尾部的情况
	  String_View operand = sv_trim(sv_chop_by_delim(&line, LASM_COMMENT_SYMBOL));
	  Inst_Type inst_type = INST_NOP;
	  if (inst_by_name(token, &inst_type)) {
            lvm->program[lvm->program_size].type = inst_type;

            if (inst_has_operand(inst_type)) {
	      if (operand.count == 0) {
		fprintf(stderr, "%.*s:%d: ERROR: instruction `%.*s` requires an operand\n",
                        SV_FORMAT(input_file_path),
                        line_number,
			SV_FORMAT(token));

		exit(1);
	      }
	      //不能转成word， 则是 jmp/call 指令中的 label
              if (!number_literal_as_word(operand,
					  &lvm->program[lvm->program_size].operand)) {
		label_table_push_defered_operand(lt, lvm->program_size, operand);
              }
	    }
	    lvm->program_size += 1;
	  } else {
	    fprintf(stderr, "%.*s:%d: ERROR: unknown instruction `%.*s`\n",
                    SV_FORMAT(input_file_path),
                    line_number,
                    SV_FORMAT(token));

            exit(1);
	  }
	}
      }
    }
  }

  for (size_t i = 0; i < lt->defered_operands_size;i++) {
    String_View label = lt->defered_operands[i].label;
    //inst 从0开始， 替换jmp指令的地址为解析label的inst地址
    if (!lasm_resolve_label(
			    lt,
			    label,
			    &lvm->program[lt->defered_operands[i].addr].operand)) {
      // TODO: second pass label resolution errors don't report the location in the source code
      fprintf(stderr, "%.*s: ERROR: unknown label `%.*s`\n",
	      SV_FORMAT(input_file_path), SV_FORMAT(label));
      exit(1);
    }
  }
   free((void*) original_source.data);
}

String_View slurp_file(String_View file_path)
{
  char *file_path_cstr = malloc(file_path.count + 1);
  if (file_path_cstr == NULL) {
    fprintf(stderr,
            "ERROR: Could not allocate memory for the file path `%.*s`: %s\n",
            SV_FORMAT(file_path), strerror(errno));
    exit(1);
  }

  memcpy(file_path_cstr, file_path.data, file_path.count);
  file_path_cstr[file_path.count] = '\0';

  FILE *f = fopen(file_path_cstr, "r");
  if (f == NULL) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path_cstr, strerror(errno));
    exit(1);
  }

  if (fseek(f, 0, SEEK_END) < 0) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path_cstr, strerror(errno));
    exit(1);
  }

  long m = ftell(f);
  if (m < 0) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path_cstr, strerror(errno));
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
            file_path_cstr, strerror(errno));
    exit(1);
  }

  size_t n = fread(buffer, 1, m, f);
  if (ferror(f)) {
    fprintf(stderr, "ERROR: Could not read file `%s`: %s\n",
            file_path_cstr, strerror(errno));
    exit(1);
  }

  fclose(f);
  free(file_path_cstr);

  return (String_View) {
    .count = n,
    .data = buffer,
  };
}

int lasm_resolve_label(const Lasm *lt, String_View name, Word *output)
{
  for (size_t i = 0; i < lt->labels_size; ++i) {
    if (sv_eq(lt->labels[i].name, name)) {
      *output = lt->labels[i].word;
      return 1;
    }
  }

  return 0;
}

int lasm_bind_label(Lasm *lt, String_View name, Word word)
{
  assert(lt->labels_size < LABEL_CAPACITY);
  Word ignore = {0};
  if (lasm_resolve_label(lt, name, &ignore)) {
    return 0;
  }

  lt->labels[lt->labels_size++] = (Label) {.name = name, .word = word};
  return 1;
}

void label_table_push_defered_operand(Lasm *lt, Inst_Addr addr, String_View label)
{
    assert(lt->defered_operands_size < DEFERED_OPERANDS_CAPACITY);
    lt->defered_operands[lt->defered_operands_size++] =
        (Defered_Operand) {.addr = addr, .label = label};
}

#endif
