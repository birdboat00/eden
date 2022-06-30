#pragma once

#include <deque>

#include "proc.hh"

namespace edn::sched {
  struct scheduler {
    std::deque<proc::ctx> ready;
    std::deque<proc::ctx> waiting;

    proc::ctx curr;

    auto spawn(u32 fnid) -> pid;
    auto kill(pid pid) -> void;

    auto count() -> usize;

    auto next() -> pid;

    auto current_pid() -> pid;
    auto current() -> const proc::ctx&;
    auto current_mut() -> proc::ctx&;

  private:
    u32 last_pid = 0;
  };
}