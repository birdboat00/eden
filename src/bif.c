#include "eden.h"

edn_err_t edn_bif_printreg(edn_vm_t* vm, const edn_op_t* op) {
  const edn_reg_t srcreg = vm->registers[op->arg3];
  switch (srcreg.type) {
    case integer: printf("%i", srcreg.data.i); break;
    case floating: printf("%f", srcreg.data.f); break;
    case string: printf("%s", srcreg.data.s); break;
    default: printf("%i", srcreg.data); break;
  }
  return edn_make_err(kErrModBif, kErrNone);
}

edn_err_t edn_bif_dispatch_bif(edn_vm_t* vm, u32 bifid, const edn_op_t* op) {

  typedef edn_err_t (*edn_bif_fn_ptr)(edn_vm_t* vm, const edn_op_t* op);

  static edn_bif_fn_ptr dispatch_table[1] = {
    &edn_bif_printreg
  };

  return dispatch_table[bifid <= 0 ? 0 : bifid - 1](vm, op);
}