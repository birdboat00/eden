#include "builtin_test_pack.h"

#define r(n) n
#define c(c) c
#define i(i) i

#define opri(op, reg, id) op, r(reg), i(id),
#define oprrr(op, dest, src1, src2) op, r(dest), r(src1), r(src2),
#define opccr(op, const1, const2, reg) op, c(const1), c(const2), r(reg),
#define opccrr(op, const1, const2, reg1, reg2) op, c(const1), c(const2), r(reg1), r(reg2),

edn_pack_t create_test_pack() {

  static edn_bytecode_t bytecode[] = {
    opri(oint, 0, 0)
    opri(oint, 1, 1)
    oprrr(oadd, 2, 0, 1)
    opri(ostr, 3, 3)
    opccr(obifcall, 2, 0x01, 2)
    opccr(obifcall, 2, 0x01, 3)
    opri(ostr, 0, 0)
    opccr(obifcall, 2, 0x01, 0)
    opccr(obifcall, 2, 0x01, 3)
  };
  
  static const edn_function_t main_func = {
    .bytecode = &bytecode[0],
    .bytecodelen = arraylen(edn_bytecode_t, bytecode)
  };

  static i32 ints[] = { 10, 20, 40 };
  static f64 floats[] = { -1.2, 1.0, 10.23 };
  static str strings[] = { "gamma", "beta", "alpha", "\n" };

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

    .functions = &main_func,
    .functionslen = 1
  };

  return pack;
}