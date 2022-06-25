#pragma once

#include "../libeden/eden.hh"

namespace edn::vm { struct vm; }

namespace edn::nif {
  using niffn = std::function <res<term::term>(edn::vm::vm&, const vec<bc::bc_t>& args)>;

  struct niflib {
    void* libptr;
    ~niflib();
  };

  auto load(cref<str> filename, vm::vm& vm) -> res<sptr<niflib>>;
}