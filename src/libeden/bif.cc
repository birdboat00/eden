#include "eden.hh"

#include <array>
#include <functional>
#include <iostream>

namespace edn::bif {

  err::err bif_printreg(vm::vm& vm, const bc::op& op, term::term& result) {
    const auto term = vm.regs[op.args[2]];
    str buf;
    const auto err = term::to_str(term, buf);
    if (!err::is_ok(err)) {
      result = term::from<i64>(static_cast<i64>(err.kind));
      return err;
    }
    std::cout << buf;
    result = term::from<i64>(0);
    return err::make_err_none(err::err_module::bif);
  }

  err::err dispatch(vm::vm& vm, u32 bifid, const bc::op& op, term::term& result) {
    using biffn = std::function<err::err(vm::vm&, const bc::op&, term::term&)>;

    static std::array<biffn, 1> table = {
      &bif_printreg
    };

    if ((bifid - 1) >= table.size()) {
      return err::make_err(err::err_kind::bifnotfound, err::err_module::bif);
    }

    return table.at(bifid <= 0 ? 0 : bifid - 1)(vm, op, result);
  }
}