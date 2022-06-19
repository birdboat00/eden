#include "eden.hh"

#include <sstream>

namespace edn::bc {
  str opcode_to_str(const opcode opcode) {
    switch (opcode) {
      case opcode::move: return "move";
      case opcode::lint: return "lint";
      case opcode::lflt: return "lflt";
      case opcode::lstr: return "lstr";
      case opcode::lfun: return "lfun";
      case opcode::call: return "call";
      case opcode::ret: return "ret";
      case opcode::nifcallnamed: return "nifcallnamed";
      case opcode::test_isint: return "test_isint";
      case opcode::test_isflt: return "test_isflt";
      case opcode::test_isstr: return "test_isstr";
      case opcode::test_isfun: return "test_isfun";
      case opcode::cmp_islt: return "cmp_islt";
      case opcode::cmp_isge: return "cmp_isge";
      case opcode::cmp_iseq: return "cmp_iseq";
      case opcode::cmp_isne: return "cmp_isne";
      case opcode::jump: return "jump";
      case opcode::label: return "label";
      default: return "unknown-opcode";
    }
    // unreachable
  }

  usize opcode_arity(const opcode opcode, bc_t next) {
    switch (opcode) {
      case opcode::move: case opcode::lint: case opcode::lflt: case opcode::lstr: case opcode::lfun: return 2;
      case opcode::call: case opcode::nifcallnamed: return 1 + next;
      case opcode::test_isint: case opcode::test_isflt: case opcode::test_isstr: case opcode::test_isfun: return 2;
      case opcode::cmp_islt: case opcode::cmp_isge: case opcode::cmp_iseq: case opcode::cmp_isne: return 3;
      case opcode::ret: return 0;
      case opcode::jump: return 1;
      case opcode::label: return 1;
      default: return 0;
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
      case opcode::lfun: stream << "lfun r(" << op.args.at(0) << ") <- t(fun, " << op.args.at(1) << ")"; break;
      case opcode::call: stream << "call<" << op.args[0] << "> t(fns, " << op.args[1] << ")"; break;
      case opcode::ret: stream << "ret"; break;
      case opcode::nifcallnamed: stream << "nifcallnamed<" << op.args[0] << "> t(str, " << op.args[2] << ")"; break;
      case opcode::test_isint: stream << "test_isint @" << op.args.at(0) << " r(" << op.args.at(1) << ")"; break;
      case opcode::test_isflt: stream << "test_isflt @" << op.args.at(0) << " r(" << op.args.at(1) << ")"; break;
      case opcode::test_isstr: stream << "test_isstr @" << op.args.at(0) << " r(" << op.args.at(1) << ")"; break;
      case opcode::test_isfun: stream << "test_isfun @" << op.args.at(0) << " r(" << op.args.at(1) << ")"; break;
      case opcode::cmp_islt: stream << "cmp_islt @" << op.args.at(0) << " r(" << op.args.at(1) << ") ? r(" << op.args.at(2) << ")"; break;
      case opcode::cmp_isge: stream << "cmp_isge @" << op.args.at(0) << " r(" << op.args.at(1) << ") ? r(" << op.args.at(2) << ")"; break;
      case opcode::cmp_iseq: stream << "cmp_iseq @" << op.args.at(0) << " r(" << op.args.at(1) << ") ? r(" << op.args.at(2) << ")"; break;
      case opcode::cmp_isne: stream << "cmp_isne @" << op.args.at(0) << " r(" << op.args.at(1) << ") ? r(" << op.args.at(2) << ")"; break;
      case opcode::jump: stream << "jump @" << op.args.at(0); break; 
      case opcode::label: stream << "label @" << op.args.at(0); break;
      default: stream << "<unhandled opcode (" << static_cast<i32>(op.opcode) << ")>"; break;
    }

    return stream.str();
  }
}