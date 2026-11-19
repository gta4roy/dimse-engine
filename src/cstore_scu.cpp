#include "ohs/dimse/cstore_scu.hpp"
#include "dcmtk/dcmdata/dctypes.h"
#include "ohs/dimse/dicom_validator.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcxfer.h>
#include <dcmtk/dcmnet/dstorscu.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/oflist.h>
#include <dcmtk/ofstd/offile.h>

#include <sstream>
#include <utility>

namespace ohs::dimse {

namespace {

std::string status_hex(std::uint16_t status) {
  std::ostringstream oss;
  oss << "0x" << std::hex << std::uppercase << status;
  return oss.str();
}

OFCondition add_pc(DcmStorageSCU& scu, const char* abstract_syntax, const OFList<OFString>& xfers) {
  return scu.addPresentationContext(abstract_syntax, xfers);
}

}  // namespace

CStoreScu::CStoreScu(Peer peer) : peer_(std::move(peer)) {}

StoreResult CStoreScu::store_file(const std::string& dicom_path) {
  StoreResult result;

  if (peer_.host.empty()) {
    result.message = "peer host is empty";
    return result;
  }
  if (dicom_path.empty()) {
    result.message = "dicom path is empty";
    return result;
  }

  // Read file once so we can report SOP UIDs even if association fails later.
  DcmFileFormat fileformat;
  OFCondition cond = fileformat.loadFile(dicom_path.c_str());
  if (cond.bad()) {
    result.message = std::string("failed to load DICOM file: ") + cond.text();
    return result;
  }

  DcmDataset* dataset = fileformat.getDataset();
  if (dataset == nullptr) {
    result.message = "DICOM file has no dataset";
    return result;
  }

  OFString sop_class;
  OFString sop_instance;
  dataset->findAndGetOFString(DCM_SOPClassUID, sop_class);
  dataset->findAndGetOFString(DCM_SOPInstanceUID, sop_instance);

  const E_TransferSyntax file_xfer = dataset->getOriginalXfer();
  if (file_xfer != EXS_Unknown) {
    result.transfer_syntax_uid = DcmXfer(file_xfer).getXferID();
  }

  result.sop_class_uid = sop_class.c_str();
  result.sop_instance_uid = sop_instance.c_str();

  DcmStorageSCU scu;
  scu.setAETitle(peer_.calling_ae.c_str());
  scu.setPeerHostName(peer_.host.c_str());
  scu.setPeerPort(peer_.port);
  scu.setPeerAETitle(peer_.called_ae.c_str());

  DicomValidator validator;
  ValidationResult validation_result = validator.validate(dicom_path);
  if (!validation_result.ok) {
    result.message = "DICOM file is not valid";
    result.dimse_status = 0x0001;
    return result;
  }
  // Uncompressed transfer syntaxes for Phase 1; compression prefs come later.
  OFList<OFString> uncompressed;
  uncompressed.push_back(UID_LittleEndianExplicitTransferSyntax);
  uncompressed.push_back(UID_LittleEndianImplicitTransferSyntax);
  uncompressed.push_back(UID_BigEndianExplicitTransferSyntax);

  cond = add_pc(scu, UID_VerificationSOPClass, uncompressed);
  if (cond.bad()) {
    result.message = std::string("addPresentationContext(Verification) failed: ") + cond.text();
    return result;
  }

  if (!sop_class.empty()) {
    cond = add_pc(scu, sop_class.c_str(), uncompressed);
    if (cond.bad()) {
      result.message = std::string("addPresentationContext(SOP Class) failed: ") + cond.text();
      return result;
    }
  }

  OFList<OFFilename> files;
  files.push_back(OFFilename(dicom_path.c_str()));
  cond = scu.addDicomFile(files.front(), ERM_fileOnly, OFTrue /* check */);
  if (cond.bad()) {
    result.message = std::string("addDicomFiles failed: ") + cond.text();
    return result;
  }

  cond = scu.initNetwork();
  if (cond.bad()) {
    result.message = std::string("initNetwork failed: ") + cond.text();
    return result;
  }

  cond = scu.negotiateAssociation();
  if (cond.bad()) {
    result.message = std::string("negotiateAssociation failed: ") + cond.text();
    scu.releaseAssociation();
    return result;
  }

  cond = scu.sendSOPInstances();
  if (cond.bad()) {
    result.message = std::string("sendSOPInstances failed: ") + cond.text();
    scu.releaseAssociation();
    return result;
  }

  // Overall send success; per-instance DIMSE detail can be refined later.
  result.dimse_status = 0x0000;
  result.ok = true;
  result.message = "C-STORE completed (" + status_hex(result.dimse_status) + ")";

  scu.releaseAssociation();
  return result;
}

}  // namespace ohs::dimse
