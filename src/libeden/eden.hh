#pragma once

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "result.hh"

namespace edn {
  using u8 = std::uint8_t;
  using i8 = std::int8_t;
  using u16 = std::uint16_t;
  using i16 = std::int16_t;
  using u32 = std::uint32_t;
  using i32 = std::int32_t;
  using u64 = std::uint64_t;
  using i64 = std::int64_t;
  using usize = std::size_t;
  using f32 = float;
  using f64 = double;
  using str = std::string;
  using strref = std::string_view;
  template<typename T>
  using sptr = std::shared_ptr<T>;
  template<typename T>
  using ref = T&;
  template<typename T>
  using cref = const T&;
  template<typename T>
  using vec = std::vector<T>;

  const str kEdenVersion = "22w25b";
  const u16 kEdenBytecodeVersion = 0x0002;
  #define EDEN_BUILD_TIME __TIME__ " on " __DATE__
  const str kEdenBuildTime = EDEN_BUILD_TIME; 
  #undef EDEN_BUILD_TIME
  // const str kEdenPackMagic = "3d3np4y4";
  const u16 kEdenPackMagic = 0x3d9c;

  #define panic(fmt, ...)\
    do {\
      (void) fprintf(stderr, "\n\n\n*** (panic) *** [%s:%d]: " fmt "\n\n\n",\
              __FILE__, __LINE__, ##__VA_ARGS__);\
            exit(1);\
    } while(false)

  /**
   * Indicates unreachable code by panicking with a message of 
   * "entered unreachable code".
   */
  #define unreachable() panic("entered unreachable code")
  /**
   * Indicates unfinished code by panicking with a message of
   * "not yet implemented".
   */
  #define todo() panic("not yet implemented")
  /**
   * Indicated unimplemented code by panicking with a message of
   * "not implemented".
   */
  #define unimplemented() panic("not implemented")
  #define obsolete(msg) [[deprecated(msg)]]

  #define EDN_NIF_INIT_FN(initfn) extern "C" int edn_nif_init(void* vm) {\
    if (vm == nullptr) {\
      printf("EDN_NIF_INIT_FN: argument vm is null\n");\
      return 1;\
      }\
    return initfn(vm);\
  }

  #define EDN_NIF_DECL(name) auto name(edn::vm::vm& vm, edn::cref<edn::vec<edn::bc::bc_t>> args) -> edn::res<edn::term::term>
}

namespace edn {
  /**
   * std::variant visitor pattern. 
   * Usage:
   * 
   * std::variant<LightItem, HeavyItem, FragileItem> package;
   * std::visit(overload {
   *    [](LightItem&) { std::cout << "light item\n"; },
   *    [](HeavyItem&) { std::cout << "heavy item\n"; },
   *    [](FragileItem&) { std::cout << "fragile item!\n"; }
   * }, package);
   */
  template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
  // next line not needed in c++20
  template<class... Ts> overload(Ts...) -> overload<Ts...>;
}

namespace edn::err {
  enum class kind {
    none,
    invalidpack,
    invalidfile,
    mallocfail,
    termnotprintable,
    bifnotfound,
    bifinvalidargs,
    divbyzero
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
  template<typename T>
  using refres = cpp::result<T&, err::kind>;
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

namespace edn::pack { struct pack; }
namespace edn::vm { struct vm; }
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

  str opcode_to_str(const opcode opcode);
  usize opcode_arity(const opcode opcode, bc_t next);
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
}

namespace edn::pack {
  struct edn_fn {
    vec<bc::ops::bcop> bc;
    std::unordered_map<u64, usize> labels;
    str name;
    u8 arity;
  };
  struct pack {
    str name;
    str author;
    str version;
    u16 bytecode_version;
    u32 entryfn;

    edn::vec<term::term> constants;
    edn::vec<str> naps;
    edn::vec<edn_fn> fns;
  };

  err::err write_to_file(std::ostream& file, const pack& pack);
  res<sptr<pack>> read_from_file(std::istream& file);
  err::err dump_to_file(std::ostream& file, const pack& pack);
}