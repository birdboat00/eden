#include "pack.hh"

namespace edn::bif::pack {
  EDN_NIF_DECL(getpackname_0) {
    return term::from<str>(vm.pack->name);
  }

  EDN_NIF_DECL(getpackversion_0) {
    return term::from<str>(vm.pack->version);
  }
}