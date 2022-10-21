#include "dbg.hh"

#include <iostream>

namespace edn::bif::dbg {
  EDN_NIF_DECL(printreg_1) {
    const auto s = term::to_str(args.at(0));
    if (s.has_error()) {
      return term::from<i64>(static_cast<i64>(s.error()));
    }
    print(s.value());
    return term::from<i64>(0);
  }
}