#include "ohs/dimse/dicom_validator.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/ofstd/ofcond.h>

#include <string>

namespace ohs::dimse {

namespace {

ValidationResult fail(const std::string& message) {
  ValidationResult result;
  result.message = message;
  return result;
}

}  // namespace

ValidationResult DicomValidator::validate(const std::string& path) const {
  if (path.empty()) {
    return fail("dicom path is empty");
  }

  DcmFileFormat fileformat;
  OFCondition cond = fileformat.loadFile(path.c_str());
  if (cond.bad()) {
    return fail(std::string("failed to read DICOM file: ") + cond.text());
  }

  DcmDataset* dataset = fileformat.getDataset();
  if (dataset == nullptr || dataset->isEmpty()) {
    return fail("DICOM file has no dataset");
  }

  cond = dataset->verify(/*autocorrect=*/OFFalse);
  if (cond.bad()) {
    return fail(std::string("DICOM dataset failed verification: ") + cond.text());
  }

  DcmMetaInfo* meta = fileformat.getMetaInfo();
  if (meta != nullptr && !meta->isEmpty()) {
    cond = meta->verify(/*autocorrect=*/OFFalse);
    if (cond.bad()) {
      return fail(std::string("DICOM meta header failed verification: ") + cond.text());
    }
    if (!meta->tagExistsWithValue(DCM_TransferSyntaxUID)) {
      return fail("DICOM meta header missing Transfer Syntax UID");
    }
  }

  if (!dataset->tagExistsWithValue(DCM_SOPClassUID)) {
    return fail("DICOM dataset missing SOP Class UID");
  }
  if (!dataset->tagExistsWithValue(DCM_SOPInstanceUID)) {
    return fail("DICOM dataset missing SOP Instance UID");
  }
  if (!dataset->tagExistsWithValue(DCM_PixelData)) {
    return fail("DICOM dataset missing Pixel Data");
  }

  ValidationResult result;
  result.ok = true;
  result.message = "valid DICOM image";

  return result;


}

}  // namespace ohs::dimse
