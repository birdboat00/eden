# Contributing
eden is open to any contribtion.

Before contributing please read the following guidelines.

Please also make sure you understand the [0BSD License](LICENSE).

## Git Recommended Practises
- Commit message should have a summary and description
- Avoid trailing whitespaces
- Always `git pull --rebase`
- All your intermediate commits should build

## Coding Style
```c
void f(int reverse) {
  if (reverse) {
    puts("!dlroW olleH");
  } else {
    puts("Hello World!");
  }
}
```
### Names
- All names are snake_case
- Types are always postfixed with `_t`

### Other Conventions
- Pointer * should be with the type (e.g. `char* name`)
- Avoid long lines
- Use intermediate variables with meaningful names