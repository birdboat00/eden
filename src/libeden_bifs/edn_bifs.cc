#include <libeden/eden.hh>
#include <libedenvm/vm.hh>

#include <array>
#include <chrono>
#include <functional>
#include <iostream>

#include "arith.hh"
#include "dbg.hh"
#include "pack.hh"
#include "rt.hh"
#include "term.hh"

namespace edn::bif {
  void register_bifs(vm::vm& vm) {
    vm::register_nif(vm, "edn_bif_dbg_printreg/1", &dbg::printreg_1);

    vm::register_nif(vm, "edn_bif_pack_getpackname/0", &pack::getpackname_0);
    vm::register_nif(vm, "edn_bif_pack_getedenversion/0", &pack::getpackversion_0);

    vm::register_nif(vm, "edn_bif_rt_halt/1", &rt::halt_1);

    vm::register_nif(vm, "edn_bif_arith_sadd/2", &arith::sadd_2);
    vm::register_nif(vm, "edn_bif_arith_ssub/2", &arith::ssub_2);
    vm::register_nif(vm, "edn_bif_arith_smul/2", &arith::smul_2);
    vm::register_nif(vm, "edn_bif_arith_sdiv/2", &arith::sdiv_2);
    vm::register_nif(vm, "edn_bif_arith_neg/1", &arith::sneg_1);

    vm::register_nif(vm, "edn_bif_terms_is_int/1", &terms::is_int_1);
    vm::register_nif(vm, "edn_bif_terms_is_flt/1", &terms::is_flt_1);
    vm::register_nif(vm, "edn_bif_terms_is_str/1", &terms::is_str_1);
  }
}