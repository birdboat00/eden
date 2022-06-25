#include "term.hh"

namespace edn::bif::terms {
  auto is_int_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    return term::from<i64>(term::is<i64>(vm.regs.at(args.at(0))));
  }

  auto is_flt_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    return term::from<i64>(term::is<f64>(vm.regs.at(args.at(0))));
  }

  auto is_str_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    return term::from<i64>(term::is<str>(vm.regs.at(args.at(0))));
  }
}