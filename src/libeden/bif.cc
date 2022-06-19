#include "eden.hh"

#include <array>
#include <chrono>
#include <functional>
#include <iostream>

namespace edn::bif {

  res<term::term> edn_bif_printreg_1(vm::vm& vm, const bc::op& op) {
    const auto s = term::to_str(vm.regs.at(op.args.at(2)));
    if (s.has_error()) {
      return term::from<i64>(static_cast<i64>(s.error()));
    }
    std::cout << s.value();
    return term::from<i64>(0);
  }

  res<term::term> edn_bif_getpackname_0(vm::vm& vm, const bc::op& _op) {
    return term::from<str>(vm.pack.name);
  }

  res<term::term> edn_bif_getedenversion_0(vm::vm& vm, const bc::op& op) {
    return term::from<str>(kEdenVersion);
  }

  res<term::term> edn_bif_halt_1(vm::vm& vm, const bc::op& op) {
    const auto exitcode = vm.regs.at(op.args.at(2));
    exit(term::get<i64>(exitcode));
    return exitcode;
  }

  res<term::term> edn_bif_splus_2(vm::vm& vm, const bc::op& op) {
    const auto lhs = vm.regs[op.args[2]];
    const auto rhs = vm.regs[op.args[3]];

    if (term::is<i64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<i64>(term::get<i64>(lhs) + term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<i64>(lhs) + term::get<f64>(rhs));
      }
    } else if (term::is<f64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) + term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) + term::get<f64>(rhs));
      }
    } else if (term::is<str>(lhs) && term::is<str>(rhs)) {
      return term::from<str>(term::get<str>(lhs) + term::get<str>(rhs));
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  res<term::term> edn_bif_sminus_2(vm::vm& vm, const bc::op& op) {
    const auto lhs = vm.regs[op.args[2]];
    const auto rhs = vm.regs[op.args[3]];

    if (term::is<i64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<i64>(term::get<i64>(lhs) - term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<i64>(lhs) - term::get<f64>(rhs));
      }
    } else if (term::is<f64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) - term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) - term::get<f64>(rhs));
      }
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  res<term::term> edn_bif_smul_2(vm::vm& vm, const bc::op& op) {
    const auto lhs = vm.regs[op.args[2]];
    const auto rhs = vm.regs[op.args[3]];

    if (term::is<i64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<i64>(term::get<i64>(lhs) * term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<i64>(lhs) * term::get<f64>(rhs));
      }
    } else if (term::is<f64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) * term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) * term::get<f64>(rhs));
      }
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  res<term::term> edn_bif_sdiv_2(vm::vm& vm, const bc::op& op) {
    const auto lhs = vm.regs[op.args[2]];
    const auto rhs = vm.regs[op.args[3]];

    // TODO: check for division-by-zero

    if (term::is<i64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<i64>(term::get<i64>(lhs) / term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<i64>(lhs) / term::get<f64>(rhs));
      }
    } else if (term::is<f64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) / term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) / term::get<f64>(rhs));
      }
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  res<term::term> edn_bif_neg_1(vm::vm& vm, const bc::op& op) {
    const auto lhs = vm.regs[op.args[2]];

    if (term::is<f64>(lhs)) {
      return term::from<f64>(-term::get<f64>(lhs));
    } else if (term::is<i64>(lhs)) {
      return term::from<i64>(-term::get<i64>(lhs));
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  res<term::term> edn_bif_is_int_1(vm::vm& vm, const bc::op& op) {
    return term::from<i64>(term::is<i64>(vm.regs.at(op.args.at(2))));
  }

  res<term::term> edn_bif_is_flt_1(vm::vm& vm, const bc::op& op) {
    return term::from<i64>(term::is<f64>(vm.regs.at(op.args.at(2))));
  }

  res<term::term> edn_bif_is_str_1(vm::vm& vm, const bc::op& op) {
    return term::from<i64>(term::is<str>(vm.regs.at(op.args.at(2))));
  }

  res<term::term> edn_bif_unixtime_0(vm::vm& vm, const bc::op& op) {
    const auto now = std::chrono::system_clock::now();
    return term::from<i64>(std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
  }

  void register_bifs(vm::vm& vm) {
    vm::register_nif(vm, "edn_bif_printreg/1", &edn_bif_printreg_1);
    vm::register_nif(vm, "edn_bif_getpackname/0", &edn_bif_getpackname_0);
    vm::register_nif(vm, "edn_bif_getedenversion/0", &edn_bif_getedenversion_0);

    vm::register_nif(vm, "edn_bif_halt/1", &edn_bif_halt_1);

    vm::register_nif(vm, "edn_bif_splus/2", &edn_bif_splus_2);
    vm::register_nif(vm, "edn_bif_sminus/2", &edn_bif_sminus_2);
    vm::register_nif(vm, "edn_bif_smul/2", &edn_bif_smul_2);
    vm::register_nif(vm, "edn_bif_sdiv/2", &edn_bif_sdiv_2);
    vm::register_nif(vm, "edn_bif_neg/1", &edn_bif_neg_1);

    vm::register_nif(vm, "edn_bif_is_int/1", &edn_bif_is_int_1);
    vm::register_nif(vm, "edn_bif_is_flt/1", &edn_bif_is_flt_1);
    vm::register_nif(vm, "edn_bif_is_str/1", &edn_bif_is_str_1);

    vm::register_nif(vm, "edn_bif_unixtime/0", &edn_bif_unixtime_0);
  }
}