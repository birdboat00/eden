#include <libeden/eden.hh>
#include <libedenvm/vm.hh>

namespace edn::bif::terms {
  auto is_int_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
  auto is_flt_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
  auto is_str_1(vm::vm& vm, cref<vec<bc::bc_t>> args) -> res<term::term>;
}