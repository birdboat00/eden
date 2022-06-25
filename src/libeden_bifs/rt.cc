#include "rt.hh"

namespace edn::bif::rt {
  auto halt_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    const auto exitcode = vm.regs.at(args.at(0));
    exit(term::get<i64>(exitcode));
    
    unreachable();
  }
}