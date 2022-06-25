#include "vm.hh"

#include <iostream>

namespace edn::vm {

  usize do_move(vm& vm, cref<bc::ops::move> move) {
    vm.regs.at(move.dest) = vm.regs.at(move.src);
    return 1;
  }

  usize do_ldc(vm& vm, cref<bc::ops::ldc> ldc) {
    vm.regs.at(ldc.dest) = vm.pack->constants.at(ldc.idx);
    return 1;
  }

  usize do_call(vm& vm, cref<bc::ops::call> call) {
    
    if (call.tailcall) {
      vm.callstack.top() = callstackentry{
        .fn_id = call.idx, .ip = 0
      };
    }
    else {
      vm.callstack.top().ip++;

      vm.callstack.push(callstackentry{
        .fn_id = call.idx, .ip = 0
        });
    }

    return 0;
  }

  usize do_ret(vm& vm, cref<bc::ops::ret> ret) {
    vm.callstack.pop();
    return 0;
  }

  usize do_nifcall(vm& vm, cref<bc::ops::nifcallnamed> nifcall) {
    const auto name = term::get<str>(vm.pack->constants.at(nifcall.nameidx));
    if (!vm.nifs.contains(name)) {
      std::cout << "nif with name '" << name << "' is not registered. available ones are:" << std::endl;
      for (const auto& nif : vm.nifs) {
        std::cout << "  '" << nif.first << "'" << std::endl;
      }
      throw std::runtime_error("nif is not registered.");
    }

    const auto res = vm.nifs.at(name)(vm, nifcall.args);
    if (res.has_error()) {
      std::cout << "bif error" << std::endl;
      throw std::runtime_error("bif error");
    } else {
      vm.regs.at(0) = res.value();
    }

    return 1;
  }

  auto do_test(vm& vm, cref<bc::ops::test> test) -> usize {
    auto is = false;
    switch (test.fn) {
    case bc::ops::test_fun::isint:
      is = term::is<i64>(vm.regs.at(test.reg));
      break;
    case bc::ops::test_fun::isflt:
      is = term::is<f64>(vm.regs.at(test.reg));
      break;
    case bc::ops::test_fun::isstr:
      is = term::is<str>(vm.regs.at(test.reg));
      break;
    default:
      unreachable();
    }

    if (is) vm.callstack.top().ip = vm.pack->fns.at(vm.callstack.top().fn_id).labels.at(test.dest);
    return is ? 0 : 1;
  }

  auto do_cmp(vm& vm, cref<bc::ops::cmp> cmp) -> usize {
    const auto& lhs = vm.regs.at(cmp.rl);
    const auto& rhs = vm.regs.at(cmp.rr);
    auto is = false;

    switch (cmp.fn) {
    case bc::ops::cmp_fun::islt:
      is = lhs.val < rhs.val;
      break;
    case bc::ops::cmp_fun::isge:
      is = lhs.val >= rhs.val;
      break;
    case bc::ops::cmp_fun::isne:
      is = lhs.val != rhs.val;
      break;
    case bc::ops::cmp_fun::iseq:
      is = lhs.val == rhs.val;
      break;
    default: unreachable();
    }

    if (is) vm.callstack.top().ip = vm.pack->fns.at(vm.callstack.top().fn_id).labels.at(cmp.dest);
    return is ? 0 : 1;
  }

  auto do_jump(vm& vm, cref<bc::ops::jump>& j) -> usize {
    vm.callstack.top().ip = vm.pack->fns.at(vm.callstack.top().fn_id).labels.at(j.dest);
    return 0;
  }

  auto do_label(vm& vm, cref<bc::ops::label> l) -> usize {
    return 1;
  }

  auto dispatch(vm& vm, usize adv) -> err::err {
    vm.callstack.top().ip += adv;

    if (vm.callstack.top().ip >= vm.pack->fns.at(vm.callstack.top().fn_id).bc.size()) {
      std::cout << "vm: bytecode end reached." << std::endl;
      return err::make_err_none(err::err_module::vm);
    }

    const auto& fn = vm.pack->fns.at(vm.callstack.top().fn_id);
    const auto& op = fn.bc.at(vm.callstack.top().ip);

    const auto next = std::visit(overload {
      [&](cref<bc::ops::move> mov) -> auto { return do_move(vm, mov); },
      [&](cref<bc::ops::ldc> ldc) -> auto{ return do_ldc(vm, ldc);  },
      [&](cref<bc::ops::call> c) -> auto { return do_call(vm, c); },
      [&](cref<bc::ops::ret> r) -> auto { return do_ret(vm, r);  },
      [&](cref<bc::ops::nifcallnamed> nc) -> auto { return do_nifcall(vm, nc); },
      [&](cref<bc::ops::test> t) -> auto { return do_test(vm, t); },
      [&](cref<bc::ops::cmp> c) -> auto { return do_cmp(vm, c); },
      [&](cref<bc::ops::jump> j) -> auto { return do_jump(vm, j); },
      [&](cref<bc::ops::label> l) -> auto { return do_label(vm, l); },
      [&](const auto&) { unimplemented(); }
    }, op);

    return dispatch(vm, next);
  }

  auto run(vm& vm) -> err::err {
    vm.callstack.push(callstackentry{
      .fn_id = vm.pack->entryfn,
      .ip = 0
    });
    std::cout << "callstack setup finished.." << std::endl;

    return dispatch(vm, 0);
  }

  auto register_nif(vm& vm, cref<str> name, nif::niffn fnptr) -> bool {
    if (vm.nifs.contains(name)) {
      return false;
    }

    vm.nifs.insert_or_assign(name, fnptr);
    return true;
  }
}