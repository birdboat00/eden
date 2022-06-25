#include "dbg.hh"

#include <iostream>

namespace edn::bif::dbg {
  auto printreg_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    const auto s = term::to_str(vm.regs.at(args.at(0)));
    if (s.has_error()) {
      return term::from<i64>(static_cast<i64>(s.error()));
    }
    std::cout << s.value();
    return term::from<i64>(0);
  }
}