#include "rt.hh"

namespace edn::bif::rt {
  EDN_NIF_DECL(halt_1) {
    const auto exitcode = args.at(0);
    exit(term::get<i64>(exitcode));
    
    unreachable();
  }
}