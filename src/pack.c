#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "eden.h"

#define DEF_READ_FOR(T) T read_##T(FILE* f) { T val; fread(&(val), sizeof(T), 1, f); return val; }
#define DEF_WRITE_FOR(T) void write_##T(FILE* f, const T v) { fwrite(&(v), sizeof(v), 1, f); }

typedef enum table_kind {
  kTableInts, kTableFloats, kTableStrings, kTableFuncs
} table_kind_t;

DEF_WRITE_FOR(u8);
DEF_WRITE_FOR(u32);
DEF_WRITE_FOR(i32);
DEF_WRITE_FOR(usize);
DEF_WRITE_FOR(f64);

void write_string(FILE* outfile, const str string) {
  const usize len = strlen(string);
  fwrite(&len, sizeof(len), 1, outfile);
  if (len > 0) fwrite(string, sizeof(string[0]), strlen(string), outfile);
}

edn_error_t write_header(FILE* outfile, const edn_pack_t* pack) {
  fwrite(EDEN_PACK_MAGIC, sizeof(EDEN_PACK_MAGIC[0]), strlen(EDEN_PACK_MAGIC), outfile);

  write_u32(outfile, pack->target_version);
  write_string(outfile, pack->name);
  write_u32(outfile, pack->entryifuncid);

  return kErrNone;
}

edn_error_t write_table(FILE* outfile, const edn_pack_t* pack, table_kind_t table) {
  if (table == kTableInts) {
    write_u32(outfile, pack->integerslen);
    fwrite(&(pack->integers[0]), sizeof(pack->integers[0]), pack->integerslen, outfile);
  } else if (table == kTableFloats) {
    write_u32(outfile, pack->floatslen);
    fwrite(&(pack->floats[0]), sizeof(pack->floats[0]), pack->floatslen, outfile);
  } else if (table == kTableStrings) {
    write_u32(outfile, pack->stringslen);
    for (usize i = 0; i < pack->stringslen; i++) {
      write_string(outfile, pack->strings[i]);
    }
  } else if (table == kTableFuncs) {
    write_u32(outfile, pack->functionslen);
    for(usize i = 0; i < pack->functionslen; i++) {
      write_usize(outfile, pack->functions[i].bytecodelen);
      for (usize j = 0; j < pack->functions[i].bytecodelen; j++) {
        write_i32(outfile, pack->functions[i].bytecode[j]);
      }
    }
  }
  return kErrNone;
}

edn_error_t edn_write_pack(FILE* outfile, const edn_pack_t* pack) {
  if (isnull(outfile)) {
    return kErrInvalidFile;
  }
  
  edn_error_t err = write_header(outfile, pack);
  if (err != kErrNone) { return err; }

  err = write_table(outfile, pack, kTableInts);
  if (err != kErrNone) { return err; }
  err = write_table(outfile, pack, kTableFloats);
  if (err != kErrNone) { return err; }
  err = write_table(outfile, pack, kTableStrings);
  if (err != kErrNone) { return err; }

  err = write_table(outfile, pack, kTableFuncs);
  if (err != kErrNone) { return err; }

  return err;
}

DEF_READ_FOR(u8);
DEF_READ_FOR(u32);
DEF_READ_FOR(i32);
DEF_READ_FOR(usize);
DEF_READ_FOR(f64);

str read_string(FILE* file) {
  usize len = 0;
  fread(&len, sizeof(usize), 1, file);
  str string = malloc(sizeof(char) * (len + 1));
  if (string == NULL) {
    printf("failed to allocate string buffer when reading pack string.\n");
    return NULL;
  }
  usize readbytes = fread(&string[0], sizeof(char), len, file);
  string[len] = '\0';
  return string;
}

edn_error_t read_integers_table(FILE* file, u32* out_tablelen, i32** out_tabledata) {
  assert(out_tablelen != NULL);
  assert(out_tabledata != NULL);
  assert(file != NULL);

  const u32 len = read_u32(file);

  i32* table = malloc(sizeof(i32) * len);
  if (table == NULL) {
    return kErrMallocFail;
  }

  if (fread(&table[0], sizeof(i32), len, file) < len) {
    return kErrInvalidPack;
  }

  *out_tabledata = table;
  *out_tablelen = len;
  return kErrNone;
}

edn_error_t read_f64_table(FILE* file, u32* out_tablelen, f64** out_tabledata) {
  assert(out_tablelen != NULL);
  assert(out_tabledata != NULL);
  assert(file != NULL);

  const u32 len = read_u32(file);

  f64* table = malloc(sizeof(f64) * len);
  if (table == NULL) {
    return kErrMallocFail;
  }

  if (fread(&table[0], sizeof(f64), len, file) < len) {
    return kErrInvalidPack;
  }

  *out_tabledata = table;
  *out_tablelen = len;
  return kErrNone;
}

edn_error_t read_strings_table(FILE* file, u32* out_tablelen, str** out_tabledata) {
  assert(out_tablelen != NULL);
  assert(out_tabledata != NULL);
  assert(file != NULL);

  const u32 len = read_u32(file);

  str* table = malloc(sizeof(str) * len);
  if (table == NULL) {
    return kErrMallocFail;
  }

  for (usize i = 0; i < len; i++) {
    table[i] = read_string(file);
  }

  *out_tabledata = table;
  *out_tablelen = len;
  return kErrNone;
}

edn_error_t read_functions_table(FILE* file, u32* out_tablelen, edn_function_t** out_tabledata) {
  assert(out_tablelen != NULL);
  assert(out_tabledata != NULL);
  assert(file != NULL);

  const u32 len = read_u32(file);

  edn_function_t* table = malloc(sizeof(edn_function_t) * len);
  if (table == NULL) {
    return kErrMallocFail;
  }
  
  for (usize i = 0; i < len; i++) {
    table[i].bytecodelen = read_usize(file);
    table[i].bytecode = malloc(sizeof(edn_bytecode_t) * table[i].bytecodelen);
    if (table[i].bytecode == NULL) {
      return kErrMallocFail;
    }
    for (usize j = 0; j < table[i].bytecodelen; j++) {
      table[i].bytecode[j] = read_i32(file);
    }
  }

  *out_tabledata = table;
  *out_tablelen = len;
  return kErrNone;
}

edn_pack_t edn_read_pack(FILE* infile) {

  edn_pack_t pack = {
    .target_version = 0,
    .floatslen = 0,
    .integerslen = 0,
    .stringslen = 0,
    .functionslen = 0,
    .name = "@no.name@"
  };

  char magic_buf[5];
  magic_buf[5] = '\0';
  fread(&magic_buf[0], sizeof(EDEN_PACK_MAGIC[0]), 4, infile);
  if (strcmp(&magic_buf[0], EDEN_PACK_MAGIC) != 0) {
    printf("file is not a pack file.\n");
    return pack;
  }

  pack.target_version = read_u32(infile);
  pack.name = read_string(infile);
  pack.entryifuncid = read_u32(infile);

  read_integers_table(infile, &(pack.integerslen), &(pack.integers));
  read_f64_table(infile, &(pack.floatslen), &(pack.floats));
  read_strings_table(infile, &(pack.stringslen), &(pack.strings));
  read_functions_table(infile, &(pack.functionslen), &(pack.functions));

  return pack;
}