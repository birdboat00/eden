#include "eden.hh"

namespace edn::btp {
  #define r(n) n
  #define c(c) c
  #define i(table, i) i
  #define l(l) l
  
  constexpr auto op(bc::opcode o) -> bc::bc_t { return static_cast<bc::bc_t>(o); }

  pack::pack create_test_pack() {
    static const auto fn_main = edn::pack::edn_fn {
      .bytecode = {
        op(bc::opcode::label), l(0),
        op(bc::opcode::lstr), r(0), i(strs, 0),
        op(bc::opcode::lstr), r(1), i(strs, 1),
        op(bc::opcode::lstr), r(2), i(strs, 2),
        op(bc::opcode::call), 1, i(fns, 1),
        op(bc::opcode::move), r(0), r(1),
        op(bc::opcode::call), 1, i(fns, 1),
        op(bc::opcode::move), r(0), r(2),
        op(bc::opcode::call), 1, i(fns, 1),

        op(bc::opcode::test_isstr), l(0), r(0)
      }
    };

    static const auto fn_putStrLn = edn::pack::edn_fn {
      .bytecode = {
        op(bc::opcode::nifcallnamed), 2, i(strs, 4), r(0),
        op(bc::opcode::lstr), r(4), i(strs, 3),
        op(bc::opcode::nifcallnamed), 2, i(strs, 4), r(4),
        op(bc::opcode::ret)
      }
    };

    return pack::pack {
      .name = "edn_bitp",
      .bytecode_version = kEdenBytecodeVersion,
      .entryfn = 0,
      .ints = { 10, 20, 40 },
      .flts = { -1.2, 1.0, 10.23 },
      .strs = { "alpha", "beta", "gamma", "\n", "edn_bif_printreg/1", "edn_bif_getpackname/0" },
      .fns = { fn_main, fn_putStrLn }
    };
  }
}