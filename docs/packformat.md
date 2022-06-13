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

# New Pack Format
eDeNPACK (bytes)
pack-version (u16)
nodes (list)

## Encoding
Everything is encoded in little-endian byteorder.
### Primitives
Primitives are encoded in their bytes.
### Strings
Strings are encoded by a usize for their length followed by their byte data. They are not null-terminated. Length is determined by the length part.

## Nodes
|Length in bytes|Type  |Description  |
|---------------|------|-------------|
|1              |u8    |Type of Entry|
|4              |u32   |ID of Entry  |
|var            |str   |Name of Entry|

### packinfo (0x00)
|Length in bytes|Type  |Description     |
|---------------|------|----------------|
|2              |u16   |bytecode-version|
|var            |str   |pack name       |

### inttable (0x01)
|Length in bytes|Type  |Description  |
|---------------|------|-------------|
|8              |usize |table entries|
|table-entries  |i64   |integers     |

### flttable (0x02)
|Length in bytes|Type  |Description  |
|---------------|------|-------------|
|8              |usize |table entries|
|table-entries  |f64   |floats       |

### strtable (0x03)
|Length in bytes|Type  |Description  |
|---------------|------|-------------|
|8              |usize |table entries|
|table-entries  |str   |strings      |

### fnstable (0x04)
|Length in bytes|Type  |Description  |
|---------------|------|-------------|
|8              |usize |table entries|
|table-entries  |u32   |fn entry id  |

### fn (0x05)
|Length in bytes|Type  |Description  |
|---------------|------|-------------|
|var            |str   |fn name      |
|8              |usize |bytecode len |