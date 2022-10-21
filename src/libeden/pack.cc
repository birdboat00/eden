#include "pack.hh"

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>

#include "defines.hh"

namespace edn::pack {
#define DEF_READ_FOR(T) T read_##T(std::istream& s) { T val; s.read(reinterpret_cast<char*>(&val), sizeof(T)); return val; }
#define DEF_WRITE_FOR(T) void write_##T(std::ostream& s, T v) { s.write(reinterpret_cast<char*>(&v), sizeof(v)); }
#define DEF_RW_FOR(T) DEF_WRITE_FOR(T); DEF_READ_FOR(T);

DEF_RW_FOR(u8);
DEF_RW_FOR(u16);
DEF_RW_FOR(u32);
DEF_RW_FOR(i32);
DEF_RW_FOR(i64);
DEF_RW_FOR(u64);
DEF_RW_FOR(usize);
DEF_RW_FOR(f64);

#undef DEF_READ_FOR
#undef DEF_WRITE_FOR
#undef DEF_RW_FOR

  void write_string(std::ostream& s, const str& str) {
    write_usize(s, str.size());
    s.write(str.data(), str.size());
  }

  err::kind write_header(std::ostream& s, const pack& pack) {
    write_u16(s, kEdenPackMagic);

    write_u16(s, pack.bytecode_version);
    write_string(s, pack.name);
    write_string(s, pack.author);
    write_string(s, pack.version);
    write_u32(s, pack.entryfn);

    return err::kind::none;
  }

  err::kind write_constants_table(std::ostream& s, const pack& pack) {
    write_u32(s, pack.constants.size());

    for (usize i = 0; i < pack.constants.size(); i++) {
      const auto c = pack.constants.at(i);
      if (term::is<i64>(c)) {
        write_u8(s, 0b0000);
        write_i64(s, term::get<i64>(c));
      } else if (term::is<f64>(c)) {
          write_u8(s, 0b0001);
          write_f64(s, term::get<f64>(c));
      } else if (term::is<str>(c)) {
        write_u8(s, 0b0010);
        write_string(s, term::get<str>(c));
      }
    }

    return err::kind::none;
  }

  vec<bc::bc_t> compile_opcodes(cref<edn_fn> fn) {
    vec<bc::bc_t> bc;
    for (const auto& op : fn.bc) {
      std::visit(overload{
        [&](cref<bc::ops::move> m) {
          bc.push_back(static_cast<i32>(bc::opcode::move));
          bc.push_back(m.dest);
          bc.push_back(m.src);
        },
        [&](cref<bc::ops::ldc> ldc) {
           bc.push_back(static_cast<i32>(bc::opcode::ldc));
           bc.push_back(ldc.dest);
           bc.push_back(ldc.idx);
        },
        [&](cref<bc::ops::call> c) {
          bc.push_back(static_cast<i32>(c.tailcall ? bc::opcode::tailcall : bc::opcode::call));
          bc.push_back(c.idx);
        },
        [&](cref<bc::ops::ret> r) {
          bc.push_back(static_cast<i32>(bc::opcode::ret));
        },
        [&](cref<bc::ops::nifcallnamed> nc) {
          bc.push_back(static_cast<i32>(bc::opcode::nifcallnamed));
          bc.push_back(nc.arity);
          bc.push_back(nc.nameidx);
          for (const auto& arg : nc.args) { bc.push_back( arg); }
        },
        [&](cref<bc::ops::test> t) {
          auto opcode = bc::opcode::test_isint;
          if (t.fn == bc::ops::test_fun::isflt) { opcode = bc::opcode::test_isflt; }
          else if (t.fn == bc::ops::test_fun::isstr) { opcode = bc::opcode::test_isstr; }
          bc.push_back(static_cast<i32>(opcode));
          bc.push_back(t.dest);
          bc.push_back(t.reg);
        },
        [&](cref<bc::ops::cmp> c) {
          auto opcode = bc::opcode::cmp_islt;
          if (c.fn == bc::ops::cmp_fun::isge) { opcode = bc::opcode::cmp_isge; }
          else if (c.fn == bc::ops::cmp_fun::iseq) { opcode = bc::opcode::cmp_iseq; }
          else if (c.fn == bc::ops::cmp_fun::isne) { opcode = bc::opcode::cmp_isne; }
          bc.push_back(static_cast<i32>(opcode));
          bc.push_back(c.dest);
          bc.push_back(c.rl);
          bc.push_back(c.rr);
        },
        [&](cref<bc::ops::jump> j) {
          bc.push_back(static_cast<i32>(bc::opcode::jump));
          bc.push_back(j.dest);
        },
        [&](cref<bc::ops::label> l) {
          bc.push_back(static_cast<i32>(bc::opcode::label));
          bc.push_back(l.name);
        },
        [&](const auto&) { unimplemented();  }
        }, op);
    }

    return bc;
  }

