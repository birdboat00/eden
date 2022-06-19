# NIFs

NIFs are native functions for the eden vm implemented in c++, that
can be called from eden bytecode using the `nifcallnamed` opcode.

They are implemented as dynamic link library.

They can be loaded the the eden vm using the `-n` command line
paremeter followed by a list of NIF DLLs.

Those DLLs need to have a C function called `edn_nif_entry(vm* vm)`

## Signature