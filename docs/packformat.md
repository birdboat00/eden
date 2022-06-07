# eden pack format

Eden executable code units are called packs. They contain metadata about the pack, tables of atoms and the functions with their code.
The pack is always saved in the little-endian format.

## String representation
String are represented by a 64-bit unsigned integer for their length, followed by their character data.

## Header
The header consists of the magic string `eDeN`.
This is followed by 4 bytes, which make up a 32-bit unsigned integer, that tell us the bytecode version.
Next are 4 bytes, which make up a 32-bit unsigned integer, that tells us the ID of the entry point function.

TODO: add pack name to the header

## Tables
Next up in the file are the atom tables. They contain the integer, float and string constants used in the pack as well as the functions of the pack.
The atoms and functions are grouped in their respecting tables and all have an integer.

The order of the tables is the following: `integers, floats, strings, functions`.

A table looks like this:
- 4 bytes, making up a 32-bit unsigned integer: number of elements in the table.
- the array of atoms
  - integers array: 32-bit signed integers
  - floats array: 64-bit floating points numbers
  - strings array: strings as described in [string representation](#string-representation)
  - functions array: see [function table](#function-table)

### Function table