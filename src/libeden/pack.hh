#pragma once

#include "bc.hh"
#include "types.hh"
#include "term.hh"

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

  err::kind write_to_file(std::ostream& file, const pack& pack);
  res<sptr<pack>> read_from_file(std::istream& file);
  err::kind dump_to_file(std::ostream& file, const pack& pack);

  struct pack_builder {
    pack_builder();

    auto with_name(cref<str> name) -> ref<pack_builder>;
    auto with_author(cref<str> name) -> ref<pack_builder>;
    auto with_version(cref<str> ver) -> ref<pack_builder>;
    auto constant(cref<term::term> term) -> ref<pack_builder>;
    auto constants(cref<vec<term::term>> terms) -> ref<pack_builder>;
    auto function(cref<edn_fn> fn) -> ref<pack_builder>;
    auto entry(u32 entryidx) -> ref<pack_builder>;

    auto build() -> pack;

  private:
    str _name, _author, _version;
    u32 _entryfn;
    edn::vec<term::term> _constants;
    edn::vec<edn_fn> _fns;
  };

  struct fn_builder {
    fn_builder();

    auto signature(cref<str> name, u8 arity) -> ref<fn_builder>;
    auto bytecode(vec<bc::ops::bcop> ops) -> ref<fn_builder>;
    auto op(bc::ops::bcop op) -> ref<fn_builder>;

    auto build() -> edn_fn;

  private:
    str _name;
    u8 _arity;
    vec<bc::ops::bcop> _bc;
  };
}