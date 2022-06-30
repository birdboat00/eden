#pragma once

#include "types.hh"

namespace edn::term {
  struct term { std::variant<i64, f64, str, pid> val; };

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