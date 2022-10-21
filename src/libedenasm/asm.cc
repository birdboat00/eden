#include "asm.hh"

#include <queue>

namespace edn::as {
  auto assemble_text_to_pack(cref<str> code) -> edn::pack::pack {

  }

  auto lex(cref<str> code) -> vec<token> {
    for(auto i = 0; i < code.size(); i++) {
      const auto ch = code.at(i);
      const auto next = code.size() == i - 1 ? ' ' : code.at(i + 1);
      auto advance = 1;

      if (ch == ';' && next == ';') {
        advance += 2;
      }
    }
  }
}