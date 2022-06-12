#include <stdio.h>
#include <string.h>

#include "eden.h"
#include "builtin_test_pack.h"

bool has_arg(const str arg, int argc, str* argv) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], arg) == 0) return 1;
  }
  return 0;
}

void print_version() {
  printf("eden %s [bytecode: %u] (compiled %s)\n", EDEN_VERSION, EDEN_BYTECODE_VERSION, EDEN_BUILD_TIME);
}

void print_usage() {
  printf("Usage: eden command [options]\n\n"
    "Commands:\n\n"
    "  <filename.eden>          Interpret file <filename.eden> and exit\n"
    "  version --version  -v    Print version information and exit\n"
    "  help    --help     -h    Print this help message and exit\n\n"
    "Temporary Commands:\n\n"
    "  --dumptestpack           Dumps the integrated test pack to dump.txt\n"
    "  --savetestpack           Exports the integrated test pack to code.eden\n"
    "  --readtestpack           Reads the code.eden pack file\n");
}

int main(u32 argc, str* argv) {

  if (argc <= 1) {
    print_usage();
    return 0;
  }

  if (has_arg("version", argc, argv) || has_arg("--version", argc, argv) || has_arg("-v", argc, argv)) {
    print_version();
    return 0;
  } else if (argc == 1 || has_arg("help", argc, argv) || has_arg("--help", argc, argv) || has_arg("-h", argc, argv)) {
    print_version();
    print_usage();
    return 0;
  }

  if (has_arg("--dumptestpack", argc, argv)) {
    FILE* file = fopen("dump.txt", "w");
    edn_pack_t test_pack = create_test_pack();
    edn_pack_dump(isnull(file) ? stdout : file, &test_pack);
    fclose(file);
    return 0;
  } else if (has_arg("--savetestpack", argc, argv)) {
    FILE* outfile = fopen("code.eden", "wb");
    edn_pack_t test_pack = create_test_pack();
    edn_pack_dump(stdout, &test_pack);
    edn_err_t err = edn_pack_write(outfile, &test_pack);
    if (!edn_err_is_ok(err)) printf("failed to write pack: %i\n", err);
    fclose(outfile);
    return err.kind;
  } else if (has_arg("--readtestpack", argc, argv)) {
    FILE* infile = fopen("code.eden", "rb");
    edn_pack_t pack;
    edn_err_t err = edn_pack_read(infile, &pack);
    if (edn_err_is_ok(err)) edn_pack_dump(stdout, &pack);
    else printf("Error while reading pack: %i\n", err);
    fclose(infile);
    return err.kind;
  }

  if (argc == 2) {
    FILE* file = fopen(argv[1], "rb");
    if (isnull(file)) {
      printf("failed to open file '%s'.\n", argv[1]);
      return kErrInvalidFile;
    }

    edn_pack_t pack;
    const edn_err_t err = edn_pack_read(file, &pack);
    if (!edn_err_is_ok(err)) {
      printf("failed to read pack file: %s\n", edn_err_to_str(&err));
      return err.kind;
    }

    edn_vm_t* vm = edn_make_vm(&pack, (edn_vm_params_t) { .verbose = 1 });
    if (isnull(vm)) {
      printf("failed to allocate vm memory.\n");
      return kErrMallocFail;
    }

    const edn_err_t runerr = edn_vm_run(vm);
    if (!edn_err_is_ok(runerr)) {
      printf("VM ERROR - dumping register...\n");
      for(usize i = 0; i < arraylen(edn_reg_t, vm->registers); i++) {
        char buf[256];
        edn_term_to_str(&(vm->registers[i]), buf, 256);
        printf("r%i = %s\n", i, buf);
      }
    }

    return runerr.kind;
  }

  return 0;
}