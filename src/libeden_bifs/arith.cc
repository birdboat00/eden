#include "arith.hh"

namespace edn::bif::arith {
  EDN_NIF_DECL(sadd_2) {
    const auto& lhs = args.at(0);
    const auto& rhs = args.at(1);

    if (term::is<i64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<i64>(term::get<i64>(lhs) + term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<i64>(lhs) + term::get<f64>(rhs));
      }
    } else if (term::is<f64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) + term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) + term::get<f64>(rhs));
      }
    } else if (term::is<str>(lhs) && term::is<str>(rhs)) {
      return term::from<str>(term::get<str>(lhs) + term::get<str>(rhs));
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  EDN_NIF_DECL(ssub_2) {
    const auto& lhs = args.at(0);
    const auto& rhs = args.at(1);

    if (term::is<i64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<i64>(term::get<i64>(lhs) - term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<i64>(lhs) - term::get<f64>(rhs));
      }
    } else if (term::is<f64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) - term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) - term::get<f64>(rhs));
      }
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  EDN_NIF_DECL(smul_2) {
    const auto& lhs = args.at(0);
    const auto& rhs = args.at(1);

    if (term::is<i64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<i64>(term::get<i64>(lhs) * term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<i64>(lhs) * term::get<f64>(rhs));
      }
    } else if (term::is<f64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) * term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) * term::get<f64>(rhs));
      }
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  EDN_NIF_DECL(sdiv_2) {
    const auto& lhs = args.at(0);
    const auto& rhs = args.at(0);

    if (
        (term::is<i64>(rhs) && term::get<i64>(rhs) == 0)
        || (term::is<f64>(rhs) && term::get<f64>(rhs) == 0.0f)) {
      return cpp::fail(err::kind::divbyzero);
    }

    if (term::is<i64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<i64>(term::get<i64>(lhs) / term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<i64>(lhs) / term::get<f64>(rhs));
      }
    } else if (term::is<f64>(lhs)) {
      if (term::is<i64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) / term::get<i64>(rhs));
      } else if (term::is<f64>(rhs)) {
        return term::from<f64>(term::get<f64>(lhs) / term::get<f64>(rhs));
      }
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }

  EDN_NIF_DECL(sneg_1) {
    const auto arg = args.at(0);

    if (term::is<f64>(arg)) {
      return term::from<f64>(-term::get<f64>(arg));
    } else if (term::is<i64>(arg)) {
      return term::from<i64>(-term::get<i64>(arg));
    }

    return cpp::fail(err::kind::bifinvalidargs);
  }
}