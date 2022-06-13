#include "eden.hh"

#include <functional>
#include <iostream>

namespace edn::vm {

  usize do_move(vm& vm, const bc::op& op) {
    vm.regs[op.args[0]] = vm.regs[op.args[1]];
    return bc::opcode_arity(op.opcode, op.args[0]) + 1;
  }

  usize do_lint(vm& vm, const bc::op& op) {
    vm.regs[op.args[0]] = term::from<i64>(vm.pack.ints.at(op.args[1]));
    return bc::opcode_arity(op.opcode, op.args[0]) + 1;
  }

  usize do_lflt(vm& vm, const bc::op& op) {
    vm.regs[op.args[0]] = term::from<f64>(vm.pack.flts.at(op.args[1]));
    return bc::opcode_arity(op.opcode, op.args[0]) + 1;
  }

  usize do_lstr(vm& vm, const bc::op& op) {
    vm.regs[op.args[0]] = term::from<str>(vm.pack.strs.at(op.args[1]));
    return bc::opcode_arity(op.opcode, op.args[0]) + 1;
  }

  usize do_call(vm& vm, const bc::op& op) {
    const auto arity = op.args[0];
    const auto fnid = op.args[1];

    vm.callstack.top().ip += 2 + arity;

    vm.callstack.push(callstackentry {
      .fnid = static_cast<u32>(fnid), .ip = 0
    });

    return 0;
  }

  usize do_tailcall(vm& vm, const bc::op& op) {
    const auto arity = op.args[0];
    const auto fnid = op.args[1];

    vm.callstack.top() = callstackentry {
      .fnid = static_cast<u32>(fnid), .ip = 0
    };

    return 0;
  }

  usize do_bifcall(vm& vm, const bc::op& op) {
    const auto arity = op.args[0];
    term::term result;
    const auto err = bif::dispatch(vm, op.args[1], op, vm.regs[0]);
    if (!err::is_ok(err)) throw std::runtime_error(err::to_str(err));
    return 2 + arity;
  }

  usize do_ret(vm& vm, const bc::op& op) {
    vm.callstack.pop();
    return 0;
  }

  usize op_unimplemented(vm& vm, const bc::op& op) {
    std::cout << "vm - op not implemented. (" << bc::op_to_str(op) << ")." <<  std::endl;
    return 0;
  }

  err::err run(vm& vm) {
    using ophandler = std::function<usize(struct vm&, const bc::op& op)>;

    static std::array<ophandler, static_cast<usize>(bc::opcode::opcodecount)> table = {
      &do_move, &do_lint, &do_lflt, &do_lstr,
      &op_unimplemented, &op_unimplemented, &op_unimplemented, &op_unimplemented, &op_unimplemented,
      &do_call, &do_tailcall, &do_bifcall, &do_ret
    };

    vm.callstack.push(callstackentry {
      .fnid = vm.pack.entryfn,
      .ip = 0
    });

    std::function<err::err(usize)> dispatch;
    dispatch = [&](usize adv) -> err::err {
      vm.callstack.top().ip += adv;
      if (vm.callstack.top().ip >= vm.pack.fns.at(vm.callstack.top().fnid).bytecode.size()) {
        std::cout << "bytecode end reached." << std::endl;
        return err::make_err_none(err::err_module::vm);
      }

      const auto top = vm.callstack.top();
      const auto opc = vm.pack.fns.at(top.fnid).bytecode.at(top.ip);
      const auto bclen = vm.pack.fns.at(top.fnid).bytecode.size();
      const auto op = bc::op {
        .opcode = static_cast<bc::opcode>(opc),
        .args = {
          (top.ip + 1 < bclen) ? vm.pack.fns.at(top.fnid).bytecode.at(top.ip + 1) : 0,
          (top.ip + 2 < bclen) ? vm.pack.fns.at(top.fnid).bytecode.at(top.ip + 2) : 0,
          (top.ip + 3 < bclen) ? vm.pack.fns.at(top.fnid).bytecode.at(top.ip + 3) : 0,
          (top.ip + 4 < bclen) ? vm.pack.fns.at(top.fnid).bytecode.at(top.ip + 4) : 0,
          (top.ip + 5 < bclen) ? vm.pack.fns.at(top.fnid).bytecode.at(top.ip + 5) : 0
        }
      };
      const auto a = table.at(static_cast<bc::bc_t>(vm.pack.fns.at(top.fnid).bytecode.at(top.ip)))(vm, op);
      return dispatch(a);
    };

    return dispatch(0);
  }
}