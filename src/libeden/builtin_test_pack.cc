#include "eden.hh"

namespace edn::btp {
  #define r(n) n
  #define c(c) c
  #define i(table, i) i
  #define l(l) l
  
  constexpr auto op(bc::opcode o) -> bc::bc_t { return static_cast<bc::bc_t>(o); }

  pack::pack create_test_pack() {
    static const auto fn_main = pack::edn_fn {
      .bytecode = {
        op(bc::opcode::label), l(0),
        op(bc::opcode::lstr), r(0), i(strs, 0),
        op(bc::opcode::call), 1, i(fns, 1),
      },
      .name = "main",
      .arity = 0
    };

    static const auto fn_putStrLn = edn::pack::edn_fn {
      .bytecode = {
        op(bc::opcode::nifcallnamed), 2, i(strs, 2), r(0),
        op(bc::opcode::lstr), r(1), i(strs, 1),
        op(bc::opcode::nifcallnamed), 2, i(strs, 2), r(1),
        op(bc::opcode::ret)
      },
      .name = "putstrln",
      .arity = 1
    };

    return pack::pack {
      .name = "edn_bitp",
      .author = "eden project",
      .version = "22w24a",
      .bytecode_version = kEdenBytecodeVersion,
      .entryfn = 0,
      .ints = { 10, 20, 40 },
      .flts = { -1.2, 1.0, 10.23 },
      .strs = { "Hello, World!", "\n", "edn_bif_printreg/1" },
      .fns = { fn_main, fn_putStrLn }
    };
  }
}