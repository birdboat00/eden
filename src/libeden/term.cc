#include "eden.hh"

#include <sstream>

namespace edn::term {
  err::err to_str(const term& term, str& buf) {
    std::stringstream stream;
    if (is<i64>(term)) {
      stream << get<i64>(term);
      return err::make_err_none(err::err_module::term);
    } else if (is<f64>(term)) {
      stream << get<f64>(term);
      return err::make_err_none(err::err_module::term);
    } else if (is<str>(term)) {
      stream << get<str>(term);
      return err::make_err_none(err::err_module::term);
    }

    return err::make_err(err::err_kind::termnotprintable, err::err_module::term);
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