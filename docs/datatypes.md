# Data Types
Eden provides a number of primitive data types, which are listed in this section.

Note that there are no user defined types, only composite types made up eden terms.

## Terms
A piece of data of any data type is called a term.

### Numbers
There are two types of numbers, integers and floats.
Integers are 32-bit signed integers and floats are 64-bit double-precision floats.

### Strings
Strings are represented by a 64-bit unsigned integers for their length, followed by a list of 8-bit integer charachters in UTF-8 encoding.

## Literal Encoding
Literals in bytecode are encoded the following way:

- bytes 0 and 1: table (0: int, 1: flt, 2: str)
- bytes 2 to 30: index in the table

There are 2^30 possible entries to every table.