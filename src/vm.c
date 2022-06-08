#include "eden.h"

#include <stdlib.h>

#define loop for(;;)

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

void bif_call(u32 fnid, edn_pack_t* pack, const edn_function_t* fn, edn_reg_t* regs, const edn_op_t* op) {
  switch (fnid) {
    case 0x01:
      const edn_reg_t srcreg = regs[op->arg3];
      switch (srcreg.type) {
        case integer: printf("%i", srcreg.data.i); break;
        case floating: printf("%f", srcreg.data.f); break;
        case string: printf("%s", srcreg.data.s); break;
        default: printf("%i", srcreg.data); break;
      }
      break;
    default:
      return;
  }
}

edn_error_t edn_vm_interpret(edn_vm_t* vm, edn_pack_t* pack) {
  static void* dispatch_table[opcode_count];
  dispatch_table[omov] = &&do_omov;
  dispatch_table[oint] = &&do_oint;
  dispatch_table[oflt] = &&do_oflt;
  dispatch_table[ostr] = &&do_ostr;
  dispatch_table[ocall] = &&do_ocall;
  dispatch_table[obifcall] = &&do_obifcall;
  dispatch_table[oret] = &&do_oret;

  #define stack_top_fn vm->pack->functions[vm->callstack[vm->callstack_top].functionid]
  #define stack_top_fn_id vm->callstack[vm->callstack_top].functionid
  #define stack_top_ip vm->callstack[vm->callstack_top].ip
#define dispatch(n) if (dispatch_table[stack_top_fn.bytecode[stack_top_ip + n]] == NULL) return kErrInvalidPack;\
  if (stack_top_ip + n >= stack_top_fn.bytecodelen) { printf("end of bytecode reached (%i)\n", stack_top_ip + n); return kErrNone;}\
  goto *dispatch_table[stack_top_fn.bytecode[stack_top_ip += n]]
#define op_argn(n) stack_top_fn.bytecode[stack_top_ip + n]

  vm->callstack_top = 0;
  vm->callstack[vm->callstack_top] = (edn_callstack_entry_t) {
    .ip = 0,
    .functionid = vm->pack->entryfuncid
  };

  dispatch(0);

  loop {
    do_omov:
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
    do_ocall:
      {
        const u32 arity = op_argn(1);
        const u32 calledid = op_argn(2);
        vm->callstack[vm->callstack_top].ip += (2 + arity);
        vm->callstack_top = vm->callstack_top + 1;
        vm->callstack[vm->callstack_top].functionid = calledid;
        vm->callstack[vm->callstack_top].ip = 0;
        dispatch(0);
      }
    do_obifcall:
      {
        const u32 arity = op_argn(1);
        const u32 bifid = op_argn(2);
        const edn_op_t op = (edn_op_t) { .arg1 = op_argn(1), .arg2 = op_argn(2), .arg3 = op_argn(3), .opcode = op_argn(0) };
        bif_call(bifid, vm->pack, &vm->pack->functions[vm->callstack[vm->callstack_top].functionid], vm->registers, &op);
        dispatch(2 + arity);
      }
    do_oret:
      {
        vm->callstack_top--;
        dispatch(0);
      }
  }
}

edn_error_t edn_run_vm(edn_vm_t* vm) {
  return edn_vm_interpret(vm, vm->pack);
}