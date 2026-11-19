#pragma once

#include "ohs/dimse/types.hpp"

#include <string>

namespace ohs::dimse {

/// Reads a DICOM image from disk and checks that DCMTK accepts it.
///
/// Public headers stay free of DCMTK types so callers never depend on DCMTK
/// includes. All DCMTK usage lives in the .cpp.
class DicomValidator {
 public:
  /// Load the file at \p path and validate it as a DICOM image.
  ///
  /// Succeeds when DCMTK can parse the file, the dataset verifies, and the
  /// instance carries SOP Class UID, SOP Instance UID, and Pixel Data.
  ValidationResult validate(const std::string& path) const;
};

}  // namespace ohs::dimse
