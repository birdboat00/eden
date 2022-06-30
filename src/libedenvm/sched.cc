#include "sched.hh"

#include <algorithm>

namespace edn::sched {
    auto scheduler::spawn(u32 fnid) -> pid {
      proc::ctx newproc;
      newproc.pid = last_pid++;
      newproc.callstack.push(proc::callstackentry {
        .fn_id = fnid,
        .ip = 0
      });
      
      ready.push_back(newproc);

      return newproc.pid;
    }

    auto scheduler::kill(pid pid) -> void {
      if (current().pid == pid) {
        auto proc = ready.front();
        curr = proc;
        ready.pop_front();
        return;
      }

      auto proc = std::find_if(std::begin(waiting), std::end(waiting), [&pid](const auto& p) { return p.pid == pid; });
      if (proc != std::end(waiting)) {
        waiting.erase(proc);
        return;
      }

      proc = std::find_if(std::begin(ready), std::end(ready), [&pid](const auto& p) { return p.pid == pid; });
      if (proc != std::end(ready)) {
        ready.erase(proc);
        return;
      }
    }

    auto scheduler::count() -> usize {
      return waiting.size() + ready.size() + 1;
    }

    auto scheduler::next() -> pid {
      auto proc = ready.front();
      ready.push_back(curr);
      curr = proc;
      ready.pop_front();
      return curr.pid;
    }

    auto scheduler::current_pid() -> pid {
      return curr.pid;
    }

    auto scheduler::current() -> const proc::ctx& {
      return curr;
    }

    auto scheduler::current_mut() -> proc::ctx& {
      return curr;
    }
}