#ifndef LVM_H
#define LVM_H
#include <ctype.h>
#include <stdbool.h>
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

#define LVM_STACK_CAPACITY 1024
#define LVM_NATIVES_CAPACITY 1024
#define LVM_PROGRAM_CAPACITY 1024
#define LVM_EXECUTION_LIMIT 128
#define LVM_MEMORY_CAPACITY (640 * 1000)
#define LASM_LABEL_CAPACITY 1024
#define LASM_DEFERED_OPERANDS_CAPACITY 1024
#define LASM_NUMBER_LITERAL_CAPACITY 1024
#define LASM_MEMORY_CAPACITY (1000 * 1000 * 1000)


#define LASM_COMMENT_SYMBOL ';'
#define LASM_PP_SYMBOL '%'
#define LASM_MAX_INCLUDE_LEVEL 64

typedef uint64_t Inst_Addr;
typedef uint64_t Memory_Addr;

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
  ERR_ILLEGAL_MEMORY_ACCESS,
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
  INST_ANDB,
  INST_ORB,
  INST_XOR,
  INST_SHR,
  INST_SHL,
  INST_NOTB,
  INST_READ8,
  INST_READ16,
  INST_READ32,
  INST_READ64,
  INST_WRITE8,
  INST_WRITE16,
  INST_WRITE32,
  INST_WRITE64,
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
bool sv_eq(String_View a, String_View b);

const char *inst_name(Inst_Type type);
bool inst_has_operand(Inst_Type type);
bool inst_by_name(String_View name, Inst_Type *output);

bool inst_by_name(String_View name, Inst_Type *output)
{
    for (Inst_Type type = (Inst_Type) 0; type < NUMBER_OF_INSTS; type += 1) {
        if (sv_eq(cstr_as_sv(inst_name(type)), name)) {
            *output = type;
            return true;
        }
    }

    return false;
}

const char *inst_name(Inst_Type type)
{
    switch (type) {
    case INST_NOP:		return "nop";
    case INST_PUSH:		return "push";
    case INST_DROP:		return "drop";
    case INST_DUP:		return "dup";
    case INST_PLUSI:		return "plusi";
    case INST_MINUSI:		return "minusi";
    case INST_MULTI:		return "multi";
    case INST_DIVI:		return "divi";
    case INST_PLUSF:		return "plusf";
    case INST_MINUSF:		return "minusf";
    case INST_MULTF:		return "multf";
    case INST_DIVF:		return "divf";
    case INST_JMP:		return "jmp";
    case INST_JMP_IF:		return "jmp_if";
    case INST_EQ:		return "eq";
    case INST_HALT:		return "halt";
    case INST_PRINT_DEBUG:	return "print_debug";
    case INST_SWAP:		return "swap";
    case INST_NOT:		return "not";
    case INST_GEF:		return "gef";
    case INST_RET:		return "ret";
    case INST_CALL:		return "call";
    case INST_NATIVE:		return "native";
    case INST_ANDB:		return "andb";
    case INST_ORB:		return "orb";
    case INST_XOR:		return "xor";
    case INST_SHR:		return "shr";
    case INST_SHL:		return "shl";
    case INST_NOTB:		return "notb";
    case INST_READ8:		return "read8";
    case INST_READ16:		return "read16";
    case INST_READ32:		return "read32";
    case INST_READ64:		return "read64";
    case INST_WRITE8:		return "write8";
    case INST_WRITE16:		return "write16";
    case INST_WRITE32:		return "write32";
    case INST_WRITE64:		return "write64";
    case NUMBER_OF_INSTS:
    default: assert(false && "inst_name: unreachable");
    }
}

