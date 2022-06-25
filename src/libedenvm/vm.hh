#pragma once

// #include <libeden/eden.hh>
#include "../libeden/eden.hh"
#include "nif.hh"

namespace edn::vm {
  struct callstackentry {
    usize fn_id;
    usize ip;
  };

  struct vm {
    sptr<pack::pack> pack;
    std::stack<callstackentry> callstack;
    std::array<term::term, 64> regs;

    std::unordered_map<str, nif::niffn> nifs;
  };

  auto run(vm& vm) -> err::err;
  auto register_nif(vm& vm, cref<str> name, nif::niffn fnptr) -> bool;
}