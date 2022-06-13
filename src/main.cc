#include <iostream>
#include <fstream>
#include <vector>

#include "libeden/eden.hh"

auto main (int argc, char** argv) -> int {

  std::vector<edn::str> args(argv + 1, argv + argc);
  std::for_each(std::begin(args), std::end(args), [](const auto arg) {
    std::cout << "arg: " << arg << std::endl;
  });

  if (args.size() >= 1 && args.at(0).compare("--dumptestpack") == 0) {
    std::cout << "--dumptestpack" << std::endl;
    const auto bitp = edn::btp::create_test_pack();
    std::fstream outfile("codecc.eden.dump.txt", std::ios::trunc | std::ios::out);
    if (outfile.good()) {
      std::cout << "dumping bitp to codecc.eden.dump.txt ..." << std::endl;
      const auto err = edn::pack::dump_to_file(outfile, bitp);
      if (!edn::err::is_ok(err)) return static_cast<edn::i32>(err.kind);
      return 0;
    }
    std::cout << "failed to dump file to codecc.eden.dump.txt." << std::endl;
    return -1;
  } else if (args.size() >= 1 && args.at(0) == "--savetestpack") {
    const auto bitp = edn::btp::create_test_pack();
    std::fstream outfile("codecc.eden", std::ios::trunc | std::ios::out | std::ios::binary);
    if (outfile.good()) {
      const auto err = edn::pack::write_to_file(outfile, bitp);
      if (!edn::err::is_ok(err)) return static_cast<edn::i32>(err.kind);
      return 0;
    }
    return -1;
  } else if (args.size() >= 2 && args.at(0) == "--load") {
    const auto filename = args.at(1);
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
      edn::vm::vm vm = edn::vm::vm { .pack = pack };
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

  return 0;
}