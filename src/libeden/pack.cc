#include "eden.hh"

#include <bitset>
#include <fstream>
#include <iostream>

#define DEF_READ_FOR(T) T read_##T(std::istream& s) { T val; s.read(reinterpret_cast<char*>(&val), sizeof(T)); return val; }
#define DEF_WRITE_FOR(T) void write_##T(std::ostream& s, T v) { s.write(reinterpret_cast<char*>(&v), sizeof(v)); }
#define DEF_RW_FOR(T) DEF_WRITE_FOR(T); DEF_READ_FOR(T);

namespace edn::pack {
DEF_RW_FOR(u8);
DEF_RW_FOR(u16);
DEF_RW_FOR(u32);
DEF_RW_FOR(i32);
DEF_RW_FOR(i64);
DEF_RW_FOR(u64);
DEF_RW_FOR(usize);
DEF_RW_FOR(f64);

  void write_string(std::ostream& s, const str& str) {

  }

  err::err write_header(std::ostream& s, const pack& pack) {
    s.write(&kEdenPackMagic.c_str()[0], kEdenPackMagic.length());

    write_u16(s, pack.bytecode_version);
    write_string(s, pack.name);
    write_u32(s, pack.entryfn);

    std::bitset<8> tables(0);
    tables.set(0, pack.ints.size() > 0);
    tables.set(1, pack.flts.size() > 0);
    tables.set(2, pack.strs.size() > 0);
    tables.set(3, pack.fns.size() > 0);
    write_u8(s, tables.to_ulong());

    return err::make_err_none(err::err_module::pack);
  }

  err::err write_table_ints(std::ostream& s, const pack& pack) {
    if (pack.ints.size() > 0) {
      write_u32(s, pack.ints.size());
      for(auto i = 0; i < pack.ints.size(); i++) {
        write_i64(s, pack.ints.at(i));
      }
    }
    return err::make_err_none(err::err_module::pack);
  }

  err::err write_table_flts(std::ostream& s, const pack& pack) {
    if (pack.flts.size() > 0) {
      write_u32(s, pack.flts.size());
      for(auto i = 0; i < pack.flts.size(); i++) {
        write_f64(s, pack.flts.at(i));
      }
    }
    return err::make_err_none(err::err_module::pack);
  }

  err::err write_table_strs(std::ostream& s, const pack& pack) {
    if (pack.strs.size() > 0) {
      write_u32(s, pack.strs.size());
      for(auto i = 0; i < pack.strs.size(); i++) {
        write_string(s, pack.strs.at(i));
      }
    }
    return err::make_err_none(err::err_module::pack);
  }

  err::err write_table_fns(std::ostream& s, const pack& pack) {
    if (pack.fns.size() > 0) {
      write_u32(s, pack.fns.size());
      for(auto i = 0; i < pack.fns.size(); i++) {
        write_usize(s, pack.fns.at(i).bytecode.size());
        for (auto j = 0; j < pack.fns.at(i).bytecode.size(); j++) {
          write_i32(s, pack.fns.at(i).bytecode.at(j));
        }
      }
    }
    return err::make_err_none(err::err_module::pack);
  }

  err::err write_to_file(std::ostream& file, const pack& pack) {
    auto err = write_header(file, pack);
    if (!err::is_ok(err)) return err;

    err = write_table_ints(file, pack);
    if (!err::is_ok(err)) return err;
  }

  str read_string(std::istream& file) {
    const auto len = read_usize(file);
    char buf[len + 1];
    buf[len] = '\0';
    file.read(&buf[0], len);
    return str(buf);
  }

  err::err read_table_ints(std::istream& s, std::vector<i64>& outtable) {
    const auto len = read_u32(s);
    for (usize i = 0; i < len; i++) {
      outtable.push_back(read_i64(s));
    }
    return err::make_err_none(err::err_module::pack);
  }

  err::err read_table_flts(std::istream& s, std::vector<f64>& outtable) {
    const auto len = read_u32(s);
    for (usize i = 0; i < len; i++) {
      outtable.push_back(read_f64(s));
    }
    return err::make_err_none(err::err_module::pack);
  }

  err::err read_table_strs(std::istream& s, std::vector<str>& outtable) {
    const u32 len = read_u32(s);
    for (usize i = 0; i < len; i++) {
      outtable.push_back(read_string(s));
    }
    return err::make_err_none(err::err_module::pack);
  }

  err::err read_table_fns(std::istream& s, std::vector<edn_fn>& outtable) {
    const auto len = read_u32(s);
    for (usize i = 0; i < len; i++) {
      outtable.push_back(edn_fn {});
      const auto bclen = read_usize(s);
      outtable.at(i).bytecode.reserve(bclen);
      for (usize j = 0; j < bclen; j++) {
        outtable.at(i).bytecode.push_back(read_i32(s));
      }
    }
    return err::make_err_none(err::err_module::pack);
  }

