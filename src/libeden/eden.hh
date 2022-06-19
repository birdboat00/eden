#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "result.hh"

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
  template<typename T>
  using ref = T&;
  template<typename T>
  using cref = const T&;

  const str kEdenVersion = "22w24a";
  const u16 kEdenBytecodeVersion = 0x0001;
  #define EDEN_BUILD_TIME __TIME__ " on " __DATE__
  const str kEdenBuildTime = EDEN_BUILD_TIME; 
  #undef EDEN_BUILD_TIME
  const str kEdenPackMagic = "eDeNPACK";
}

namespace edn::err {
  enum class kind {
    none,
    invalidpack,
    invalidfile,
    mallocfail,
    termnotprintable,
    bifnotfound,
    bifinvalidargs
  };

  enum class err_module {
    bif, btp, err, main, op, pack, term, vm
  };

  struct err {
    kind kind;
    err_module mod;
  };

  inline err make_err(kind kind, err_module mod) { return err { .kind = kind, .mod = mod }; }
  inline err make_err_none(err_module mod) { return make_err(kind::none, mod); }

  str to_str(const err& err);
  inline bool is_ok(const err& err) { return err.kind == kind::none; }
}

namespace edn {
  template<typename T>
  using res = cpp::result<T, err::kind>;
}

namespace edn::term {
  struct term { std::variant<i64, f64, str> val; };

  res<str> to_str(const term& term) noexcept;
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
    move, lint, lflt, lstr, lfun, add, sub, mul, div, neg,
    call, tailcall, bifcall, ret, nifcallnamed, 
    test_isint, test_isflt, test_isstr, test_isfun,
    cmp_islt, cmp_isge, cmp_iseq, cmp_isne,
    jump,
    label,
    opcodecount
  };

  struct op {
    opcode opcode;
    std::array<bc_t, 5> args;
  };

  str opcode_to_str(const opcode opcode);
  usize opcode_arity(const opcode opcode, bc_t next);
  str op_to_str(const op& op);
}

namespace edn::pack {
  struct edn_fn {
    std::vector<bc::bc_t> bytecode;
    std::unordered_map<u64, usize> labels;
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

namespace edn {
  namespace vm { struct vm; }
  using niffn = std::function<res<term::term>(edn::vm::vm&, const edn::bc::op&)>;
}

namespace edn::vm {
  struct callstackentry {
    u32 fnid;
    usize ip;
  };
  struct vm {
    pack::pack& pack;
    std::stack<callstackentry> callstack;
    std::array<term::term, 64> regs;

    std::unordered_map<str, niffn> nifs;
  };
  err::err run(vm& vm);
  void register_nif(vm& vm, cref<str> name, niffn impl);
}

namespace edn::bif {
  void register_bifs(vm::vm& vm);
}

namespace edn::btp {
  pack::pack create_test_pack();
}