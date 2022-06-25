#include "arith.hh"

namespace edn::bif::arith {
  auto sadd_2(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    const auto lhs = vm.regs.at(args.at(0));
    const auto rhs = vm.regs.at(args.at(1));

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

  auto ssub_2(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    const auto lhs = vm.regs.at(args.at(0));
    const auto rhs = vm.regs.at(args.at(1));

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

  auto smul_2(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    const auto lhs = vm.regs.at(args.at(0));
    const auto rhs = vm.regs.at(args.at(1));

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

  auto sdiv_2(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    const auto lhs = vm.regs.at(args.at(0));
    const auto rhs = vm.regs.at(args.at(1));

    if (
        (term::is<i64>(rhs) && term::get<i64>(rhs) == 0)
        || (term::is<f64>(rhs) && term::get<f64>(rhs) == 0.0f)) {
      return cpp::fail(err::kind::divbyzero);
    }

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

  auto sneg_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    const auto lhs = vm.regs.at(args.at(0));

    if (term::is<f64>(lhs)) {
      return term::from<f64>(-term::get<f64>(lhs));
    } else if (term::is<i64>(lhs)) {
      return term::from<i64>(-term::get<i64>(lhs));
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }
}