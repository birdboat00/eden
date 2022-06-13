#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <variant>
#include <vector>

namespace edn {
  using u8 = std::uint8_t;
  using u16 = std::uint16_t;
  using u32 = std::uint32_t;
  using i32 = std::int32_t;
  using u64 = std::uint64_t;
  using i64 = std::int64_t;
  using usize = std::size_t;
  using f32 = float;
  using f64 = double;
  using str = std::string;
  template<typename T>
  using sptr = std::shared_ptr<T>;

  const str kEdenVersion = "22w23a";
  const u16 kEdenBytecodeVersion = 0x0001;
  #define EDEN_BUILD_TIME __TIME__ " on " __DATE__
  const str kEdenBuildTime = EDEN_BUILD_TIME; 
  #undef EDEN_BUILD_TIME
  const str kEdenPackMagic = "eDeNPACK";
}

namespace edn::err {
  enum class err_kind {
    none,
    invalidpack,
    invalidfile,
    mallocfail,
    termnotprintable,
    bifnotfound
  };

  enum class err_module {
    bif, btp, err, main, op, pack, term, vm
  };

  struct err {
    err_kind kind;
    err_module mod;
  };

  inline err make_err(err_kind kind, err_module mod) { return err { .kind = kind, .mod = mod }; }
  inline err make_err_none(err_module mod) { return make_err(err_kind::none, mod); }

  str to_str(const err& err);
  inline bool is_ok(const err& err) { return err.kind == err_kind::none; }
}

namespace edn::term {
  struct term { std::variant<i64, f64, str> val; };

  err::err to_str(const term& term, str& buf);
  term numf_to_numi(const term& term);
  term numi_to_numf(const term& term);
  template<typename Kind>
  inline term from(const Kind& val) { return term { val }; }
  template<typename Kind>
  inline bool is(const term& term) { return std::holds_alternative<Kind>(term.val); }
  template<typename Kind>
  inline Kind get(const term& term) { return std::get<Kind>(term.val); }
}

namespace edn::bc {
  using bc_t = i32;

  enum class opcode {
    move, lint, lflt, lstr, add, sub, mul, div, neg,
    call, bifcall, ret, opcodecount
  };

  struct op {
    opcode opcode;
    bc_t args[5];
  };

  str opcode_to_str(const opcode opcode);
  usize opcode_arity(const opcode opcode, bc_t next);
  str op_to_str(const op& op);
}

namespace edn::pack {
  struct edn_fn {
    std::vector<bc::bc_t> bytecode;
  };
  struct pack {
    str name;
    u16 bytecode_version;
    u32 entryfn;

    std::vector<i64> ints;
    std::vector<f64> flts;
    std::vector<str> strs;
    std::vector<edn_fn> fns;
  };

  err::err write_to_file(std::ostream& file, const pack& pack);
  err::err read_from_file(std::istream& file, pack& pack);
  err::err dump_to_file(std::ostream& file, const pack& pack);
}

namespace edn::vm {
  struct callstackentry {
    u32 fnid;
    usize ip;
  };
  struct vm {
    pack::pack& pack;
    std::stack<callstackentry> callstack;
    term::term regs[64];
  };
  err::err run(vm& vm);
}

namespace edn::bif {
  err::err dispatch(vm::vm& vm, u32 bifid, const bc::op& op, term::term& result);
}

namespace edn::btp {
  pack::pack create_test_pack();
}