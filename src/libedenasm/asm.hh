#pragma once

#include "../libeden/eden.hh"

namespace edn::as {
  auto assemble_text_to_pack(cref<str> code) -> edn::pack::pack;

  struct tok_kw { str keyword; };
  struct tok_int { i64 num; };
  struct tok_flt { float num; };
  struct tok_str { str content; };
  struct tok_at {};
  struct tok_dot {};
  struct tok_comma {};
  struct tok_arrow {};
  using token = std::variant<tok_kw, tok_int, tok_flt, tok_str, tok_at, tok_dot, tok_comma, tok_arrow>;

  auto lex(cref<str> code) -> vec<token>;
}