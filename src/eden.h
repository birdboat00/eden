#ifndef EDEN_H
#define EDEN_H 1

#include <stdint.h>
#include <stdio.h>

#define EDEN_VERSION "22w23a"
#define EDEN_BYTECODE_VERSION (u16)0x0001
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

// Module: error
typedef enum edn_err_kind {
  kErrNone = 0,
  kErrInvalidPack,
  kErrInvalidFile,
  kErrMallocFail,
  kErrTermNotPrintable,
} edn_err_kind_t;
typedef enum edn_err_module {
  kErrModBif,
  kErrModBTP,
  kErrModErr,
  kErrModMain,
  kErrModOp,
  kErrModPack,
  kErrModTerm,
  kErrModVm
} edn_err_module_t;
typedef struct edn_err {
  edn_err_kind_t kind;
  edn_err_module_t module;
} edn_err_t;
edn_err_t edn_make_err(edn_err_module_t mod, edn_err_kind_t kind);
str edn_err_to_str(const edn_err_t* err);
bool edn_err_is_ok(const edn_err_t err);

// Module: term
typedef enum edn_term_kind {
  kTermKindIntnum,
  kTermKindFloatnum,
  kTermKindString
} edn_term_kind_t;
typedef struct edn_term {
  union {
    i32 i;
    f64 f;
    str s;
  } val;
  edn_term_kind_t type;
} edn_term_t;
edn_err_t edn_term_to_str(const edn_term_t* term, str buf, usize buflen);
edn_term_t edn_term_floatnum_to_intnum(const edn_term_t* term);
edn_term_t edn_term_intnum_to_floatnum(const edn_term_t* term);
edn_term_t edn_term_from_f64(f64 val);
edn_term_t edn_term_from_i32(i32 val);
edn_term_t edn_term_from_str(str val);
bool edn_term_is_floatnum(const edn_term_t* term);
f64 edn_term_get_floatnum(const edn_term_t* term);
bool edn_term_is_intnum(const edn_term_t* term);
i32 edn_term_get_intnum(const edn_term_t* term);
bool edn_term_is_string(const edn_term_t* term);
str edn_term_get_string(const edn_term_t* term);

// Module: op
typedef enum edn_opcode {
  omove,
  oint, oflt, ostr,
  oadd, osub, omul, odiv,
  oneg,
  ocall, obifcall, oret,
  opcode_count
} edn_opcode_t;
typedef i32 edn_bytecode_t;
typedef struct edn_op {
  edn_opcode_t opcode;
  i32 arg1;
  i32 arg2;
  i32 arg3;
  i32 arg4;
  i32 arg5;
} edn_op_t;
const str edn_opcode_to_str(const edn_opcode_t op);
u32 edn_op_arity(const edn_opcode_t op, const edn_bytecode_t next);
void edn_op_to_str(const edn_op_t op, str buffer, usize buffersz);

// Module: pack
typedef struct edn_function {
  edn_bytecode_t* bytecode;
  usize bytecodelen;
} edn_function_t;
typedef struct edn_pack {
  str name;
  u16 target_version;
  u32 entryfuncid;

  i32* integers;
  u32 integerslen;
  f64* floats;
  u32 floatslen;
  str* strings;
  u32 stringslen;
  edn_function_t* functions;
  u32 functionslen;
} edn_pack_t;
edn_err_t edn_write_pack(FILE* outfile, const edn_pack_t* pack);
edn_err_t edn_read_pack(FILE* infile, edn_pack_t* out_pack);
void edn_dump_pack(FILE* outfile, const edn_pack_t* pack);

// Module: vm
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
#define EDEN_VM_CALLSTACK_SIZE 65536
typedef struct edn_vm {
  edn_vm_params_t params;
  edn_pack_t* pack;

  u16 callstack_top;
  edn_callstack_entry_t callstack[EDEN_VM_CALLSTACK_SIZE];
  
  edn_term_t registers[64];
} edn_vm_t;
typedef struct edn_process_ctx {
  edn_reg_t regs[64];
} edn_process_ctx_t;
typedef struct edn_process {
  edn_process_ctx_t ctx;
} edn_process_t;
edn_vm_t* edn_make_vm(edn_pack_t* pack, const edn_vm_params_t params);
edn_err_t edn_run_vm(edn_vm_t* vm);

// Module: bif
edn_err_t edn_bif_dispatch_bif(edn_vm_t* vm, u32 bifid, const edn_op_t* op, edn_term_t* result);

#endif