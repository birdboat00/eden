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
    "  version --version  -v    Print version information and exit\n"
    "  help    --help     -h    Print this help message and exit\n\n"
    "Temporary Commands:\n\n"
    "  --dumptestpack           Dumps the integrated test pack to dump.txt\n"
    "  --savetestpack           Exports the integrated test pack to code.eden\n"
    "  --readtestpack           Reads the code.eden pack file\n"
    "  --interptestpack         Interpret the integrated test pack\n");
}

int main(u32 argc, str* argv) {
  if (has_arg("version", argc, argv) || has_arg("--version", argc, argv) || has_arg("-v", argc, argv)) {
    print_version();
    return kErrNone;
  } else if (argc == 1 || has_arg("help", argc, argv) || has_arg("--help", argc, argv) || has_arg("-h", argc, argv)) {
    print_version();
    print_usage();
    return kErrNone;
  }

  const edn_pack_t test_pack = create_test_pack();

  if (has_arg("--dumptestpack", argc, argv)) {
    FILE* file = fopen("dump.txt", "w");
    edn_dump_pack(isnull(file) ? stdout : file, &test_pack);
    fclose(file);
    return kErrNone;
  } else if (has_arg("--savetestpack", argc, argv)) {
    FILE* outfile = fopen("code.eden", "wb");
    edn_dump_pack(stdout, &test_pack);
    edn_error_t err = edn_write_pack(outfile, &test_pack);
    if (err != kErrNone) printf("failed to write pack: %i\n", err);
    fclose(outfile);
    return err;
  } else if (has_arg("--readtestpack", argc, argv)) {
    FILE* infile = fopen("code.eden", "rb");
    edn_pack_t pack;
    edn_error_t err = edn_read_pack(infile, &pack);
    if (err == kErrNone) edn_dump_pack(stdout, &pack);
    else printf("Error while reading pack: %i\n", err);
    fclose(infile);
    return err;
  }

  edn_vm_t vm = edn_make_vm(&test_pack, (edn_vm_params_t) { .verbose = 1 });

  return edn_run_vm(&vm);
}