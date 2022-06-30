#include "bc.hh"

#include <sstream>

namespace edn::bc::ops {
  auto move::to_str() const -> str {
    std::stringstream s;
    s << "move r" << static_cast<i32>(dest) << " <- r" << static_cast<i32>(src);
    return s.str();
  }

  auto ldc::to_str() const -> str {
    std::stringstream s;
    s << "ldc r" << static_cast<i32>(dest) << " <- const$" << idx;
    return s.str();
  }

  auto call::to_str() const -> str {
    std::stringstream s;
    s << "call fn$" << idx;
    return s.str();
  }

  auto ret::to_str() const -> str {
    return "ret";
  }

  auto nifcallnamed::to_str() const -> str {
    std::stringstream s;
    s << "nifcallnamed <" << static_cast<i32>(arity) << "> const$" << nameidx << " args[";
    for (const auto& arg : args) { s << "(" << arg << ")"; }
    s << "]";
    return s.str();
  }

  auto test::to_str() const -> str {
    std::stringstream s;
    s << "test <" << static_cast<i32>(fn) << "> @" << dest << " r" << static_cast<i32>(reg);
    return s.str();
  }

  auto cmp::to_str() const -> str {
    std::stringstream s;
    s << "cmp <" << static_cast<i32>(fn) << "> @" << dest << " r" << static_cast<i32>(rl) << " ? r" << static_cast<i32>(rr);
    return s.str();
  }

  auto jump::to_str() const -> str {
    std::stringstream s;
    s << "jump @" << dest;
    return s.str();
  }

  auto label::to_str() const -> str {
    std::stringstream s;
    s << "@" << name;
    return s.str();
  }

  auto to_str(cref<bcop> op) -> str {
    return std::visit(overload {
      [&](cref<bc::ops::move> mov) -> auto { return mov.to_str(); },
      [&](cref<bc::ops::ldc> ldc) -> auto{ return ldc.to_str();  },
      [&](cref<bc::ops::call> c) -> auto { return c.to_str(); },
      [&](cref<bc::ops::ret> r) -> auto { return r.to_str(); },
      [&](cref<bc::ops::nifcallnamed> nc) -> auto { return nc.to_str(); },
      [&](cref<bc::ops::test> t) -> auto { return t.to_str(); },
      [&](cref<bc::ops::cmp> c) -> auto { return c.to_str(); },
      [&](cref<bc::ops::jump> j) -> auto { return j.to_str(); },
      [&](cref<bc::ops::label> l) -> auto { return l.to_str(); },
      [&](const auto&) { unimplemented(); }
    }, op);
  }
}