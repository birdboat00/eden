#include "../libeden/eden.hh"
#include "../libedenvm/vm.hh"

#include <memory>
#include <iostream>

namespace edn::niftest {
  EDN_NIF_DECL(edn_nif_niftest_helloworld_0) {
    std::cout << "hello, world from niftest!" << std::endl;
    return term::from<i64>(0);
  }
}

int edn_niftest_init(void* vm) {
  auto& ednvm = *(static_cast<edn::vm::vm*>(vm));
  edn::vm::register_nif(ednvm, "edn_niftest_helloworld/0", std::addressof(edn::niftest::edn_nif_niftest_helloworld_0), {});
  
  return 0;
}

EDN_NIF_INIT_FN(edn_niftest_init)