#pragma once

#include "../libeden/eden.hh"

namespace edn::vm { struct vm; }

namespace edn::nif {
  using niffn = std::function <res<term::term>(ref<edn::vm::vm>, cref<vec<term::term>>)>;
  struct nifptr {
    niffn fn;
    vec<enum class argtype> argtypes;
  };

  struct niflib {
    void* libptr;
    ~niflib();
  };

  auto load(cref<str> filename, vm::vm& vm) -> res<sptr<niflib>>;

  enum class argtype {
    int64, float64, str, reg, cint32,
  };
}