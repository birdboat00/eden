#include "pack.hh"

namespace edn::bif::pack {
  auto getpackname_0(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    return term::from<str>(vm.pack->name);
  }

  auto getpackversion_0(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term> {
    return term::from<str>(vm.pack->version);
  }
}