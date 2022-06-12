#include "eden.h"

#include <string.h>

edn_err_t edn_term_to_str(const edn_term_t* term, str buf, usize buflen) {

  if (edn_term_is_intnum(term)) {
    snprintf(buf, buflen, "%i", edn_term_get_intnum(term));
    return edn_make_err(kErrModTerm, kErrNone);
  } else if (edn_term_is_floatnum(term)) {
    snprintf(buf, buflen, "%d", edn_term_get_floatnum(term));
    return edn_make_err(kErrModTerm, kErrNone);
  } else if (edn_term_is_string(term)) {
    str val = edn_term_get_string(term);
    snprintf(buf, buflen, "%.*s", strlen(val), val);
    return edn_make_err(kErrModTerm, kErrNone);
  }

  return edn_make_err(kErrModTerm, kErrTermNotPrintable);;
}

edn_term_t edn_term_floatnum_to_intnum(const edn_term_t* term) {
  if (term->type == kTermKindIntnum) return *term;
  else if (term->type == kTermKindFloatnum) {
    return edn_term_from_i32((i32)term->val.f);
  }
}

edn_term_t edn_term_intnum_to_floatnum(const edn_term_t* term) {
  if (term->type == kTermKindFloatnum) return *term;
  else if (term->type == kTermKindIntnum) {
    return edn_term_from_f64((f64)term->val.i);
  }
}

edn_term_t edn_term_from_f64(f64 val) {
  return (edn_term_t) { .type = kTermKindFloatnum, .val.f = val };
}

edn_term_t edn_term_from_i32(i32 val) {
  return (edn_term_t) { .type = kTermKindIntnum, .val.i = val };
}

edn_term_t edn_term_from_str(str val) {
  return (edn_term_t) { .type = kTermKindString, .val.s = val };
}

bool edn_term_is_floatnum(const edn_term_t* term) {
  return term->type == kTermKindFloatnum;
}

f64 edn_term_get_floatnum(const edn_term_t* term) {
  return edn_term_is_floatnum(term) ? term->val.f : 0;
}

bool edn_term_is_intnum(const edn_term_t* term) {
  return term->type == kTermKindIntnum;
}

i32 edn_term_get_intnum(const edn_term_t* term) {
  return edn_term_is_intnum(term) ? term->val.i : 0;
}

bool edn_term_is_string(const edn_term_t* term) {
  return term->type == kTermKindString;
}

str edn_term_get_string(const edn_term_t* term) {
  return edn_term_is_string(term) ? term->val.s : "";
}