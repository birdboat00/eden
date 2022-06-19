#include "eden.hh"

#include <functional>
#include <iostream>

namespace edn::vm {

  usize do_move(vm& vm, const bc::op& op) {
    vm.regs.at(op.args.at(0)) = vm.regs.at(op.args.at(1));
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
    const auto fnid = op.args[1];

    vm.callstack.top() = callstackentry {
      .fnid = static_cast<u32>(fnid), .ip = 0
    };

    return 0;
  }

  usize do_nifcallnamed(vm& vm, const bc::op& op) {
    const auto arity = op.args[0];
    const auto bifname = vm.pack.strs.at(op.args[1]);
    if (!vm.nifs.contains(bifname)) {
      throw std::runtime_error(err::to_str(err::make_err(err::kind::bifnotfound, err::err_module::vm)));
    }
    const auto result = vm.nifs.at(bifname)(vm, op);
    if (result.has_error()) {
      throw std::runtime_error(err::to_str(err::make_err(result.error(), err::err_module::vm)));
    } else {
      vm.regs.at(0) = result.value();
    }
    return 2 + arity; // opcode + arity + <args>
  }

  usize do_ret(vm& vm, const bc::op& _op) {
    vm.callstack.pop();
    return 0;
  }

  usize do_test_isint(vm& vm, const bc::op& op) {
    const auto is = term::is<i64>(vm.regs.at(op.args.at(1)));
    std::cout << "test_isint " << is << " (" << term::to_str(vm.regs.at(op.args.at(1))).value() << ")" << std::endl;
    if (is) vm.callstack.top().ip = vm.pack.fns.at(vm.callstack.top().fnid).labels.at(op.args.at(0));
    return is ? 0 : 3;
  }

  usize do_test_isflt(vm& vm, const bc::op& op) {
    const auto is = term::is<f64>(vm.regs.at(op.args.at(1)));
    if (is) vm.callstack.top().ip = vm.pack.fns.at(vm.callstack.top().fnid).labels.at(op.args.at(0));
    return is ? 0 : 3;
  }
  
  usize do_test_isstr(vm& vm, const bc::op& op) {
    const auto is = term::is<str>(vm.regs.at(op.args.at(1)));
    std::cout << "test_isstr " << is << " (" << term::to_str(vm.regs.at(op.args.at(1))).value() << ")" << std::endl;
    if (is) vm.callstack.top().ip = vm.pack.fns.at(vm.callstack.top().fnid).labels.at(op.args.at(0));
    return is ? 0 : 3;
  }

  usize do_cmp_islt(vm& vm, const bc::op& op) {
    const auto lhs = vm.regs.at(op.args.at(1));
    const auto rhs = vm.regs.at(op.args.at(2));
    const auto is = lhs.val < rhs.val;
    if (is) vm.callstack.top().ip = vm.pack.fns.at(vm.callstack.top().fnid).labels.at(op.args.at(0));
    return is ? 0 : 4;
  }

  usize do_cmp_isge(vm& vm, const bc::op& op) {
    const auto lhs = vm.regs.at(op.args.at(1));
    const auto rhs = vm.regs.at(op.args.at(2));
    const auto is = lhs.val >= rhs.val;
    if (is) vm.callstack.top().ip = vm.pack.fns.at(vm.callstack.top().fnid).labels.at(op.args.at(0));
    return is ? 0 : 4;
  }

  usize do_cmp_iseq(vm& vm, const bc::op& op) {
    const auto lhs = vm.regs.at(op.args.at(1));
    const auto rhs = vm.regs.at(op.args.at(2));
    const auto is = lhs.val == rhs.val;
    if (is) vm.callstack.top().ip = vm.pack.fns.at(vm.callstack.top().fnid).labels.at(op.args.at(0));
    return is ? 0 : 4;
  }

  usize do_cmp_isne(vm& vm, const bc::op& op) {
    const auto lhs = vm.regs.at(op.args.at(1));
    const auto rhs = vm.regs.at(op.args.at(2));
    const auto is = lhs.val != rhs.val;
    if (is) vm.callstack.top().ip = vm.pack.fns.at(vm.callstack.top().fnid).labels.at(op.args.at(0));
    return is ? 0 : 4;
  }

  usize do_jump(vm& vm, const bc::op& op) {
    vm.callstack.top().ip = vm.pack.fns.at(vm.callstack.top().fnid).labels.at(op.args.at(0));
    return 0;
  }

  usize do_label(vm& vm, const bc::op& op) {
    return 2;
  }

  usize op_unimplemented(vm& _vm, const bc::op& op) {
    std::cout << "vm - op not implemented. (" << bc::op_to_str(op) << ")." <<  std::endl;
    return 0;
  }

  err::err run(vm& vm) {
    using ophandler = std::function<usize(struct vm&, const bc::op& op)>;

    static const std::array<ophandler, static_cast<usize>(bc::opcode::opcodecount)> table = {
      &do_move, &do_lint, &do_lflt, &do_lstr, &op_unimplemented /*lfun*/,
      &op_unimplemented /*add*/, &op_unimplemented /*sub*/, &op_unimplemented/*mul*/, &op_unimplemented/*div*/, &op_unimplemented/*neg*/,
      &do_call, &do_tailcall, &op_unimplemented /*bifcall*/, &do_ret, &do_nifcallnamed,
      &do_test_isint, &do_test_isflt, &do_test_isstr, &op_unimplemented /*test_isfun*/,
      &do_cmp_islt, &do_cmp_isge, &do_cmp_iseq, &do_cmp_isne,
      &do_jump,
      &do_label
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

  void register_nif(vm& vm, cref<str> name, niffn impl) {
    vm.nifs.insert_or_assign(name, impl);
  }
}