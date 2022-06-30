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
#include <format>
#include <iostream>

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

  using pid = u32;

  template<typename... Args>
  [[noreturn]] inline auto panic(strref fmt, Args&&... args) -> void {
    do {
      std::cout << std::vformat(fmt, std::make_format_args(args...));
      exit(1);
    } while(false);
  }

  /**
   * Indicates unreachable code by panicking with a message of 
   * "entered unreachable code".
   */
  [[noreturn]] inline auto unreachable() -> void { panic("entered unreachable code"); }
  /**
   * Indicates unfinished code by panicking with a message of
   * "not yet implemented".
   */
  [[noreturn]] inline auto todo() -> void { panic("not yet implemented"); }
  /**
   * Indicated unimplemented code by panicking with a message of
   * "not implemented".
   */
  [[noreturn]] inline auto unimplemented() -> void { panic("not implemented"); }

  #define EDN_NIF_INIT_FN(initfn) extern "C" int edn_nif_init(void* vm) {\
    if (vm == nullptr) {\
      printf("EDN_NIF_INIT_FN: argument vm is null\n");\
      return 1;\
      }\
    return initfn(vm);\
  }

  #define EDN_NIF_DECL(name) auto name(ref<edn::vm::vm> vm, edn::cref<edn::vec<edn::term::term>> args) -> edn::res<edn::term::term>
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
}

namespace edn {
  template<typename T>
  using res = cpp::result<T, err::kind>;
  template<typename T>
  using refres = cpp::result<T&, err::kind>;
}