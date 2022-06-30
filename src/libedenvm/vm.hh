#pragma once

// #include <libeden/eden.hh>
#include "../libeden/eden.hh"
#include "nif.hh"
#include "sched.hh"

namespace edn::vm {
  struct vm {
    sptr<pack::pack> pack;
    std::unordered_map<str, nif::nifptr> nifs;

    sptr<sched::scheduler> sched;

    inline auto cp_mut() -> proc::ctx& { return sched->current_mut(); }
  };

  auto run(vm& vm) -> err::kind;
  auto register_nif(ref<vm> vm, cref<str> name, nif::niffn fnptr, cref<vec<nif::argtype>> argtypes) -> bool;

  template<typename T, typename C>
  vec<T> map(cref<vec<C>> array, auto iteratee) {
    auto idx = -1;
    auto len = array.size();
    vec<T> v(len);
    while(++idx < len) {
      v[idx] = iteratee(array[idx], idx);
    }
    return v;
  }
}