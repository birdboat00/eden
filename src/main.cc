#include <iostream>
#include <fstream>
#include <vector>

#include "cxxopts.hh"

#include "libeden/eden.hh"
#include "libedenvm/vm.hh"
#include "libedenvm/nif.hh"
#include "builtin_test_pack.hh"

#include <libeden_bifs/edn_bifs.hh>

auto printversioninfo() -> int {
  edn::println("eden runtime {} [bytecode: {}] (build {})", 
    edn::kEdenVersion,
    edn::kEdenBytecodeVersion,
    edn::kEdenBuildTime
  );
  return 0;
}

auto dumppack(edn::cref<edn::str> filename) -> int {
  
  edn::println("dump pack");

  std::fstream packfile(filename, std::ios::in | std::ios::binary);
  if (packfile.good()) {
    const auto res = edn::pack::read_from_file(packfile);
    if (res.has_error()) {
      edn::eprintln("failed to read pack file. Error: {}.", static_cast<edn::i32>(res.error()));
      return static_cast<edn::i32>(res.error());
    }
    else {
      edn::println("read pack file...");
    }
    std::fstream outfile(filename + ".dump.txt", std::ios::trunc | std::ios::out);
    if (outfile.good()) {
      const auto err = edn::pack::dump_to_file(outfile, *res.value());
      if (err != edn::err::kind::none) {
        edn::eprintln("failed to dump pack file. Error: {}.", static_cast<edn::i32>(err));
        return static_cast<edn::i32>(err);
      }
      return 0;
    }
    edn::eprintln("failed to create file '{}.dump.txt'", filename);
    return -1;
  }
  edn::eprintln("failed to open file '{}'.", filename);
  return -1;
}

auto savetestpack() -> int {
  const auto bitp = edn::btp::create_test_pack();
  std::fstream outfile("codecc.eden", std::ios::trunc | std::ios::out | std::ios::binary);
  if (outfile.good()) {
    const auto err = edn::pack::write_to_file(outfile, bitp);
    if (err != edn::err::kind::none) return static_cast<edn::i32>(err);
    return 0;
  }
  return -1;
}

auto interpretpack(const edn::str& filename, const edn::vec<edn::str>& nappaths) -> int {
  std::fstream infile(filename, std::ios::in | std::ios::binary);
  if (infile.good()) {
    auto res = edn::pack::read_from_file(infile);
    if (res.has_error()) {
      edn::eprintln("failed to read pack from file '{}'. Error: '{}'.", filename, static_cast<edn::i32>(res.error()));
      return static_cast<edn::i32>(res.error());
    } else {
      edn::println("successfully read pack file ...");
    }

    edn::vm::vm vm = edn::vm::vm { .pack = res.value(), .nifs = { } };
    edn::println("created vm ...");

    edn::bif::register_bifs(vm);
    edn::println("registered bifs ...");

    edn::vec<edn::sptr<edn::nif::niflib>> niflibs;
    for (auto& np : nappaths) {
      const auto res = edn::nif::load(np, vm);
      if (res.has_error()) {
        edn::eprintln("failed to load nif lib '{}'. Reason: {}.", np, static_cast<edn::i32>(res.error()));
        return -1;
      }
      niflibs.push_back(res.value());
    }
    edn::println("loaded naps ...");
    
    const auto vmerr = edn::vm::run(vm);
    if (vmerr != edn::err::kind::none) {
      edn::eprintln("failed to execute pack file '{}'. Error: {}.", filename, static_cast<edn::i32>(vmerr));
      return -1;
    }
    return 0;
  } else {
    edn::eprintln("failed to open file '{}'.", filename);
    return -1;
  }
}

auto main (int argc, char** argv) -> int {
  cxxopts::Options options("eden", "eden runtime");
  options
    .set_width(80)
    .add_options()
    ("v,version", "Print version information")
    ("h,help", "Print usage")
    ("p,pack", "Load the eden pack and execute it", cxxopts::value<edn::str>(), "<filename>")
    ("n,naps", "Load the list of native packs", cxxopts::value<edn::vec<edn::str>>(), "<filenames>")
    ("d,dump", "Dump the eden pack file", cxxopts::value<edn::str>(), "<filename>");
  options.add_options("Builtin Test Pack")
    ("s,savetestpack", "Save the builtin test pack to codecc.eden and exit");

  cxxopts::ParseResult result;
  try {
    result = options.parse(argc, argv);
  } catch (cxxopts::OptionException& e) {
    edn::eprintln(e.what());
    return -1;
  }

  if (result.count("help")) {
    edn::println(options.help());
    return 0;
  }

  if (result.count("version")) {
    return printversioninfo();
  }

  if (result.count("dump")) {
    const auto packfilename = result["dump"].as<edn::str>();
    return dumppack(packfilename);
  }

  if (result.count("savetestpack")) {
    return savetestpack();
  }

  if (result.count("pack")) {
    const auto packfilename = result["pack"].as<edn::str>();
    edn::vec<edn::str> naps;
    if (result.count("naps")) {
      naps = result["naps"].as<edn::vec<edn::str>>();
    }
    return interpretpack(packfilename, naps);
  }

  return 0;
}