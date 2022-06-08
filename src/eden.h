#ifndef EDEN_H
#define EDEN_H 1

#include <stdint.h>
#include <stdio.h>

#define EDEN_VERSION "22w22a"
#define EDEN_BYTECODE_VERSION 0x0001
#define EDEN_BUILD_TIME __TIME__ " on " __DATE__

#define EDEN_PACK_MAGIC "eDeNPACK"
#define EDEN_PACK_MAGIC_LEN 8

typedef uint8_t bool;
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef size_t usize;
typedef float f32;
typedef double f64;
typedef char* str;

#define arraylen(type, array) (sizeof(array) / sizeof(type))
#define isnull(x) ((x) == NULL)

#define bit_set(val, idx) (val |= 1UL << idx)
#define bit_clear(val, idx) (val &= ~(1UL << idx))
#define bit_check(val, idx) ((val >> idx) & 1UL)

typedef enum edn_opcode {
  omov,
  oint,
  oflt,
  ostr,
  oadd, osub, omul, odiv,
  oneg,
  ocall, obifcall, oret
} edn_opcode_t;
typedef i32 edn_bytecode_t;
const str edn_opcode_to_str(const edn_opcode_t op);
u32 edn_op_arity(const edn_opcode_t op, const edn_bytecode_t next);

typedef struct edn_op {
  edn_opcode_t opcode;
  i32 arg1;
  i32 arg2;
  i32 arg3;
} edn_op_t;

typedef struct edn_function {
  edn_bytecode_t* bytecode;
  usize bytecodelen;
} edn_function_t;

#define DEF_TABLE(type, name) type* name; u32 name##len;

typedef struct edn_pack {
  str name;
  u16 target_version;
  u32 entryfuncid;

  DEF_TABLE(i32, integers);
  DEF_TABLE(f64, floats);
  DEF_TABLE(str, strings);
  DEF_TABLE(edn_function_t, functions);
} edn_pack_t;

typedef enum edn_reg_type {
  integer,
  floating,
  string
} edn_reg_type_t;

typedef struct edn_reg {
  union {
    i32 i;
    f64 f;
    str s;
  } data;
  edn_reg_type_t type;
} edn_reg_t;

typedef struct edn_vm_params {
  bool verbose;
} edn_vm_params_t;

typedef struct edn_callstack_entry {
  u32 functionid;
  usize ip;
} edn_callstack_entry_t;

typedef struct edn_vm {
  edn_vm_params_t params;
  edn_pack_t* pack;

  u16 callstack_top;
  edn_callstack_entry_t call_stack[65536];
  
  edn_reg_t registers[64];
} edn_vm_t;

typedef enum edn_error {
  kErrNone = 0,
  kErrInvalidPack = 1,
  kErrInvalidFile = 2,
  kErrMallocFail = 3,
} edn_error_t;

typedef struct edn_process_ctx {
  edn_reg_t regs[64];
} edn_process_ctx_t;

typedef struct edn_process {
  edn_process_ctx_t ctx;
} edn_process_t;

edn_vm_t edn_make_vm(const edn_pack_t* pack, const edn_vm_params_t params);
edn_error_t edn_run_vm(edn_vm_t* vm);

edn_error_t edn_write_pack(FILE* outfile, const edn_pack_t* pack);
edn_error_t edn_read_pack(FILE* infile, edn_pack_t* out_pack);
void edn_dump_pack(FILE* outfile, const edn_pack_t* pack);

#endif