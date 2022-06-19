#include "eden.hh"

#include <iostream>

namespace edn::nif {
  niflib::~niflib() {
    std::cout << "niflib is freed..." << std::endl;
    if (hmodlib != NULL) FreeLibrary(hmodlib);
  }

  res<sptr<niflib>> load(const str& filename, vm::vm& vm) {
    sptr<niflib> lib = std::make_shared<niflib>();

    lib->hmodlib = LoadLibraryA(filename.c_str());
    if (lib->hmodlib == NULL || lib->hmodlib == INVALID_HANDLE_VALUE) {
      std::cout << "failed to load DLL. Reason: " << GetLastError() << std::endl;
      return cpp::fail(err::kind::invalidfile);
    }

    typedef int (__cdecl *initproc)(void*);
    initproc initprocaddr = 
      (initproc) GetProcAddress(lib->hmodlib, "edn_nif_init");
    if (!initprocaddr) {
      std::cout << "failed to get address of 'edn_nif_init' in library '" << filename << "'." << std::endl;
      return cpp::fail(err::kind::invalidfile);
    }

    const auto res = initprocaddr(&vm);
    if (res != 0) {
      std::cout << "failed to init nif library in library '" << filename << "'. Reason: " << res << std::endl;
      return cpp::fail(err::kind::invalidfile);
    }

    return lib;
  }
}