#include <libeden/eden.hh>
#include <libedenvm/vm.hh>

namespace edn::bif::dbg {
  auto printreg_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
}