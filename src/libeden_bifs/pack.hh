#include <libeden/eden.hh>
#include <libedenvm/vm.hh>

namespace edn::bif::pack {
  auto getpackname_0(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
  auto getpackversion_0(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
}