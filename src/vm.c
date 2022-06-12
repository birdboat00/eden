#include "eden.h"

#include <stdlib.h>

edn_vm_t* edn_make_vm(edn_pack_t* pack, const edn_vm_params_t params) {
  edn_vm_t* vm = malloc(sizeof(edn_vm_t));
  if (isnull(vm)) {
    return NULL;
  }
  vm->params = params;
  vm->pack = pack;
  vm->callstack_top = 0;

  return vm;
}

#define IMPL_ARTIH_INSTR(op, opname) edn_err_t opname##_terms(const edn_term_t* lhs, const edn_term_t* rhs, edn_term_t* res) {\
  if (edn_term_is_floatnum(lhs)) {\
    if (edn_term_is_floatnum(rhs)) {\
      *res = edn_term_from_f64(edn_term_get_floatnum(lhs) op edn_term_get_floatnum(rhs));\
    } else if (edn_term_is_intnum(rhs)) {\
      *res = edn_term_from_f64(edn_term_get_floatnum(lhs) op edn_term_get_intnum(rhs));\
    }\
  } else if (edn_term_is_intnum(lhs)) {\
    if (edn_term_is_floatnum(rhs)) {\
      *res = edn_term_from_f64(edn_term_get_intnum(lhs) op edn_term_get_floatnum(rhs));\
    } else if (edn_term_is_intnum(rhs)) {\
      *res = edn_term_from_f64(edn_term_get_intnum(lhs) op edn_term_get_intnum(rhs));\
    }\
  }\
  return edn_make_err(kErrModVm, kErrInvalidPack);\
}

IMPL_ARTIH_INSTR(+, add)
IMPL_ARTIH_INSTR(-, sub)
IMPL_ARTIH_INSTR(*, mul)
IMPL_ARTIH_INSTR(/, div)


edn_err_t edn_vm_interpret(edn_vm_t* vm, edn_pack_t* pack) {
  static void* dispatch_table[opcode_count] = {
    &&do_omove, &&do_oint, &&do_oflt, &&do_ostr,
    &&do_oadd, &&do_osub, &&do_omul, &&do_odiv, &&do_oneg,
    &&do_ocall, &&do_obifcall, &&do_oret
  };

  #define stack_top_fn vm->pack->functions[vm->callstack[vm->callstack_top].functionid]
  #define stack_top_fn_id vm->callstack[vm->callstack_top].functionid
  #define stack_top_ip vm->callstack[vm->callstack_top].ip
#define dispatch(n) if (dispatch_table[stack_top_fn.bytecode[stack_top_ip + n]] == NULL) return edn_make_err(kErrModVm, kErrInvalidPack);\
  if (stack_top_ip + n >= stack_top_fn.bytecodelen) { printf("end of bytecode reached (%i)\n", stack_top_ip + n); return edn_make_err(kErrModVm, kErrNone);}\
  goto *dispatch_table[stack_top_fn.bytecode[stack_top_ip += n]]
#define op_argn(n) stack_top_fn.bytecode[stack_top_ip + n]
#define reg_argn(n) (vm->registers[op_argn(n)])

  vm->callstack_top = 0;
  vm->callstack[vm->callstack_top] = (edn_callstack_entry_t) {
    .ip = 0,
    .functionid = vm->pack->entryfuncid
  };

  dispatch(0);

  for(;;) {
    do_omove:
      {
        vm->registers[op_argn(1)] = vm->registers[op_argn(2)];
        dispatch(3);
      }
    do_oint:
      {
        vm->registers[op_argn(1)] = edn_term_from_i32(vm->pack->integers[op_argn(2)]);
        dispatch(3);
      }
    do_oflt:
      {
  	    vm->registers[op_argn(1)] = edn_term_from_f64(vm->pack->floats[op_argn(2)]);
        dispatch(3);
      }
    do_ostr:
      {
        vm->registers[op_argn(1)] = edn_term_from_str(vm->pack->strings[op_argn(2)]);
        dispatch(3);
      }
    do_oadd: {
      edn_term_t result;
      const edn_err_t err = add_terms(&reg_argn(2), &reg_argn(3), &result);
      if (!edn_err_is_ok(err)) {
        return err;
      }
      vm->registers[op_argn(1)] = result;
      dispatch(4); // opcode + dest + lhs + rhs
    }
    do_osub: {
      edn_term_t result;
      const edn_err_t err = sub_terms(&reg_argn(2), &reg_argn(3), &result);
      if (!edn_err_is_ok(err)) {
        return err;
      }
      vm->registers[op_argn(1)] = result;
      dispatch(4); // opcode + dest + lhs + rhs
    }
    do_omul: {
      edn_term_t result;
      const edn_err_t err = mul_terms(&reg_argn(2), &reg_argn(3), &result);
      if (!edn_err_is_ok(err)) {
        return err;
      }
      vm->registers[op_argn(1)] = result;
      dispatch(4); // opcode + dest + lhs + rhs
    }
    do_odiv: {
      edn_term_t result;
      const edn_err_t err = div_terms(&reg_argn(2), &reg_argn(3), &result);
      if (!edn_err_is_ok(err)) {
        return err;
      }
      vm->registers[op_argn(1)] = result;
      dispatch(4); // opcode + dest + lhs + rhs
    }
    do_oneg: {
      if (edn_term_is_intnum(&reg_argn(2))) {
        reg_argn(1) = edn_term_from_i32(-edn_term_get_intnum(&reg_argn(2)));
      } else if (edn_term_is_floatnum(&reg_argn(2))) {
        reg_argn(1) = edn_term_from_f64(-edn_term_get_floatnum(&reg_argn(2)));
      } else {
        return edn_make_err(kErrModVm, kErrInvalidPack);
      }
      dispatch(3); // opcode + dest + src
    }
    do_ocall:
      {
        const u32 arity = op_argn(1);
        const u32 fnid = op_argn(2);
        vm->callstack[vm->callstack_top].ip += (2 + arity);
        vm->callstack_top = vm->callstack_top + 1;
        vm->callstack[vm->callstack_top].functionid = fnid;
        vm->callstack[vm->callstack_top].ip = 0;
        dispatch(0);
      }
    do_obifcall:
      {
        const u32 arity = op_argn(1);
        const edn_op_t op = (edn_op_t) { .arg1 = op_argn(1), .arg2 = op_argn(2), .arg3 = op_argn(3), .arg4 = op_argn(4), .arg5 = op_argn(5), .opcode = op_argn(0) };
        edn_term_t result;
        const edn_err_t err = edn_bif_dispatch_bif(vm, op_argn(2), &op, &result);
        vm->registers[0] = result;
        if (!edn_err_is_ok(err)) return err;
        dispatch(2 + arity);
      }
    do_oret:
      {
        vm->callstack_top--;
        dispatch(0);
      }
  }

  return edn_make_err(kErrModVm, kErrNone);
}

edn_err_t edn_vm_run(edn_vm_t* vm) {
  return edn_vm_interpret(vm, vm->pack);
}
