#include "builtin_test_pack.h"

#include <stdlib.h>

#define r(n) n
#define c(c) c
#define i(i) i

#define opri(op, reg, id) op, r(reg), i(id),
#define oprrr(op, dest, src1, src2) op, r(dest), r(src1), r(src2),
#define opccr(op, const1, const2, reg) op, c(const1), c(const2), r(reg),
#define opccrr(op, const1, const2, reg1, reg2) op, c(const1), c(const2), r(reg1), r(reg2),

edn_pack_t create_test_pack() {

  static edn_bytecode_t bytecode[] = {
    ostr, r(0), i(0), // load str 0 into r0
    ostr, r(1), i(1), // load str 1 into r1
    ostr, r(2), i(2), // load str 2 into r2
    ocall, 1, 0x01,   // call putStrLn (0x01) with no argument -> chooses string to print from r0
    omove, r(0), r(1), // move r1 into r0
    ocall, 1, 0x01,   // call putStrLn (0x01) with no argument -> chooses string to print from r0
    omove, r(0), r(2), // move r2 into r0
    ocall, 1, 0x01,   // call putStrLn (0x01) with no argument -> chooses string to print from r0
  };
  
  static const edn_function_t main_func = {
    .bytecode = &bytecode[0],
    .bytecodelen = arraylen(edn_bytecode_t, bytecode)
  };

  static edn_bytecode_t bytecode_putStrLn[] = {
    opccr(obifcall, 2, 0x01, 0) // bifcall printreg
    opri(ostr, 4, 3) // load newline 
    opccr(obifcall, 2, 0x01, 4) // bifcall printreg newline
    oret // return
  };

  static const edn_function_t putStrLn_func = {
    .bytecode = &bytecode_putStrLn[0],
    .bytecodelen = arraylen(edn_bytecode_t, bytecode_putStrLn)
  };

  static i32 ints[] = { 10, 20, 40 };
  static f64 floats[] = { -1.2, 1.0, 10.23 };
  static str strings[] = { "gamma", "beta", "alpha", "\n" };
  edn_function_t* funcs = malloc(sizeof(edn_function_t) * 2);
  funcs[0] = main_func;
  funcs[1] = putStrLn_func;

  edn_pack_t pack = {
    .name = "edn_bitp",
    .target_version = EDEN_BYTECODE_VERSION,
    .entryfuncid = 0,
    .integers = &ints[0],
    .integerslen = arraylen(i32, ints),
    .floats = &floats[0],
    .floatslen = arraylen(f64, floats),
    .strings = &strings[0],
    .stringslen = arraylen(str, strings),

    .functions = &funcs[0],
    .functionslen = 2
  };

  return pack;
}