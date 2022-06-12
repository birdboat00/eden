#include "eden.h"

edn_err_t edn_bif_printreg(edn_vm_t* vm, const edn_op_t* op, edn_term_t* result) {
  const edn_term_t term = vm->registers[op->arg3];
#ifndef PRINT_BUF_LEN
#define PRINT_BUF_LEN 256
#endif
  char buf[PRINT_BUF_LEN];
  const edn_err_t err = edn_term_to_str(&term, buf, PRINT_BUF_LEN);
#undef PRINT_BUF_LEN
  if (!edn_err_is_ok(err)) {
    *result = edn_term_from_i32(err.kind);
    return err;
  }
  printf("%s", buf);
  *result = edn_term_from_i32(0);
  return edn_make_err(kErrModBif, kErrNone);
}

edn_err_t edn_bif_is_integer(edn_vm_t* vm, const edn_op_t* op, edn_term_t* result) {
  const edn_term_t term = vm->registers[op->arg3];
  vm->registers[0] = edn_term_from_i32(edn_term_is_intnum(&term));
  return edn_make_err(kErrModBif, kErrNone);
}

edn_err_t edn_bif_is_float(edn_vm_t* vm, const edn_op_t* op, edn_term_t* result) {
  const edn_term_t term = vm->registers[op->arg3];
  vm->registers[0] = edn_term_from_i32(edn_term_is_floatnum(&term));
  return edn_make_err(kErrModBif, kErrNone);
}

edn_err_t edn_bif_is_string(edn_vm_t* vm, const edn_op_t* op, edn_term_t* result) {
  const edn_term_t term = vm->registers[op->arg3];
  vm->registers[0] = edn_term_from_i32(edn_term_is_string(&term));
  return edn_make_err(kErrModBif, kErrNone);
}

edn_err_t edn_bif_dispatch_bif(edn_vm_t* vm, u32 bifid, const edn_op_t* op, edn_term_t* result) {
  typedef edn_err_t (*edn_bif_fn_ptr)(edn_vm_t* vm, const edn_op_t* op, edn_term_t* result);

  static edn_bif_fn_ptr dispatch_table[4] = {
    &edn_bif_printreg, &edn_bif_is_integer, &edn_bif_is_float, &edn_bif_is_string
  };

  if (bifid >= arraylen(edn_bif_fn_ptr, dispatch_table)) {
    return edn_make_err(kErrModBif, kErrBifNotFound);
  }

  return dispatch_table[bifid <= 0 ? 0 : bifid - 1](vm, op, result);
}