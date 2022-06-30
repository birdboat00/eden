#pragma once

#include "types.hh"

namespace edn::bc {
  using bc_t = i32;

  enum class opcode {
    move, ldc,
    call, tailcall, ret, nifcallnamed, 
    test_isint, test_isflt, test_isstr, test_isfun,
    cmp_islt, cmp_isge, cmp_iseq, cmp_isne,
    jump,
    label,
    opcodecount
  };
}

namespace edn::bc::ops {
  using reg_t = u8;
  using tbl_t = u8;
  using idx_t = u32;

  struct move { reg_t dest; reg_t src; str to_str() const; };
  struct ldc { reg_t dest; idx_t idx; str to_str() const; };

  struct call { idx_t idx; bool tailcall; str to_str() const; };
  struct ret { str to_str() const; };
  struct nifcallnamed { u8 arity; idx_t nameidx; vec<bc_t> args; str to_str() const; };

  enum class test_fun { isint, isflt, isstr, isfun };
  struct test { usize dest; test_fun fn; u8 reg; str to_str() const; };
  enum class cmp_fun { islt, isge, iseq, isne };
  struct cmp { usize dest; cmp_fun fn; u8 rl; u8 rr; str to_str() const; };

  struct label { usize name; str to_str() const; };
  struct jump { usize dest; str to_str() const; };
  
  using bcop = std::variant<move, ldc, call, ret, nifcallnamed, test, cmp, label, jump>;
  auto to_str(cref<bcop> op) -> str;
}