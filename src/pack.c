#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "eden.h"

#define DEF_READ_FOR(T) T read_##T(FILE* f) { T val; fread(&(val), sizeof(T), 1, f); return val; }
#define DEF_WRITE_FOR(T) void write_##T(FILE* f, const T v) { fwrite(&(v), sizeof(v), 1, f); }
#define DEF_RW_FOR(T) DEF_WRITE_FOR(T); \
  	                  DEF_READ_FOR(T);

DEF_RW_FOR(u8);
DEF_RW_FOR(u16);
DEF_RW_FOR(u32);
DEF_RW_FOR(i32);
DEF_RW_FOR(usize);
DEF_RW_FOR(f64);

#define DEF_TABLE_WRITE_PRIM(tablename, tablelenname) edn_error_t write_##tablename##_table(FILE* f, const edn_pack_t* pack) {\
  if(pack->tablelenname > 0) { \
    write_u32(f, pack->tablelenname); \
    fwrite(&(pack->tablename[0]), sizeof(pack->tablename[0]), pack->tablelenname, f); \
  } \
  return kErrNone; \
}

DEF_TABLE_WRITE_PRIM(integers, integerslen);
DEF_TABLE_WRITE_PRIM(floats, floatslen);

#define DEF_TABLE_READ_PRIM(type, tablename) edn_error_t read_##tablename##_table(FILE* f, u32* out_tablelen, type** out_tabledata) { \
  assert(out_tablelen != NULL); \
  assert(out_tabledata != NULL); \
  assert(f != NULL); \
  const u32 len = read_u32(f); \
  type* table = malloc(sizeof(type) * len); \
  if (table == NULL) { return kErrMallocFail; } \
  if (fread(&table[0], sizeof(type), len, f) < len) { return kErrInvalidPack; } \
  *out_tabledata = table; \
  *out_tablelen = len; \
  return kErrNone; \
}

DEF_TABLE_READ_PRIM(i32, integers);
DEF_TABLE_READ_PRIM(f64, floats);

void write_string(FILE* outfile, const str string) {
  const usize len = strlen(string);
  write_usize(outfile, len);
  if (len > 0) fwrite(string, sizeof(string[0]), len, outfile);
}

edn_error_t write_strings_table(FILE* file, const edn_pack_t* pack) {
  if (pack->stringslen > 0) {
    write_u32(file, pack->stringslen);
    for (usize i = 0; i < pack->stringslen; i++) {
      write_string(file, pack->strings[i]);
    }
  }
  return kErrNone;
}

edn_error_t write_functions_table(FILE* file, const edn_pack_t* pack) {
  printf("Writing functions table with length: %u", pack->functionslen);
  write_u32(file, pack->functionslen);
  for(usize i = 0; i < pack->functionslen; i++) {
    write_usize(file, pack->functions[i].bytecodelen);
    for (usize j = 0; j < pack->functions[i].bytecodelen; j++) {
      write_i32(file, pack->functions[i].bytecode[j]);
    }
  }
  return kErrNone;
}

edn_error_t write_header(FILE* outfile, const edn_pack_t* pack) {
  fwrite(EDEN_PACK_MAGIC, sizeof(EDEN_PACK_MAGIC[0]), strlen(EDEN_PACK_MAGIC), outfile);

  write_u16(outfile, pack->target_version);
  write_string(outfile, pack->name);
  write_u32(outfile, pack->entryfuncid);

  uint8_t tables = 0;
  if (pack->integerslen > 0) bit_set(tables, 0);
  if (pack->floatslen > 0) bit_set(tables, 1);
  if (pack->stringslen > 0) bit_set(tables, 2);
  if (pack->functionslen > 0) bit_set(tables, 3);
  write_u8(outfile, tables); 

  return kErrNone;
}

edn_error_t edn_write_pack(FILE* file, const edn_pack_t* pack) {
  if (isnull(file)) {
    return kErrInvalidFile;
  }
  
  edn_error_t err = write_header(file, pack);
  if (err != kErrNone) { return err; }

  err = write_integers_table(file, pack);
  if (err != kErrNone) { return err; }
  
  err = write_floats_table(file, pack);
  if (err != kErrNone) { return err; }

  err = write_strings_table(file, pack);
  if (err != kErrNone) { return err; }

  err = write_functions_table(file, pack);

  return err;
}

