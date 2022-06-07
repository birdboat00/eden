#include "eden.h"

const str edn_opcode_to_str(const edn_opcode_t op) {
  switch (op)
  {
    case omov: return "mov";
    case oint: return "int";
    case oflt: return "flt";
    case ostr: return "str";
    case oadd: return "add";
    case osub: return "sub";
    case omul: return "mul";
    case odiv: return "div";
    case oneg: return "neg";
    case ocall: return "call";
    case obifcall: return "bifcall";
    case oret: return "ret";
    default: return "unknown-opcode";
  }
  // UNREACHABLE
}

u32 edn_op_arity(const edn_opcode_t op, const edn_bytecode_t next) {
  switch (op) {
    case oadd: case osub: case omul: case odiv: return 3;
    case omov: case oint: case oflt: case ostr: case oneg: return 2;
    case ocall: case obifcall: return next + 1;
    case oret: return 1;
    default: return 0;
  }
}