  err::err read_from_file(std::istream& file, pack& pack) {
    std::cout << "kEdenPackMagic len is " << kEdenPackMagic.length() << std::endl;
    char magic_buf[kEdenPackMagic.length() + 1];
    file.read(&magic_buf[0], kEdenPackMagic.length());
    magic_buf[kEdenPackMagic.length()] = '\0';
    str magic = magic_buf;
    if (magic.compare(kEdenPackMagic) != 0) {
      std::cout << "file is not a pack file. magic is " << magic << " len " << magic.length() << std::endl;
      return err::make_err(err::err_kind::invalidpack, err::err_module::pack);
    }

    pack.bytecode_version = read_u16(file);
    if (pack.bytecode_version != kEdenBytecodeVersion) {
      std::cout << "pack target version (" << pack.bytecode_version << ") is different from eden vm bytecode version (" << kEdenBytecodeVersion << ")." << std::endl;
      return err::make_err(err::err_kind::invalidpack, err::err_module::pack);
    }

    pack.name = read_string(file);
    pack.entryfn = read_u32(file);

    const auto tables = std::bitset<8>(read_u8(file));
    if (tables.test(0)) {
      std::cout << "has integer table" << std::endl;
      const auto err = read_table_ints(file, pack.ints);
      if (!err::is_ok(err)) return err;
    }
    if (tables.test(1)) {
      std::cout << "has float table" << std::endl;
      const auto err = read_table_flts(file, pack.flts);
      if (!err::is_ok(err)) return err;
    }
    if (tables.test(2)) {
      std::cout << "has strings table" << std::endl;
      const auto err = read_table_strs(file, pack.strs);
      if (!err::is_ok(err)) return err;
    }
    if (tables.test(3)) {
      std::cout << "has functions table" << std::endl;
      const auto err = read_table_fns(file, pack.fns);
      if (!err::is_ok(err)) return err;
    }

    return err::make_err_none(err::err_module::pack);
  }

  err::err dump_to_file(std::ostream& file, const pack& pack) {
    file << "--- BEGIN PACK DUMP ---\n";
    file << "pack\n  name->" << pack.name << "\n  bytecodeversion-> " << pack.bytecode_version << "\n  entryfn-> " << pack.entryfn << "\n";
    
    file << "tables:\nints (" << std::size(pack.ints) << ")\n";
    usize ii = 0;
    std::for_each(std::begin(pack.ints), std::end(pack.ints), [&](auto i) {
      file << "  @" << ii << " -> " << i << "\n";
      ii++;
    });

    file << "floats (" << std::size(pack.flts) << ")\n";
    usize fi = 0;
    std::for_each(std::begin(pack.flts), std::end(pack.flts), [&](auto f) {
      file << "  @" << fi << " -> " << f << "\n";
      fi++;
    });

    file << "strings (" << std::size(pack.strs) << ")\n";
    usize si = 0;
    std::for_each(std::begin(pack.strs), std::end(pack.strs), [&](auto s) {
      file << "  @" << si << " -> \"" << s << "\"\n";
      si++;
    });

    file << "functions (" << std::size(pack.fns) << ")\n";
    usize fni = 0;
    std::for_each(std::begin(pack.fns), std::end(pack.fns), [&](const edn_fn& fn) {
      file << "  @" << fni << " -> (" << fn.bytecode.size() << ") {\n";
      for(usize i = 0; i < fn.bytecode.size(); i++) {
        const auto bclen = fn.bytecode.size();
        const auto opcode = bc::opcode_to_str(static_cast<bc::opcode>(fn.bytecode.at(i)));
        auto arity = bc::opcode_arity(static_cast<bc::opcode>(fn.bytecode.at(i)), (i + 1 < bclen) ? fn.bytecode.at( i + 1) : 0);
        const auto op = bc::op {
          .opcode = static_cast<bc::opcode>(fn.bytecode.at(i)),
          .args = {
            (i + 1 < bclen) ? fn.bytecode.at(i + 1) : 0,
            (i + 2 < bclen) ? fn.bytecode.at(i + 2) : 0,
            (i + 3 < bclen) ? fn.bytecode.at(i + 3) : 0,
            (i + 4 < bclen) ? fn.bytecode.at(i + 4) : 0,
            (i + 5 < bclen) ? fn.bytecode.at(i + 5) : 0
          }
        };

        file << "    " << bc::op_to_str(op) << "\n";
        i += arity;
      }
      file << "  }\n";
    });

    file << "--- END PACK DUMP ---\n";

    return err::make_err_none(err::err_module::pack);
  }
}