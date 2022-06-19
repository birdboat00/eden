#include "eden.hh"

#include <iostream>

#include <optional>

namespace edn::bc::check {
  bool check(const vm::vm& vm, const pack::pack& pack) {
    for (usize f = 0; f < pack.fns.size(); f++) {
      const auto& fn = pack.fns.at(f);
      const auto bclen = fn.bytecode.size();
      for (usize i = 0; i < bclen; i++) {
        const auto opc = static_cast<opcode>(fn.bytecode.at(i));
        std::optional<bc_t> arg0, arg1, arg2, arg3;
        if (i + 1 < bclen) arg0 = fn.bytecode.at(i + 1);
        if (i + 2 < bclen) arg1 = fn.bytecode.at(i + 2);
        if (i + 3 < bclen) arg2 = fn.bytecode.at(i + 3);
        if (i + 4 < bclen) arg3 = fn.bytecode.at(i + 4);

        switch (opc) {
          case opcode::move: {
            if (!arg0.has_value() || !arg1.has_value()) {
              std::cout << "check: move -> missing dest or src register" << std::endl;
              return false;
            }
          } break;
          case opcode::lint: {
            if (!arg0.has_value() || !arg1.has_value()) {
              std::cout << "check: lint -> missing register or table index" << std::endl;
              return false;
            }
            if (arg1.value() > pack.ints.size()) {
              std::cout << "check: lint -> table index bigger than table length" << std::endl;
              return false;
            }
          } break;
          case opcode::lflt: {
            if (!arg0.has_value() || !arg1.has_value()) {
              std::cout << "check: lflt -> missing register or table index" << std::endl;
              return false;
            }
            if (arg1.value() > pack.flts.size()) {
              std::cout << "check: lflt -> table index bigger than table length" << std::endl;
              return false;
            }
          } break;
          case opcode::lstr: {
            if (!arg0.has_value() || !arg1.has_value()) {
              std::cout << "check: lstr -> missing register or table index" << std::endl;
              return false;
            }
            if (arg1.value() > pack.strs.size()) {
              std::cout << "check: lstr -> table index bigger than table length" << std::endl;
              return false;
            }
          } break;
          case opcode::call: case opcode::tailcall: {
            if (!arg0.has_value()) {
              std::cout << "check: call/tailcall -> missing opcode arity" << std::endl;
              return false;
            }
            if (!arg1.has_value()) {
              std::cout << "check: call/tailcall -> missing function index" << std::endl;
              return false;
            }
            if (arg1.value() > pack.fns.size()) {
              std::cout << "check: call/tailcall -> table index bigger than table length" << std::endl;
              return false;
            }
          } break;
          case opcode::nifcallnamed: {
            if (!arg0.has_value()) {
              std::cout << "check: nifcallnamed -> missing opcode arity" << std::endl;
              return false;
            }
            if (!arg1.has_value()) {
              std::cout << "check: nifcallnamed -> missing nif name index" << std::endl;
              return false;
            }
            if (arg1.value() > pack.strs.size()) {
              std::cout << "check: nifcallnamed -> nif name index bigger than strings table length" << std::endl;
              return false;
            }
          } break;
          case opcode::test_isint: case opcode::test_isflt: case opcode::test_isstr: {
            if (!arg0.has_value()) {
              std::cout << "check: test_* -> missing destination label" << std::endl;
              return false;
            }
            if (!arg1.has_value()) {
              std::cout << "check: test_* -> missing test register" << std::endl;
              return false;
            }
            if (!fn.labels.contains(arg0.value())) {
              std::cout << "check: test_* -> function does not have label " << arg0.value() << std::endl;
              return false;
            }
          } break;
          case opcode::cmp_islt: case opcode::cmp_isge: case opcode::cmp_iseq: case opcode::cmp_isne: {
            if (!arg0.has_value()) {
              std::cout << "check: cmp_* -> missing destination label" << std::endl;
              return false;
            }
            if (!arg1.has_value()) {
              std::cout << "check: cmp_* -> missing comparision lhs register" << std::endl;
              return false;
            }
            if (!arg2.has_value()) {
              std::cout << "check: cmp_* -> missing comparision rhs register" << std::endl;
              return false;
            }
            if (!fn.labels.contains(arg0.value())) {
              std::cout << "check: cmp_* -> function does not have label " << arg0.value() << std::endl;
              return false;
            }
          } break;
          case opcode::ret: case opcode::label: break;
          default: {
            std::cout << "unknown opcode " << opcode_to_str(opc) << std::endl;
            return false;
          }
        }

        i += opcode_arity(opc, arg0.has_value() ? arg0.value() : 1);
      }
    }

    return true;
  }
}