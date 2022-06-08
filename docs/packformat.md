# eden pack format

Eden executable code units are called packs. They contain metadata about the pack, tables of atoms and the functions with their code.
The pack is always saved in the little-endian format.

## String representation
String are represented by a 64-bit unsigned integer for their length, followed by their character data.

## Header
The header consists of the magic string `eDeNPACK`.
This is followed by 2 bytes, which make up a 16-bit unsigned integer, that tell us the bytecode version.
After this comes the name of the pack encoded as a [string as described above](#string-representation).
Next are 4 bytes, which make up a 32-bit unsigned integer, that tells us the ID of the entry point function.

After this there is an 8-bit unsigned integer that holds a bitset on wether a table exists:
- bit 1: integer table
- bit 2: floats table
- bit 3: strings table
- bit 4: functions table

If the corresponding bit of the table is set to 0 the table does not exists. There is no length or table data written to the pack file and reading the table must be skipped.

## Tables
Next up in the file are the atom tables. They contain the integer, float and string constants used in the pack as well as the functions of the pack.
The atoms and functions are grouped in their respecting tables and all have an integer.

The order of the tables is the following: `integers, floats, strings, functions`.

A table looks like this:
- 4 bytes, making up a 32-bit unsigned integer: number of elements in the table.
- the array of atoms, either one of those
  - integers array: 32-bit signed integers
  - floats array: 64-bit floating points numbers
  - strings array: strings as described in [string representation](#string-representation)
  - functions array: see [function table](#function-table)

### Function table