bool inst_has_operand(Inst_Type type)
{
  switch (type) {
    case INST_NOP:	return false;
    case INST_PUSH:	return true;
    case INST_DROP:	return false;
    case INST_DUP:	return true;
    case INST_PLUSI:	return false;
    case INST_MINUSI:	return false;
    case INST_MULTI:	return false;
    case INST_DIVI:	return false;
    case INST_PLUSF:	return false;
    case INST_MINUSF:	return false;
    case INST_MULTF:	return false;
    case INST_DIVF:	return false;
    case INST_JMP:	return true;
    case INST_JMP_IF:	return true;
    case INST_EQ:	return false;
    case INST_HALT:	return false;
    case INST_SWAP:	return true;
    case INST_NOT:	return false;
    case INST_GEF:	return false;
    case INST_RET:	return false;
    case INST_CALL:	return true;
    case INST_NATIVE:	return true;
    case INST_ANDB:	return false;
    case INST_ORB:	return false;
    case INST_XOR:	return false;
    case INST_SHR:	return false;
    case INST_SHL:	return false;
    case INST_NOTB:	return false;
    case INST_READ8:	return false;
    case INST_READ16:	return false;
    case INST_READ32:	return false;
    case INST_READ64:	return false;
    case INST_WRITE8:	return false;
    case INST_WRITE16:	return false;
    case INST_WRITE32:	return false;
    case INST_WRITE64:	return false;
    case INST_PRINT_DEBUG: return false;
    case NUMBER_OF_INSTS:
    default: assert(false && "inst_name: unreachable");
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
  case ERR_ILLEGAL_MEMORY_ACCESS:
    return "ERR_ILLEGAL_MEMORY_ACCESS";
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

    uint8_t memory[LVM_MEMORY_CAPACITY];

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
  case INST_ANDB:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_u64 = lvm->stack[lvm->stack_size - 2].as_u64 & lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 1;
    lvm->pc += 1;

    break;

  case INST_ORB:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_u64 = lvm->stack[lvm->stack_size - 2].as_u64 | lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;

  case INST_XOR:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_u64 = lvm->stack[lvm->stack_size - 2].as_u64 ^ lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;

  case INST_SHR:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_u64 = lvm->stack[lvm->stack_size - 2].as_u64 >> lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;

  case INST_SHL:
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 2].as_u64 = lvm->stack[lvm->stack_size - 2].as_u64 << lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 1;
    lvm->pc += 1;
    break;

  case INST_NOTB:
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }

    lvm->stack[lvm->stack_size - 1].as_u64 = ~lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->pc += 1;
    break;
  case INST_READ8: {
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    const Memory_Addr addr = lvm->stack[lvm->stack_size - 1].as_u64;
    if (addr >= LVM_MEMORY_CAPACITY) {
      return ERR_ILLEGAL_MEMORY_ACCESS;
    }
    lvm->stack[lvm->stack_size - 1].as_u64 = lvm->memory[addr];
    lvm->pc += 1;
  } break;

  case INST_READ16: {
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    const Memory_Addr addr = lvm->stack[lvm->stack_size - 1].as_u64;
    if (addr >= LVM_MEMORY_CAPACITY - 1) {
      return ERR_ILLEGAL_MEMORY_ACCESS;
    }
    lvm->stack[lvm->stack_size - 1].as_u64 = *(uint16_t*)&lvm->memory[addr];
    lvm->pc += 1;
  } break;

  case INST_READ32: {
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    const Memory_Addr addr = lvm->stack[lvm->stack_size - 1].as_u64;
    if (addr >= LVM_MEMORY_CAPACITY - 3) {
      return ERR_ILLEGAL_MEMORY_ACCESS;
    }
    lvm->stack[lvm->stack_size - 1].as_u64 = *(uint32_t*)&lvm->memory[addr];
    lvm->pc += 1;
  } break;

  case INST_READ64: {
    if (lvm->stack_size < 1) {
      return ERR_STACK_UNDERFLOW;
    }
    const Memory_Addr addr = lvm->stack[lvm->stack_size - 1].as_u64;
    if (addr >= LVM_MEMORY_CAPACITY - 7) {
      return ERR_ILLEGAL_MEMORY_ACCESS;
    }
    lvm->stack[lvm->stack_size - 1].as_u64 = *(uint64_t*)&lvm->memory[addr];
    lvm->pc += 1;
  } break;

  case INST_WRITE8: {
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    const Memory_Addr addr = lvm->stack[lvm->stack_size - 2].as_u64;
    if (addr >= LVM_MEMORY_CAPACITY) {
      return ERR_ILLEGAL_MEMORY_ACCESS;
    }
    lvm->memory[addr] = (uint8_t) lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 2;
    lvm->pc += 1;
  } break;

  case INST_WRITE16: {
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    const Memory_Addr addr = lvm->stack[lvm->stack_size - 2].as_u64;
    if (addr >= LVM_MEMORY_CAPACITY - 1) {
      return ERR_ILLEGAL_MEMORY_ACCESS;
    }
    *(uint16_t*)&lvm->memory[addr] = (uint16_t) lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 2;
    lvm->pc += 1;
  } break;

  case INST_WRITE32: {
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    const Memory_Addr addr = lvm->stack[lvm->stack_size - 2].as_u64;
    if (addr >= LVM_MEMORY_CAPACITY - 3) {
      return ERR_ILLEGAL_MEMORY_ACCESS;
    }
    *(uint32_t*)&lvm->memory[addr] = (uint32_t) lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 2;
    lvm->pc += 1;
  } break;

  case INST_WRITE64: {
    if (lvm->stack_size < 2) {
      return ERR_STACK_UNDERFLOW;
    }
    const Memory_Addr addr = lvm->stack[lvm->stack_size - 2].as_u64;
    if (addr >= LVM_MEMORY_CAPACITY - 7) {
      return ERR_ILLEGAL_MEMORY_ACCESS;
    }
    *(uint64_t*)&lvm->memory[addr] = lvm->stack[lvm->stack_size - 1].as_u64;
    lvm->stack_size -= 2;
    lvm->pc += 1;
  } break;

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

  assert((size_t) m % sizeof(lvm->program[0]) == 0);
  assert((size_t) m <= LVM_PROGRAM_CAPACITY * sizeof(lvm->program[0]));

  if (fseek(f, 0, SEEK_SET) < 0) {
        fprintf(stderr, "ERROR: Could not rewind file %s : %s\n",
                file_path, strerror(errno));
        exit(1);
    }

  lvm->program_size = fread(lvm->program, sizeof(lvm->program[0]), (size_t) m / sizeof(lvm->program[0]), f);

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
  Label labels[LASM_LABEL_CAPACITY];
  size_t labels_size;
  Defered_Operand defered_operands[LASM_DEFERED_OPERANDS_CAPACITY];
  size_t defered_operands_size;
  char memory[LASM_MEMORY_CAPACITY];
  size_t memory_size;
} Lasm;

