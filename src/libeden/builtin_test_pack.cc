#include "eden.hh"

namespace edn::btp {
  #define r(n) n
  #define c(c) c
  #define i(i) i
  #define op(o) static_cast<bc::bc_t>(edn::bc::opcode::o)

  pack::pack create_test_pack() {
    static const auto fn_main = edn::pack::edn_fn {
      .bytecode = {
        op(lstr), r(0), i(0),
        op(lstr), r(1), i(1),
        op(lstr), r(2), i(2),
        op(call), 1, 0x01,
        op(move), r(0), r(1),
        op(call), 1, 0x01,
        op(move), r(0), r(2),
        op(call), 1, 0x01
      }
    };

    static const auto fn_putStrLn = edn::pack::edn_fn {
      .bytecode = {
        op(bifcall), 2, 0x01, r(0),
        op(lstr), r(4), i(3),
        op(bifcall), 2, 0x01, r(4),
        op(ret)
      }
    };

    return pack::pack {
      .name = "edn_bitp",
      .bytecode_version = kEdenBytecodeVersion,
      .entryfn = 0,
      .ints = { 10, 20, 40 },
      .flts = { -1.2, 1.0, 10.23 },
      .strs = { "alpha", "beta", "gamma", "\n" },
      .fns = { fn_main, fn_putStrLn }
    };
  }
}