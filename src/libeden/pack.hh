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
}