str read_string(FILE* file) {
  const usize len = read_usize(file);
  str string = malloc(sizeof(char) * (len + 1));
  if (string == NULL) {
    printf("failed to allocate string buffer when reading pack string.\n");
    return string;
  }
  fread(&string[0], sizeof(char), len, file);
  string[len] = '\0';
  return string;
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

edn_error_t edn_read_pack(FILE* infile, edn_pack_t* out_pack) {
  char magic_buf[EDEN_PACK_MAGIC_LEN + 1];
  magic_buf[EDEN_PACK_MAGIC_LEN] = '\0';
  fread(&magic_buf[0], sizeof(EDEN_PACK_MAGIC[0]), EDEN_PACK_MAGIC_LEN, infile);
  if (strcmp(&magic_buf[0], EDEN_PACK_MAGIC) != 0) {
    printf("file is not a pack file.\n");
    return kErrInvalidPack;
  }

  (*out_pack).target_version = read_u16(infile);
  if (out_pack->target_version != EDEN_BYTECODE_VERSION) {
    printf("pack target version (%u) is different from eden vm bytecode version (%u).\n", out_pack->target_version, EDEN_BYTECODE_VERSION);
    return kErrInvalidPack;
  }

  (*out_pack).name = read_string(infile);
  (*out_pack).entryfuncid = read_u32(infile);

  uint8_t tables = read_u8(infile);

  if (bit_check(tables, 0)) {
    edn_error_t err = read_integers_table(infile, &((*out_pack).integerslen), &((*out_pack).integers));
    if (err != kErrNone) return err;
  }

  if (bit_check(tables, 1)) {
    edn_error_t err = read_floats_table(infile, &((*out_pack).floatslen), &((*out_pack).floats));
    if (err != kErrNone) return err;
  }

  if (bit_check(tables, 2)) {
    edn_error_t err = read_strings_table(infile, &((*out_pack).stringslen), &((*out_pack).strings));
    if (err != kErrNone) return err;
  }
  
  if (bit_check(tables, 3)) {
    edn_error_t err = read_functions_table(infile, &((*out_pack).functionslen), &((*out_pack).functions));
    if (err != kErrNone) return err;
  }

  return kErrNone;
}

// TODO: fix all of this shitcode.
void edn_dump_pack(FILE* stream, const edn_pack_t* pack) {
  fprintf(stream, "--- BEGIN pack DUMP ---\n");

  fprintf(stream, "pack\n  name-> %s\n  targetversion-> %i\n  entryfuncid-> %i\n", pack->name, pack->target_version, pack->entryfuncid);
  
  fprintf(stream, "tables:\nintegers (count: %i)\n", pack->integerslen);
  for (size_t i = 0; i < pack->integerslen; i++) {
    fprintf(stream, "  @%llu -> %i\n", i, pack->integers[i]);
  }

  fprintf(stream, "floats (count: %i)\n", pack->floatslen);
  for (size_t i = 0; i < pack->floatslen; i++) {
    fprintf(stream, "  @%llu -> %f\n", i, pack->floats[i]);
  }

  fprintf(stream, "strings (count: %i)\n", pack->stringslen);
  for(size_t i = 0; i < pack->stringslen; i++) {
    fprintf(stream, "  @%llu -> \"%s\"\n", i, pack->strings[i]);
  }
  
  fprintf(stream, "functions (count: %i)\n", pack->functionslen);
  for (usize i = 0; i < pack->functionslen; i++) {
    const edn_function_t* fn = &pack->functions[i];
    fprintf(stream, "  @%llu -> (%llu){\n", i, fn->bytecodelen);
    for(size_t j = 0; j < fn->bytecodelen; j++) {
      const usize bclen = fn->bytecodelen;
      const str opcode = edn_opcode_to_str(fn->bytecode[j]);
      u32 arity = edn_op_arity(fn->bytecode[j], (j + 1 < bclen) ? fn->bytecode[j + 1] : 0);
      const edn_op_t op = (edn_op_t) {
        .opcode = fn->bytecode[j],
        .arg1 = (j + 1 < bclen) ? fn->bytecode[j + 1] : 0,
        .arg2 = (j + 2 < bclen) ? fn->bytecode[j + 2] : 0,
        .arg3 = (j + 3 < bclen) ? fn->bytecode[j + 3] : 0,
        .arg4 = (j + 4 < bclen) ? fn->bytecode[j + 4] : 0,
        .arg5 = (j + 5 < bclen) ? fn->bytecode[j + 5] : 0
      };

      char buf[128];
      edn_op_to_str(op, buf, 128);
      fprintf(stream, "    %s\n", buf);
      j += arity;
    }
    fprintf(stream, "  }\n");
  }
  
  fprintf(stream, "--- END pack DUMP ---\n");
}