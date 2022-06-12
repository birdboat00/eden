#include "eden.h"

#include <stdlib.h>
#include <stdio.h>

edn_err_t edn_make_err(edn_err_module_t mod, edn_err_kind_t err) {
  return (edn_err_t) { .module = mod, .kind = err };
}

str edn_err_to_str(const edn_err_t* err) {
  str buf = calloc(7, sizeof(char));
  if (isnull(buf)) {
    return "(failed to malloc error string buffer)";
  }
  snprintf(buf, 7, "%i-%i", err->module, err->kind);
  return buf;
}

bool edn_err_is_ok(const edn_err_t err) {
  return err.kind == kErrNone;
}