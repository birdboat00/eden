#include "libeden/eden.hh"

namespace edn::btp {
  #define r(n) n
  #define c(c) c
  #define i(table, i) i
  #define l(l) l
  
  constexpr auto op(bc::opcode o) -> bc::bc_t { return static_cast<bc::bc_t>(o); }

  pack::pack create_test_pack() {
    const auto fn_main = pack::fn_builder()
      .signature("main", 0)
      .bytecode({
        bc::ops::label { .name = 0 },
        bc::ops::ldc { .dest = 0, .idx = 0 },
        bc::ops::call { .idx = 1, .tailcall = false }
      }).build();

    const auto fn_putStrLn = pack::fn_builder()
      .signature("putstrln", 1)
      .bytecode({
        bc::ops::nifcallnamed { .arity = 1, .nameidx = 2, .args = { 0 }},
        bc::ops::ldc { .dest = 1, .idx = 1},
        bc::ops::nifcallnamed { .arity = 1, .nameidx = 2, .args = { 1 }},
        bc::ops::ret {}
      }).build();


    return pack::pack_builder()
      .with_name("edn_bitp").with_author("eden project").with_version("22w42a")
      .constant(term::from<str>("Hello, World!"))
      .constant(term::from<str>("\n"))
      .constant(term::from<str>("edn_bif_dbg_printreg/1"))
      .constant(term::from<str>("edn_niftest_helloworld/0"))
      .function(fn_main)
      .function(fn_putStrLn)
      .entry(0)
      .build();
  }
}