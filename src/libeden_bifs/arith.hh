#include <libeden/eden.hh>
#include <libedenvm/vm.hh>

namespace edn::bif::arith {
  auto sadd_2(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
  auto ssub_2(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
  auto smul_2(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
  auto sdiv_2(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
  auto sneg_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
}