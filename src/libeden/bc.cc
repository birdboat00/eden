#include "eden.hh"

#include <sstream>

namespace edn::bc::ops {
  auto move::to_str() const -> str {
    std::stringstream s;
    s << "move r" << static_cast<i32>(dest) << " <- r" << static_cast<i32>(src);
    return s.str();
  }

  auto ldc::to_str() const -> str {
    std::stringstream s;
    s << "ldc r" << static_cast<i32>(dest) << " <- const$" << idx;
    return s.str();
  }

  auto call::to_str() const -> str {
    std::stringstream s;
    s << "call fn$" << idx;
    return s.str();
  }

  auto ret::to_str() const -> str {
    return "ret";
  }

  auto nifcallnamed::to_str() const -> str {
    std::stringstream s;
    s << "nifcallnamed <" << static_cast<i32>(arity) << "> const$" << nameidx << " args[";
    for (const auto& arg : args) { s << "(" << arg << ")"; }
    s << "]";
    return s.str();
  }

  auto test::to_str() const -> str {
    std::stringstream s;
    s << "test <" << static_cast<i32>(fn) << "> @" << dest << " r" << static_cast<i32>(reg);
    return s.str();
  }

  auto cmp::to_str() const -> str {
    std::stringstream s;
    s << "cmp <" << static_cast<i32>(fn) << "> @" << dest << " r" << static_cast<i32>(rl) << " ? r" << static_cast<i32>(rr);
    return s.str();
  }

  auto jump::to_str() const -> str {
    std::stringstream s;
    s << "jump @" << dest;
    return s.str();
  }

  auto label::to_str() const -> str {
    std::stringstream s;
    s << "@" << name;
    return s.str();
  }
}

namespace edn::bc {
  str opcode_to_str(const opcode opcode) {
    switch (opcode) {
      case opcode::move: return "move";
      case opcode::ldc: return "ldc";
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
    
    unreachable();
  }

  usize opcode_arity(const opcode opcode, bc_t next) {
    switch (opcode) {
      case opcode::move: case opcode::ldc: return 2;
      case opcode::call: case opcode::nifcallnamed: return 1 + static_cast<edn::usize>(next);
      case opcode::test_isint: case opcode::test_isflt: case opcode::test_isstr: case opcode::test_isfun: return 2;
      case opcode::cmp_islt: case opcode::cmp_isge: case opcode::cmp_iseq: case opcode::cmp_isne: return 3;
      case opcode::ret: return 0;
      case opcode::jump: return 1;
      case opcode::label: return 1;
      default: return 0;
    }
  
    unreachable();
  }
}