  err::kind write_table_fns(std::ostream& s, const pack& pack) {
    if (pack.fns.size() > 0) {
      write_u32(s, pack.fns.size());
      for(usize i = 0; i < pack.fns.size(); i++) {
        const auto bc = compile_opcodes(pack.fns.at(i));
        write_u8(s, pack.fns.at(i).arity);
        write_string(s, pack.fns.at(i).name);
        write_usize(s, bc.size());
        for (const auto& v : bc) {
          write_i32(s, v);
        }
      }
    }

    return err::kind::none;
  }

  err::kind write_to_file(std::ostream& file, const pack& pack) {
    auto err = write_header(file, pack);
    if (err != err::kind::none) return err;

    err = write_constants_table(file, pack);
    if (err != err::kind::none) return err;

    err = write_table_fns(file, pack);
    return err;
  }

  str read_string(std::istream& file) {
    const auto len = read_usize(file);
    str buf(len, '\0');
    file.read(buf.data(), len);
    return str(buf);
  }

  err::kind read_constants_table(std::istream& s, edn::vec<term::term>& outtable) {
    const auto len = read_u32(s);
    outtable.reserve(len);
    for (usize i = 0; i < len; i++) {
      const u8 type = read_u8(s);
      if (type == 0b0000) {
        outtable.push_back(term::from<i64>(read_i64(s)));
      } else if (type == 0b0001) {
        outtable.push_back(term::from<f64>(read_f64(s)));
      } else if (type == 0b0010) {
        outtable.push_back(term::from<str>(read_string(s)));
      }
    }
    return err::kind::none;
  }

