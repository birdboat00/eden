#include <iostream>
#include <fstream>
#include <vector>

#include "cxxopts.hh"

#include "libeden/eden.hh"

auto printversioninfo() -> int {
  std::cout << "eden runtime " << edn::kEdenVersion
    << " [bytecode: " << edn::kEdenBytecodeVersion << "] (compiled: "
    << edn::kEdenBuildTime << ")" << std::endl;
  return 0;
}

auto dumppack(edn::cref<edn::str> filename) -> int {
  std::cout << "dump pack" << std::endl;

  std::fstream packfile(filename, std::ios::in | std::ios::binary);
  if (packfile.good()) {
    edn::pack::pack pack;
    const auto err = edn::pack::read_from_file(packfile, pack);
    if (!edn::err::is_ok(err)) {
      std::cout << "failed to read pack file. Error: " << edn::err::to_str(err) << "." << std::endl;
      return static_cast<edn::i32>(err.kind);
    }
    std::fstream outfile(filename + ".dump.txt", std::ios::trunc | std::ios::out);
    if (outfile.good()) {
      const auto err = edn::pack::dump_to_file(outfile, pack);
      if (!edn::err::is_ok(err)) {
        std::cout << "failed to dump pack file. Error: " << edn::err::to_str(err) << "." << std::endl;
        return static_cast<edn::i32>(err.kind);
      }
      return 0;
    }
    std::cout << "failed to create file '" << filename << ".dump.txt'." << std::endl;
    return -1;
  }
  std::cout << "failed to open file '" << filename << "'." << std::endl;
  return -1;
}

auto savetestpack() -> int {
  const auto bitp = edn::btp::create_test_pack();
  std::fstream outfile("codecc.eden", std::ios::trunc | std::ios::out | std::ios::binary);
  if (outfile.good()) {
    const auto err = edn::pack::write_to_file(outfile, bitp);
    if (!edn::err::is_ok(err)) return static_cast<edn::i32>(err.kind);
    return 0;
  }
  return -1;
}

auto interpretpack(const edn::str& filename, const std::vector<edn::str>& nappaths) -> int {
  std::fstream infile(filename, std::ios::in | std::ios::binary);
  if (infile.good()) {
    edn::pack::pack pack;
    const auto err = edn::pack::read_from_file(infile, pack);
    if (!edn::err::is_ok(err)) {
      std::cout << "failed to read pack from file '" << filename << "'. Error: '" << edn::err::to_str(err) << "'." << std::endl;
      return -1;
    } else {
      std::cout << "successfully read pack file ..." << std::endl;
    }
    edn::vm::vm vm = edn::vm::vm { .pack = pack, .callstack = {}, .regs = {}, .nifs = {} };
    edn::bif::register_bifs(vm);
    std::vector<edn::sptr<edn::nif::niflib>> niflibs;
    for (auto& np : nappaths) {
      const auto res = edn::nif::load(np, vm);
      if (res.has_error()) {
        std::cout << "failed to load nif lib '" << np << "'. Reason: " << static_cast<int>(res.error()) << std::endl;
        return -1;
      }
      niflibs.push_back(res.value());
    }
    const auto vmerr = edn::vm::run(vm);
    if (!edn::err::is_ok(vmerr)) {
      std::cout << "failed to execute pack file '" << filename << "'. Error: '" << edn::err::to_str(vmerr) << "'." << std::endl; 
      return -1;
    }
    return 0;
  } else {
    std::cout << "failed to open file '" << filename << "'." << std::endl;
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
    ("n,naps", "Load the list of native packs", cxxopts::value<std::vector<edn::str>>(), "<filenames>")
    ("d,dump", "Dump the eden pack file", cxxopts::value<edn::str>(), "<filename>");
  options.add_options("Builtin Test Pack")
    ("s,savetestpack", "Save the builtin test pack to codecc.eden and exit");

  cxxopts::ParseResult result;
  try {
    result = options.parse(argc, argv);
  } catch (cxxopts::OptionException& e) {
    std::cout << e.what() << std::endl;
    return -1;
  }

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
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
    std::vector<edn::str> naps;
    if (result.count("naps")) {
      naps = result["naps"].as<std::vector<edn::str>>();
    }
    return interpretpack(packfilename, naps);
  }

  return 0;
}