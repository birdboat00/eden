#include "vm.hh"

#include <iostream>
#include "nif.hh"

namespace edn::vm {

  usize do_move(vm& vm, proc::ctx& p, cref<bc::ops::move> move) {
    vm.cp_mut().regs.at(move.dest) = vm.cp_mut().regs.at(move.src);
    return 1;
  }

  usize do_ldc(vm& vm, proc::ctx& p, cref<bc::ops::ldc> ldc) {
    vm.cp_mut().regs.at(ldc.dest) = vm.pack->constants.at(ldc.idx);
    return 1;
  }

  usize do_call(vm& vm, proc::ctx& p, cref<bc::ops::call> call) {
    if (call.tailcall) {
      p.callstack.top() = proc::callstackentry { .fn_id = call.idx, .ip = 0 };
    }
    else {
      p.callstack.top().ip++;
      p.callstack.push(proc::callstackentry { .fn_id = call.idx, .ip = 0 });
    }

    return 0;
  }

  usize do_ret(vm& vm, proc::ctx& p, cref<bc::ops::ret> ret) {
    p.callstack.pop();
    if (p.callstack.empty()) {
      vm.sched->kill(p.pid);
    }
    return 0;
  }

  usize do_nifcall(vm& vm, proc::ctx& p, cref<bc::ops::nifcallnamed> nifcall) {
    const auto name = term::get<str>(vm.pack->constants.at(nifcall.nameidx));
    if (!vm.nifs.contains(name)) {
      eprintln("nif with name '{}' is not registered. Available ones are:", name);
      for (const auto& nif : vm.nifs) {
        println("  '{}'", nif.first);
      }
      panic("nif is not registered");
    }

    const auto nifptr = vm.nifs.at(name);
    if (nifcall.args.size() != nifptr.argtypes.size()) {
      panic("nif with name '%s' was called with wrong arity.", name.c_str());
    }

    const auto args = map<term::term, bc::bc_t>(nifcall.args, [&](auto elem, auto idx) {
      const auto argt = nifptr.argtypes.at(idx);
      switch (argt) {
        case nif::argtype::int64: return vm.pack->constants.at(elem); // TODO: do checks
        case nif::argtype::float64: return vm.pack->constants.at(elem); // Todo: do checks
        case nif::argtype::cint32: return term::from<i64>(elem);
        case nif::argtype::reg: return vm.cp_mut().regs.at(elem); // TODO: do checks
        case nif::argtype::str: return vm.pack->constants.at(elem); // TODO: do checks
        default: unreachable();
      }
    });

    const auto res = nifptr.fn(vm, args);
    if (res.has_error()) {
      panic("bif error");
    } else {
      p.regs.at(0) = res.value();
    }

    return 1;
  }

  auto do_test(vm& vm, proc::ctx& p, cref<bc::ops::test> test) -> usize {
    auto is = false;
    switch (test.fn) {
    case bc::ops::test_fun::isint:
      is = term::is<i64>(p.regs.at(test.reg));
      break;
    case bc::ops::test_fun::isflt:
      is = term::is<f64>(p.regs.at(test.reg));
      break;
    case bc::ops::test_fun::isstr:
      is = term::is<str>(p.regs.at(test.reg));
      break;
    default:
      unreachable();
    }

    if (is) p.callstack.top().ip = vm.pack->fns.at(p.callstack.top().fn_id).labels.at(test.dest);
    return is ? 0 : 1;
  }

  auto do_cmp(vm& vm, proc::ctx& p, cref<bc::ops::cmp> cmp) -> usize {
    const auto& lhs = p.regs.at(cmp.rl);
    const auto& rhs = p.regs.at(cmp.rr);
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

    if (is) p.callstack.top().ip = vm.pack->fns.at(p.callstack.top().fn_id).labels.at(cmp.dest);
    return is ? 0 : 1;
  }

  auto do_jump(vm& vm, proc::ctx& p, cref<bc::ops::jump>& j) -> usize {
    p.callstack.top().ip = vm.pack->fns.at(p.callstack.top().fn_id).labels.at(j.dest);
    return 0;
  }

  auto do_label(vm& vm, cref<bc::ops::label> l) -> usize {
    return 1;
  }

  auto dispatch(vm& vm, usize adv) -> err::kind {
    vm.sched->current_mut().callstack.top().ip += adv;

    if (vm.sched->current().callstack.top().ip >= vm.pack->fns.at(vm.sched->current().callstack.top().fn_id).bc.size()) {
      println("[bytecode end reached]");
      return err::kind::none;
    }

    const auto& fn = vm.pack->fns.at(vm.sched->current().callstack.top().fn_id);
    const auto& op = fn.bc.at(vm.sched->current().callstack.top().ip);

    const auto next = std::visit(overload {
      [&](cref<bc::ops::move> mov) -> auto { return do_move(vm, vm.cp_mut(), mov); },
      [&](cref<bc::ops::ldc> ldc) -> auto{ return do_ldc(vm, vm.cp_mut(), ldc);  },
      [&](cref<bc::ops::call> c) -> auto { return do_call(vm, vm.cp_mut(), c); },
      [&](cref<bc::ops::ret> r) -> auto { return do_ret(vm, vm.cp_mut(), r);  },
      [&](cref<bc::ops::nifcallnamed> nc) -> auto { return do_nifcall(vm, vm.cp_mut(), nc); },
      [&](cref<bc::ops::test> t) -> auto { return do_test(vm, vm.cp_mut(), t); },
      [&](cref<bc::ops::cmp> c) -> auto { return do_cmp(vm, vm.cp_mut(), c); },
      [&](cref<bc::ops::jump> j) -> auto { return do_jump(vm, vm.cp_mut(), j); },
      [&](cref<bc::ops::label> l) -> auto { return do_label(vm, l); },
      [&](const auto&) { unimplemented(); }
    }, op);

    return dispatch(vm, next);
  }

  auto run(vm& vm) -> err::kind {
    vm.sched = std::make_shared<sched::scheduler>();
    const auto entrypid = vm.sched->spawn(vm.pack->entryfn);
    const auto pid = vm.sched->next();
    println();
    return dispatch(vm, 0);
  }

  auto register_nif(ref<vm> vm, cref<str> name, nif::niffn fnptr, cref<vec<nif::argtype>> argtypes) -> bool {
    if (vm.nifs.contains(name)) {
      return false;
    }

    vm.nifs.insert_or_assign(name, nif::nifptr { .fn = fnptr, .argtypes = argtypes });
    return true;
  }
}