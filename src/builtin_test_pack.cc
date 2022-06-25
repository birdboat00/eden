#include "libeden/eden.hh"

namespace edn::btp {
  #define r(n) n
  #define c(c) c
  #define i(table, i) i
  #define l(l) l
  
  constexpr auto op(bc::opcode o) -> bc::bc_t { return static_cast<bc::bc_t>(o); }

  pack::pack create_test_pack() {
    static const auto fn_main = pack::edn_fn {
      .bc = {
        bc::ops::label { .name = 0 },
        bc::ops::ldc { .dest = 0, .idx = 0 },
        bc::ops::call { .idx = 1, .tailcall = false },
      },
      .name = "main",
      .arity = 0
    };

    static const auto fn_putStrLn = edn::pack::edn_fn {
      .bc = {
        bc::ops::nifcallnamed { .arity = 1, .nameidx = 2, .args = { 0 }},
        bc::ops::ldc { .dest = 1, .idx = 1 },
        bc::ops::nifcallnamed { .arity = 1, .nameidx = 2, .args = { 1 }},
        bc::ops::ret {},
      },
      .name = "putstrln",
      .arity = 1
    };

    return pack::pack {
      .name = "edn_bitp",
      .author = "eden project",
      .version = "22w24b",
      .bytecode_version = kEdenBytecodeVersion,
      .entryfn = 0,
      .constants = { term::from<str>("Hello, World!"), term::from<str>("\n"), term::from<str>("edn_bif_dbg_printreg/1"), term::from<str>("edn_niftest_helloworld/0") },
      .fns = { fn_main, fn_putStrLn }
    };
  }
}