#include <iostream>
#include <fstream>
#include <vector>

#include "libeden/eden.hh"

auto main (int argc, char** argv) -> int {

  std::vector args(argv + 1, argv + argc);

  if (args.size() > 1 && args.at(1) == "--dumptestpack") {

  }

  std::ifstream codeeden(argv[1], std::ios::binary);
  if (codeeden.good()) {
    edn::pack::pack pck;
    const auto res = edn::pack::read_from_file(codeeden, pck);
    if (edn::err::is_ok(res)) {
      edn::pack::dump_to_file(std::cout, pck);
      std::fstream outfile("codecc.eden", std::ios::trunc | std::ios::binary | std::ios::out);
      if (outfile.good()) {
        edn::pack::write_to_file(outfile, pck);
      } else {
        return -1;
      }
      return 0;
    }    
  } else {
    return -1;
  }

  return 0;
}