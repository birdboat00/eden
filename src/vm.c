#include "eden.h"

edn_vm_t edn_make_vm(const edn_pack_t* pack, const edn_vm_params_t params) {
  edn_vm_t vm = {
    .pack = pack,
    .params = params
  };

  if (vm.params.verbose) {
    printf("eden vm is verbose.\n");
  }

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

edn_error_t edn_interpret_fn(edn_vm_t* vm, edn_pack_t* pack, edn_function_t* fn) {
  edn_reg_t* regs = vm->registers;
  for (size_t i = 0; i < fn->bytecodelen; i++) {
    const edn_op_t op = {
      .opcode = fn->bytecode[i],
      .arg1 = (i + 1 < fn->bytecodelen) ? fn->bytecode[i + 1] : 0,
      .arg2 = (i + 2 < fn->bytecodelen) ? fn->bytecode[i + 2] : 0,
      .arg3 = (i + 3 < fn->bytecodelen) ? fn->bytecode[i + 3] : 0
    };

    switch (op.opcode) {
      case omov: regs[op.arg1] = regs[op.arg2]; break;
      case oint: regs[op.arg1] = (edn_reg_t) { .data.i = pack->integers[op.arg2], .type = integer }; break;
      case oflt: regs[op.arg1] = (edn_reg_t) { .data.f = pack->floats[op.arg2], .type = floating }; break;
      case ostr: regs[op.arg1] = (edn_reg_t) { .data.s = pack->strings[op.arg2], .type = string }; break;
      case oadd: {
        if (regs[op.arg2].type == integer && regs[op.arg3].type == integer) {
          regs[op.arg1].type = integer;
          regs[op.arg1].data.i = regs[op.arg2].data.i + regs[op.arg3].data.i; 
        } else if (regs[op.arg2].type == floating && regs[op.arg3].type == floating) {
          regs[op.arg1].type = integer;
          regs[op.arg1].data.f = regs[op.arg2].data.f + regs[op.arg3].data.f; 
        }
      }
      case obifcall: {
        bif_call(op.arg2, pack, fn, &regs, &op);
      } break;
      case ocall: {
        vm->callstack_top++;
        edn_error_t res = edn_interpret_fn(vm, pack, &pack->functions[op.arg2]);
        if (res != kErrNone) return res;
      } break;
      case oret: {
        vm->callstack_top--;
        return kErrNone;
      } break;
      default: 
        printf("invalid opcode: %i", op.opcode);
        return kErrInvalidPack;
    }

    i = i + edn_op_arity(op.opcode, op.arg1);
  }
}

edn_error_t edn_run_vm(edn_vm_t* vm) {
  printf("Running vm...\n");

  edn_interpret_fn(vm, vm->pack, &vm->pack->functions[vm->pack->entryfuncid]);

  return kErrNone;
}