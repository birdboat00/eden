#include <iostream>

#include <libeden/eden.hh>
#include "vm.hh"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace edn::nif {
  niflib::~niflib() {
    if (libptr != nullptr) FreeLibrary(static_cast<HMODULE>(libptr));
  }

  auto load(cref<str> filename, vm::vm& vm)->res<sptr<niflib>> {
    auto lib = std::make_shared<niflib>();

    lib->libptr = LoadLibraryA(filename.c_str());
    if (lib->libptr == nullptr || lib->libptr == INVALID_HANDLE_VALUE) {
      eprintln("failed to load nap DLL. Reason: {}.", GetLastError());
      return cpp::fail(err::kind::invalidfile);
    }

    using initproc = int(__cdecl*)(void*);
    const auto proc = (initproc)GetProcAddress(static_cast<HMODULE>(lib->libptr), "edn_nif_init");
    if (!proc) {
      eprintln("Failed to get address of 'edn_nif_init' in library '{}'", filename);
      return cpp::fail(err::kind::invalidfile);
    }

    const auto res = proc(&vm);
    if (res != 0) {
      eprintln("failed to init nif library in library '{}'. Reason: {}.", filename, res);
      return cpp::fail(err::kind::invalidfile);
    }

    return lib;
  }
}