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

edn_error_t edn_vm_interpret(edn_vm_t* vm, edn_pack_t* pack) {
  static void* dispatch_table[opcode_count] = {
    &&do_omove, &&do_oint, &&do_oflt, &&do_ostr,
    &&do_oadd, &&do_osub, &&do_omul, &&do_odiv, &&do_oneg,
    &&do_ocall, &&do_obifcall, &&do_oret
  };

  #define stack_top_fn vm->pack->functions[vm->callstack[vm->callstack_top].functionid]
  #define stack_top_fn_id vm->callstack[vm->callstack_top].functionid
  #define stack_top_ip vm->callstack[vm->callstack_top].ip
#define dispatch(n) if (dispatch_table[stack_top_fn.bytecode[stack_top_ip + n]] == NULL) return kErrInvalidPack;\
  if (stack_top_ip + n >= stack_top_fn.bytecodelen) { printf("end of bytecode reached (%i)\n", stack_top_ip + n); return kErrNone;}\
  goto *dispatch_table[stack_top_fn.bytecode[stack_top_ip += n]]
#define op_argn(n) stack_top_fn.bytecode[stack_top_ip + n]

#define artih_instruction(mathop) const bool isfloat = vm->registers[op_argn(2)].type == floating || vm->registers[op_argn(3)].type == floating;\
        vm->registers[op_argn(1)].type = isfloat ? floating : integer;\
        if (isfloat) vm->registers[op_argn(1)].data.f = vm->registers[op_argn(2)].data.f mathop vm->registers[op_argn(3)].data.f;\
        else vm->registers[op_argn(1)].data.i = vm->registers[op_argn(2)].data.i mathop vm->registers[op_argn(3)].data.i;\
        dispatch(4);

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
        vm->registers[op_argn(1)].data.i = vm->pack->integers[op_argn(2)];
        vm->registers[op_argn(1)].type = integer;
        dispatch(3);
      }
    do_oflt:
      {
  	    vm->registers[op_argn(1)].data.f = vm->pack->floats[op_argn(2)];
        vm->registers[op_argn(1)].type = floating; 
        dispatch(3);
      }
    do_ostr:
      {
        vm->registers[op_argn(1)].data.s = vm->pack->strings[op_argn(2)];
        vm->registers[op_argn(1)].type = string;
        dispatch(3);
      }
    do_oadd: { artih_instruction(+); }
    do_osub: { artih_instruction(-); }
    do_omul: { artih_instruction(*); }
    do_odiv: { artih_instruction(/); }
    do_oneg: {
      vm->registers[op_argn(1)].type = vm->registers[op_argn(2)].type;
      if (vm->registers[op_argn(2)].type == floating) {
        vm->registers[op_argn(1)].data.f = -vm->registers[op_argn(2)].data.f;
      } else if (vm->registers[op_argn(2)].type == integer) {
        vm->registers[op_argn(1)].data.i = -vm->registers[op_argn(2)].data.i;
      } else {
        return kErrInvalidPack;
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
        const edn_error_t err = edn_bif_dispatch_bif(vm, op_argn(2), &op);
        if (err != kErrNone) return err;
        dispatch(2 + arity);
      }
    do_oret:
      {
        vm->callstack_top--;
        dispatch(0);
      }
  }

  return kErrNone;
}

edn_error_t edn_run_vm(edn_vm_t* vm) {
  return edn_vm_interpret(vm, vm->pack);
}
