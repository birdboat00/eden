#include <libeden/eden.hh>
#include <libedenvm/vm.hh>

namespace edn::bif::rt {
  auto halt_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
}