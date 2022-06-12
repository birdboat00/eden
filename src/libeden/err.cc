#include "eden.hh"

#include <sstream>

namespace edn::err {
  str to_str(const err& err) {
    std::stringstream stream;
    stream << static_cast<i32>(err.mod) << "-" << static_cast<i32>(err.kind);
    return stream.str();
  }
}