  auto read_opcodes(cref<vec<bc::bc_t>> bytecode) -> vec<bc::ops::bcop> {
    vec<bc::ops::bcop> ops;

    for (usize i = 0; i < bytecode.size(); i++) {
      const auto& val = bytecode.at(i);
      if (val >= static_cast<bc::bc_t>(bc::opcode::opcodecount)) {
        panic("opcode not known: %i", val);
      }
      const auto code = static_cast<bc::opcode>(val);

      auto testfun = bc::ops::test_fun::isint;
      auto cmpfun = bc::ops::cmp_fun::iseq;

      switch (code) {
      case bc::opcode::move: {
        const auto dest = bytecode.at(i + 1);
        if (dest >= 64) panic("opcode register is bigger than 64");
        const auto src = bytecode.at(i + 2);
        if (src >= 64) panic("opcode register is bigger than 64");
        ops.push_back(bc::ops::move{ .dest = static_cast<u8>(dest), .src = static_cast<u8>(src) });
        i = i + 2;
      } break;
      case bc::opcode::ldc: {
        const auto dest = bytecode.at(i + 1);
        if (dest >= 64) panic("opcode register is bigger than 64");
        const auto index = bytecode.at(i + 2);
        ops.push_back(bc::ops::ldc{ .dest = static_cast<u8>(dest), .idx = static_cast<bc::ops::idx_t>(index) });
        i = i + 2;
      } break;
      case bc::opcode::call: {
        const auto fn_id = bytecode.at(i + 1);
        ops.push_back(bc::ops::call{ .idx = static_cast<bc::ops::idx_t>(fn_id), .tailcall = false });
        i = i + 1;
      } break;
      case bc::opcode::tailcall: {
        const auto fn_id = bytecode.at(i + 1);
        ops.push_back(bc::ops::call{ .idx = static_cast<bc::ops::idx_t>(fn_id), .tailcall = true });
        i = i + 1;
      } break;
      case bc::opcode::ret: {
        ops.push_back(bc::ops::ret{});
      } break;
      case bc::opcode::nifcallnamed: {
        const auto arity = bytecode.at(i + 1);
        const auto name_index = bytecode.at(i + 2);
        vec<bc::bc_t> args;
        for (u8 a = 0; a < arity; a++) {
          const auto arg = bytecode.at(i + a + 3);
          args.push_back(arg);
        }
        ops.push_back(bc::ops::nifcallnamed{ .arity = static_cast<u8>(arity), .nameidx = static_cast<bc::ops::idx_t>(name_index), .args = args });
        i = i + 2 + arity;
      } break;
      case bc::opcode::test_isint:
        testfun = bc::ops::test_fun::isint;
      case bc::opcode::test_isstr:
        testfun = bc::ops::test_fun::isstr;
      case bc::opcode::test_isflt:
        testfun = bc::ops::test_fun::isflt;
        {
          const auto label = bytecode.at(i + 1);
          const auto reg = bytecode.at(i + 2);
          if (reg >= 64) panic("test register is bigger than 64");
          ops.push_back(bc::ops::test{ .dest = static_cast<usize>(label), .fn = testfun, .reg = static_cast<u8>(reg) });
          i = i + 2;
        } break;
      case bc::opcode::cmp_islt:
        cmpfun = bc::ops::cmp_fun::islt;
      case bc::opcode::cmp_isge:
        cmpfun = bc::ops::cmp_fun::isge;
      case bc::opcode::cmp_iseq:
        cmpfun = bc::ops::cmp_fun::iseq;
      case bc::opcode::cmp_isne:
        cmpfun = bc::ops::cmp_fun::isne;
        {
          const auto label = bytecode.at(i + 1);
          const auto rl = bytecode.at(i + 2);
          const auto rr = bytecode.at(i + 3);
          if (rl >= 64 || rr >= 64) panic("cmp register left or right is bigger than 64");
          ops.push_back(bc::ops::cmp{ .dest = static_cast<usize>(label), .fn = cmpfun, .rl = static_cast<u8>(rl), .rr = static_cast<u8>(rr) });
          i = i + 3;
        } break;
      case bc::opcode::jump: {
        const auto label = bytecode.at(i + 1);
        ops.push_back(bc::ops::jump{ .dest = static_cast<usize>(label) });
        i = i + 1;
      } break;
      case bc::opcode::label: {
        const auto name = bytecode.at(i + 1);
        ops.push_back(bc::ops::label{ .name = static_cast<usize>(name) });
        i = i + 1;
      } break;

      default: unreachable();
      }
    }

    return ops;
  }

  err::kind read_table_fns(std::istream& s, edn::vec<edn_fn>& outtable) {
    const auto len = read_u32(s);
    outtable.reserve(len);
    for (usize i = 0; i < len; i++) {
      outtable.push_back(edn_fn {});
      outtable.at(i).arity = read_u8(s);
      outtable.at(i).name = read_string(s);
      const auto bclen = read_usize(s);
      vec<bc::bc_t> bytecode;
      bytecode.reserve(bclen);
      for (usize j = 0; j < bclen; j++) {
        bytecode.push_back(read_i32(s));
      }
      const auto opcodes = read_opcodes(bytecode);
      outtable.at(i).bc = opcodes;
    }

    for (auto& fn : outtable) {
      for (usize i = 0; i < fn.bc.size(); i++) {
        const auto& op = fn.bc.at(i);
        std::visit(overload{
          [&](const bc::ops::label& l) { fn.labels.insert_or_assign(l.name, i);  },
          [&](const auto&) {}
          }, op);
      }
    }

    return err::kind::none;
  }

