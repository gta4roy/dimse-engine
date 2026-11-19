#pragma once

#include "ohs/dimse/types.hpp"

#include <string>

namespace ohs::dimse {

/// Thin C-STORE SCU wrapper around DCMTK.
///
/// Public headers stay free of DCMTK types so callers (CLI today, gRPC later)
/// never depend on DCMTK includes. All DCMTK usage lives in the .cpp.
class CStoreScu {
 public:
  explicit CStoreScu(Peer peer);

  CStoreScu(const CStoreScu&) = delete;
  CStoreScu& operator=(const CStoreScu&) = delete;

  /// Open an association, C-STORE the DICOM file at \p dicom_path, then release.
  /// Plain DIMSE (no TLS) in this initial implementation.
  StoreResult store_file(const std::string& dicom_path);

 private:
  Peer peer_;
};

}  // namespace ohs::dimse
