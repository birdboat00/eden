#include "eden.hh"

#include <sstream>

namespace edn::bc {
  str opcode_to_str(const opcode opcode) {
    switch (opcode) {
      case opcode::move: return "move";
      case opcode::lint: return "lint";
      case opcode::lflt: return "lflt";
      case opcode::lstr: return "lstr";
      case opcode::add: return "add";
      case opcode::sub: return "sub";
      case opcode::mul: return "mul";
      case opcode::div: return "div";
      case opcode::neg: return "neg";
      case opcode::call: return "call";
      case opcode::bifcall: return "bifcall";
      case opcode::ret: return "ret";
      default: return "unknown-opcode";
    }
    // unreachable
  }

  usize opcode_arity(const opcode opcode, bc_t next) {
    switch (opcode) {
      case opcode::add: case opcode::sub: case opcode::mul: case opcode::div: return 3;
      case opcode::move: case opcode::lint: case opcode::lflt: case opcode::lstr: return 2;
      case opcode::call: case opcode::bifcall: return 1 + next;
      case opcode::ret: 1;
      default: 0;
    }
    // unreachable
  }

  str op_to_str(const op& op) {
    std::stringstream stream;
    switch (op.opcode) {
      case opcode::move: stream << "move r(" << op.args[0] << ") <- r(" << op.args[1] << ")"; break;
      case opcode::lint: stream << "lint r(" << op.args[0] << ") <- t(int, " << op.args[1] << ")"; break; 
      case opcode::lflt: stream << "lflt r(" << op.args[0] << ") <- t(flt, " << op.args[1] << ")"; break;
      case opcode::lstr: stream << "lstr r(" << op.args[0] << ") <- t(str, " << op.args[1] << ")"; break;
      case opcode::call: stream << "call<" << op.args[0] << "> t(fns, " << op.args[1] << ")"; break;
      case opcode::bifcall: stream << "bifcall<" << op.args[0] << "> $" << op.args[1]; break;
      case opcode::ret: stream << "ret"; break;
      default: stream << "<unhandled opcode (" << static_cast<i32>(op.opcode) << ")>"; break;
    }

    return stream.str();
  }
}