  res<sptr<pack>> read_from_file(std::istream& file) {
    auto pck = std::make_shared<pack>();

    const auto magic = read_u16(file);
    if (magic != kEdenPackMagic) {
      eprintln("file is not an eden pack file.");
      return cpp::fail(err::kind::invalidpack);
    }
    println("file is eden pack");

    pck->bytecode_version = read_u16(file);
    if (pck->bytecode_version != kEdenBytecodeVersion) {
      eprintln("pack target version ({}) is different from eden vm bytecode version ({}).", pck->bytecode_version, kEdenBytecodeVersion);
      return cpp::fail(err::kind::invalidpack);
    }

    pck->name = read_string(file);
    pck->author = read_string(file);
    pck->version = read_string(file);
    pck->entryfn = read_u32(file);

    println("finished reading header...");

    {
      const auto err = read_constants_table(file, pck->constants);
      if (err != err::kind::none) return cpp::fail(err);
      println("finished reading constant table...");
    }
    {
      const auto err = read_table_fns(file, pck->fns);
      if (err != err::kind::none) return cpp::fail(err);
      println("finished reading functions table...");
    }

    return pck;
  }

  err::kind dump_to_file(std::ostream& file, const pack& pack) {
    file << "eden runtime " << kEdenVersion << " [bytecode: " << kEdenBytecodeVersion << "] (compiled: " << kEdenBuildTime << ")\n";
    file << "--- BEGIN PACK DUMP ---\n";
    file << "pack\n  name-> " << pack.name
    << "\n  author-> " << pack.author
    << "\n  version-> " << pack.version
    << "\n  bytecodeversion-> " << pack.bytecode_version
    << "\n  entryfn-> " << pack.entryfn << "\n";
    
    file << "constants (" << std::size(pack.constants) << ")\n";
    usize ii = 0;
    std::for_each(std::begin(pack.constants), std::end(pack.constants), [&](auto i) {
      file << "  @" << ii << " -> " << term::to_str(i).value() << "\n";
      ii++;
    });

    file << "functions (" << std::size(pack.fns) << ")\n";
    usize fni = 0;
    std::for_each(std::begin(pack.fns), std::end(pack.fns), [&](const edn_fn& fn) {
      file << "  @" << fni << " -> '" << fn.name << "/" << static_cast<u64>(fn.arity) << "' (" << fn.bc.size() << ") {\n";
      for (const auto& op : fn.bc) {
        file << "    " << bc::ops::to_str(op) << "\n";
      }
      file << "  }\n";
      fni++;
    });

    file << "--- END PACK DUMP ---\n";

    return err::kind::none;
  }

  pack_builder::pack_builder() {}

  auto pack_builder::with_name(cref<str> name) -> ref<pack_builder> {
    _name = name;
    return *this;
  }

  auto pack_builder::with_author(cref<str> name) -> ref<pack_builder> {
    _author = name;
    return *this;
  }

  auto pack_builder::with_version(cref<str> ver) -> ref<pack_builder> {
    _version = ver;
    return *this;
  }

  auto pack_builder::constant(cref<term::term> term) -> ref<pack_builder> {
    _constants.push_back(term);
    return *this;
  }

  auto pack_builder::constants(cref<vec<term::term>> terms) -> ref<pack_builder> {
    _constants = terms;
    return *this;
  }

  auto pack_builder::function(cref<edn_fn> fn) -> ref<pack_builder> {
    _fns.push_back(fn);
    return *this;
  }

  auto pack_builder::entry(u32 entryidx) -> ref<pack_builder> {
    _entryfn = entryidx;
    return *this;
  }

  auto pack_builder::build() -> pack {
    return pack {
      .name = _name,
      .author = _author,
      .version = _version,
      .bytecode_version = kEdenBytecodeVersion,
      .entryfn = _entryfn,
      .constants = _constants,
      .naps = {},
      .fns = _fns
    };
  }

  fn_builder::fn_builder() {}

  auto fn_builder::signature(cref<str> name, u8 arity) -> ref<fn_builder> {
    _name = name;
    _arity = arity;
    return *this;
  }

  auto fn_builder::bytecode(vec<bc::ops::bcop> ops) -> ref<fn_builder> {
    _bc = ops;
    return *this;
  }

  auto fn_builder::op(bc::ops::bcop op) -> ref<fn_builder> {
    _bc.push_back(op);
    return *this;
  }

  auto fn_builder::build() -> edn_fn {
    return edn_fn {
      .bc = _bc,
      .labels = {},
      .name = _name,
      .arity = _arity
    };
  }
}