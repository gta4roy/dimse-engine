#include "ohs/dimse/cstore_scu.hpp"

#include <cstdlib>
#include <iostream>
#include <string>

namespace {

void print_usage(const char* argv0) {
  std::cerr
      << "Usage: " << argv0
      << " --host <host> --port <port> --calling-ae <ae> --called-ae <ae> --file <path.dcm>\n"
      << "\n"
      << "Plain C-STORE SCU harness for dimse-engine (Phase 1).\n"
      << "Requires DCMTK shared libraries from $DCMTK_LIB (see scripts/build.sh).\n";
}

bool require_arg(int argc, char** argv, int& i, std::string& out) {
  if (i + 1 >= argc) {
    return false;
  }
  out = argv[++i];
  return true;
}

}  // namespace

int main(int argc, char** argv) {
  ohs::dimse::Peer peer;
  std::string dicom_path;

  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    if (arg == "--host") {
      if (!require_arg(argc, argv, i, peer.host)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (arg == "--port") {
      std::string port_str;
      if (!require_arg(argc, argv, i, port_str)) {
        print_usage(argv[0]);
        return 2;
      }
      peer.port = static_cast<std::uint16_t>(std::stoi(port_str));
    } else if (arg == "--calling-ae") {
      if (!require_arg(argc, argv, i, peer.calling_ae)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (arg == "--called-ae") {
      if (!require_arg(argc, argv, i, peer.called_ae)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (arg == "--file") {
      if (!require_arg(argc, argv, i, dicom_path)) {
        print_usage(argv[0]);
        return 2;
      }
    } else if (arg == "--help" || arg == "-h") {
      print_usage(argv[0]);
      return 0;
    } else {
      std::cerr << "Unknown argument: " << arg << "\n";
      print_usage(argv[0]);
      return 2;
    }
  }

  if (peer.host.empty() || dicom_path.empty()) {
    print_usage(argv[0]);
    return 2;
  }

  ohs::dimse::CStoreScu scu(peer);
  const ohs::dimse::StoreResult result = scu.store_file(dicom_path);

  std::cout << "ok=" << (result.ok ? "true" : "false") << "\n"
            << "dimse_status=0x" << std::hex << result.dimse_status << std::dec << "\n"
            << "sop_class_uid=" << result.sop_class_uid << "\n"
            << "sop_instance_uid=" << result.sop_instance_uid << "\n"
            << "transfer_syntax_uid=" << result.transfer_syntax_uid << "\n"
            << "message=" << result.message << "\n";

  return result.ok ? 0 : 1;
}
