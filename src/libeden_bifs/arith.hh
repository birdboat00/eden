#include <libeden/eden.hh>
#include <libedenvm/vm.hh>

namespace edn::bif::arith {
  EDN_NIF_DECL(sadd_2);
  EDN_NIF_DECL(ssub_2);
  EDN_NIF_DECL(smul_2);
  EDN_NIF_DECL(sdiv_2);
  EDN_NIF_DECL(sneg_1);
}