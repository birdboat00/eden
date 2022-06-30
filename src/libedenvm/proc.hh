#pragma once

#include <libeden/eden.hh>

namespace edn::proc {
  using pid = u32;

  struct callstackentry {
    usize fn_id;
    usize ip;
  };

  struct ctx {
    pid pid;
    std::array<term::term, 64> regs;
    std::stack<callstackentry> callstack;
  };
}