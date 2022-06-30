#include <libeden/eden.hh>
#include <libedenvm/vm.hh>

#include <array>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>

#include "arith.hh"
#include "dbg.hh"
#include "pack.hh"
#include "rt.hh"

namespace edn::bif {
  void register_bifs(vm::vm& vm) {
    vm::register_nif(vm, "edn_bif_dbg_printreg/1", std::addressof(dbg::printreg_1), { nif::argtype::reg });

    vm::register_nif(vm, "edn_bif_pack_getpackname/0", std::addressof(pack::getpackname_0), {});
    vm::register_nif(vm, "edn_bif_pack_getedenversion/0", std::addressof(pack::getpackversion_0), {});

    vm::register_nif(vm, "edn_bif_rt_halt/1", std::addressof(rt::halt_1), { nif::argtype::reg });

    vm::register_nif(vm, "edn_bif_arith_sadd/2", std::addressof(arith::sadd_2), { nif::argtype::reg, nif::argtype::reg });
    vm::register_nif(vm, "edn_bif_arith_ssub/2", &arith::ssub_2, { nif::argtype::reg, nif::argtype::reg });
    vm::register_nif(vm, "edn_bif_arith_smul/2", &arith::smul_2, { nif::argtype::reg, nif::argtype::reg });
    vm::register_nif(vm, "edn_bif_arith_sdiv/2", &arith::sdiv_2, { nif::argtype::reg, nif::argtype::reg });
    vm::register_nif(vm, "edn_bif_arith_neg/1", &arith::sneg_1, { nif::argtype::reg });
  }
}