String_View slurp_file(Lasm* lasm, String_View file_path);
void *lasm_alloc(Lasm *basm, size_t size);
bool lasm_number_literal_as_word(Lasm* lt, String_View sv, Word *output);

bool  lasm_resolve_label(const Lasm *lt, String_View name,Word *output);
bool  lasm_bind_label(Lasm *lt, String_View name, Word word);
void label_table_push_defered_operand(Lasm *lt, Inst_Addr addr, String_View label);

void lasm_translate_source(LVM *lvm, Lasm *lt, String_View input_file_path, size_t level);

void *lasm_alloc(Lasm *lasm, size_t size)
{
  assert(lasm->memory_size + size <= LASM_MEMORY_CAPACITY);
  void* result = lasm->memory + lasm->memory_size;
  lasm->memory_size += size;
  return result;
}

bool lasm_number_literal_as_word(Lasm* lt, String_View sv, Word *output)
{
    char *cstr = lasm_alloc(lt, sv.count + 1);

    memcpy(cstr, sv.data, sv.count);
    cstr[sv.count] = '\0';

    char *endptr = 0;
    Word result = {0};

    result.as_u64 = strtoull(cstr, &endptr, 10);
    if ((size_t) (endptr - cstr) != sv.count) {
        result.as_f64 = strtod(cstr, &endptr);
        if ((size_t) (endptr - cstr) != sv.count) {
	  return false;
        }
    }

    *output = result;
    return true;
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

bool sv_eq(String_View a, String_View b)
{
  if (a.count != b.count) {
    return false;
  } else {
    return memcmp(a.data, b.data, a.count) == 0;
  }
}


void lasm_translate_source(LVM *lvm, Lasm *lt,
			  String_View input_file_path, size_t level){
  String_View original_source = slurp_file(lt,input_file_path);
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
            if (!lasm_number_literal_as_word(lt,value, &word)) {
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

              lasm_translate_source(lvm, lt, line, level + 1);
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
              if (!lasm_number_literal_as_word(lt,operand,
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
 //  free((void*) original_source.data);
}
String_View slurp_file(Lasm* lasm, String_View file_path)
{
  char *file_path_cstr = lasm_alloc(lasm,file_path.count + 1);
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

  char *buffer = lasm_alloc(lasm,m);
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
//  free(file_path_cstr);

  return (String_View) {
    .count = n,
    .data = buffer,
  };
}

bool lasm_resolve_label(const Lasm *lt, String_View name, Word *output)
{
  for (size_t i = 0; i < lt->labels_size; ++i) {
    if (sv_eq(lt->labels[i].name, name)) {
      *output = lt->labels[i].word;
      return true;
    }
  }

  return false;
}

bool lasm_bind_label(Lasm *lt, String_View name, Word word)
{
  assert(lt->labels_size < LASM_LABEL_CAPACITY);
  Word ignore = {0};
  if (lasm_resolve_label(lt, name, &ignore)) {
    return false;
  }

  lt->labels[lt->labels_size++] = (Label) {.name = name, .word = word};
  return true;
}

void label_table_push_defered_operand(Lasm *lt, Inst_Addr addr, String_View label)
{
    assert(lt->defered_operands_size < LASM_DEFERED_OPERANDS_CAPACITY);
    lt->defered_operands[lt->defered_operands_size++] =
        (Defered_Operand) {.addr = addr, .label = label};
}

#endif
