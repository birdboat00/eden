# BIFs

BIFs are Builtin functions, implemented as NIFs (native functions) and automatically loaded when the vm is started and pack is loaded.

All BIFs return their return value to register `r0`. Any content in
that register is overwritten.

## edn_bif_printreg/1 (register)
Prints the VM register `register` to the console.
This uses the `term::to_str(term)` function.

Returns int term 0 if the printing was successful, the number of the error otherwise.

## edn_bif_getpackname/0
Returns the name of the currently loaded and executed pack as a string term.

## edn_bif_halt/1 (register)
Terminates the currently running VM instance and returns the
integer value of the `register` to the OS as exit reason.

## edn_bif_splus/2 (register, register)
## edn_bif_sminus/2 (register, register)
## edn_bif_smul/2 (register, register)
## edn_bif_sdiv/2 (register, register)
Does the corresponding artihmetics on the two registers and returns
the result.
All methods support i64 and f64 values. They can be mixed.
s_plus additionally allows str values which concatenate the strings.

> edn_bif_sdiv/2 currently does not check for division-by-zero

## edn_bif_neg/1 (register)
Negates the i64 or f64 value of the `register` and returns the result.

## edn_bif_is_int/1 (register)
Check if the register term is of type i64.
Returns 0 if false, 1 if true.

## edn_bif_is_flt/1 (register)
Check if the register term is of type f64.
Returns 0 if false, 1 if true.

## edn_bif_is_str/1 (register)
Check if the register term is of type str.
Returns 0 if false, 1 if true.

## edn_bif_unixtime/0
Returns the unix timestamp as i64 term.