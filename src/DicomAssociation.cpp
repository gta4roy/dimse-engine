#include "ohs/dimse/DicomAssociation.hpp"
#include "dcmtk/dcmdata/dcuid.h"
#include "ohs/dimse/DicomException.hpp"

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/ofstd/ofcond.h>


namespace ohs::dimse {

void DicomAssociation::addPresentationContext(const string& sopClassUid) {
  // add presentation context

  this->transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
  this->transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
  this->transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);

  OFCondition cond = this->scu.addPresentationContext( sopClassUid.c_str(), this->transferSyntaxes);
  if (cond.bad()) {
    throw DicomException("addPresentationContext(Verification) failed: " + std::string(cond.text()));
  }
}

void DicomAssociation::createAssociation() {
  // create association
  validateDicomEntries();
  this->scu.setPeerPort(remoteNode->port);
  this->scu.setPeerAETitle(remoteNode->calling_ae.c_str());
  this->scu.setPeerHostName(remoteNode->host.c_str());
  this->scu.setPeerAETitle(remoteNode->called_ae.c_str());
  this->scu.setPeerHostName(remoteNode->host.c_str());


  //TODO pass all the required presentation contexts
  addPresentationContext(UID_VerificationSOPClass);
  addPresentationContext(UID_StorageServiceClass);
  addPresentationContext(UID_MRImageStorage);
  addPresentationContext(UID_CTImageStorage);

}

bool DicomAssociation::negotiateAssociation() {

    OFCondition initNetworkCond = this->scu.initNetwork();
    if (initNetworkCond.bad()) {
        throw DicomException("initNetwork failed: " + std::string(initNetworkCond.text()));
    }
    // negotiate association
    OFCondition cond = this->scu.negotiateAssociation();
    if (cond.bad()) {
        throw DicomException("negotiateAssociation failed: " + std::string(cond.text()));
    }
    return true;
}

void DicomAssociation::releaseAssociation() {
  // release association
  OFCondition cond = this->scu.releaseAssociation();
  if (cond.bad()) {
    throw DicomException("releaseAssociation failed: " + std::string(cond.text()));
  }
}

void DicomAssociation::validateDicomEntries() {
  // validate dicom entries
  if(remoteNode == nullptr) {
    throw DicomException("peer is null");
  }
  if(remoteNode->host.empty()) {
    throw DicomException("host is empty");
  }
  if(remoteNode->port == 0) {
    throw DicomException("port is 0");
  }
  if(remoteNode->calling_ae.empty()) {
    throw DicomException("calling_ae is empty");
  }
  if(remoteNode->called_ae.empty()) {
    throw DicomException("called_ae is empty");
  }
}

}  // namespace ohs::dimse
