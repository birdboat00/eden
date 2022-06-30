#include "term.hh"

#include <sstream>

namespace edn::term {
  res<str> to_str(const term& term) noexcept {
    std::stringstream stream;
    if (is<i64>(term)) {
      stream << get<i64>(term);
      return stream.str();
    } else if (is<f64>(term)) {
      stream << get<f64>(term);
      return stream.str();
    } else if (is<str>(term)) {
      stream << get<str>(term);
      return stream.str();
    } else if (is<pid>(term)) {
      stream << "<pid." << get<pid>(term) << ">";
      return stream.str();
    }

    return cpp::fail(err::kind::termnotprintable);
  }

  term numf_to_numi(const term& term) {
    if (is<f64>(term)) return from<i64>(get<f64>(term));
    return term;
  }

  term numi_to_numf(const term& term) {
    if (is<i64>(term)) return from<f64>(get<i64>(term));
    return term;
  }
}