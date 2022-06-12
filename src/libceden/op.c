#include "eden.h"

void edn_op_to_str(const edn_op_t op, str buffer, usize buffersz) {
  switch (op.opcode) {
    case omove: snprintf(buffer, buffersz, "mov r(%u) <- r(%u)", op.arg1, op.arg2); break;
    case oint: snprintf(buffer, buffersz, "int r(%u) <- i(int, %u)", op.arg1, op.arg2); break;
    case oflt: snprintf(buffer, buffersz, "flt r(%u) <- i(flt, %u)", op.arg1, op.arg2); break;
    case ostr: snprintf(buffer, buffersz, "str r(%u) <- i(str, %u)", op.arg1, op.arg2); break;
    case ocall: snprintf(buffer, buffersz, "call (%u) !i(fn, %u)", op.arg1, op.arg2); break; // TODO: print arguments
    case obifcall: snprintf(buffer, buffersz, "bifcall (%u) $%u", op.arg1, op.arg2); break; // TODO: print arguments
    case oret: snprintf(buffer, buffersz, "ret"); break;
    default: snprintf(buffer, buffersz, "unhandled opcode! %u", op.opcode); break;
  }
}

const str edn_opcode_to_str(const edn_opcode_t op) {
  switch (op)
  {
    case omove: return "mov";
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

u32 edn_opcode_arity(const edn_opcode_t op, const edn_bytecode_t next) {
  switch (op) {
    case oadd: case osub: case omul: case odiv: return 3;
    case omove: case oint: case oflt: case ostr: case oneg: return 2;
    case ocall: case obifcall: return next + 1;
    case oret: return 1;
    default: return 0;
  }
}