#include "eden.h"

#include <string.h>

typedef enum table_kind {
  kTableInts, kTableFloats, kTableStrings, kTableFuncs
} table_kind_t;

edn_error_t write_header(FILE* outfile, const edn_pack_t* pack) {
  fwrite(EDEN_PACK_MAGIC, sizeof(EDEN_PACK_MAGIC[0]), strlen(EDEN_PACK_MAGIC), outfile);

  fwrite(&(pack->target_version), sizeof(pack->target_version), 1, outfile);
  // TODO: add back in name writing
  // fwrite(&pack->name, sizeof(pack->name[0]), strlen(pack->name), outfile);
  fwrite(&(pack->entryifuncid), sizeof(pack->entryifuncid), 1, outfile);

  return kErrNone;
}

edn_error_t write_table(FILE* outfile, const edn_pack_t* pack, table_kind_t table) {
  if (table == kTableInts) {
    fwrite(&(pack->integerslen), sizeof(pack->integerslen), 1, outfile);
    fwrite(&(pack->integers), sizeof(pack->integers[0]), pack->integerslen, outfile);
  } else if (table == kTableFloats) {
    fwrite(&pack->floatslen, sizeof(pack->floatslen), 1, outfile);
    fwrite(&pack->floats, sizeof(pack->floats[0]), pack->floatslen, outfile);
  } else if (table == kTableStrings) {
    fwrite(&pack->stringslen, sizeof(pack->stringslen), 1, outfile);
    for (usize i = 0; i < pack->stringslen; i++) {
      fwrite(&pack->strings[i], sizeof(pack->strings[i]), strlen(pack->strings[i]), outfile);
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

edn_pack_t edn_read_pack(FILE* infile) {
  fseek(infile, strlen(EDEN_PACK_MAGIC) * sizeof(EDEN_PACK_MAGIC[0]), SEEK_SET);
  edn_pack_t pack = {
    .floatslen = 0,
    .integerslen = 0,
    .stringslen = 0,
    .functionslen = 0,
    .name = "@no-name@"
  };
  fread(&pack.target_version, sizeof(pack.target_version), 1, infile);
  // TODO: read name
  fread(&pack.entryifuncid, sizeof(pack.entryifuncid), 1, infile);
  // TODO: read tables
  return pack;
}