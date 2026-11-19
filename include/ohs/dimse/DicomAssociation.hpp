#pragma once
#include "ohs/dimse/types.hpp"
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcxfer.h>
#include <dcmtk/dcmnet/dstorscu.h>
#include <dcmtk/ofstd/ofstring.h>
#include <dcmtk/ofstd/oflist.h>
#include <dcmtk/ofstd/offile.h>

#include <string>
#include <iostream>

using namespace std;
using namespace ohs::dimse;

namespace ohs::dimse {

/// Reads a DICOM image from disk and checks that DCMTK accepts it.
///
/// Public headers stay free of DCMTK types so callers never depend on DCMTK
/// includes. All DCMTK usage lives in the .cpp.
class DicomAssociation {
 public:
  /// Create a new DicomAssociation.
  ///
  /// The association is created with the given peer information.
  DicomAssociation(DcmSCU & scu, const string& host, const uint16_t port, const string& calling_ae, const string& called_ae):
                                    scu(scu),
                                    host(host), 
                                    port(port), 
                                    calling_ae(calling_ae), 
                                    called_ae(called_ae){

                                        remoteNode = new Peer();
                                        remoteNode->host = host;
                                        remoteNode->port = port;
                                        remoteNode->calling_ae = calling_ae;
                                        remoteNode->called_ae = called_ae;

                                    }

  void createAssociation();
  void addPresentationContext(const string& sopClassUid);
  bool negotiateAssociation();

  /// Release the association.
  void releaseAssociation();

  //Destructor
  ~DicomAssociation() {
    delete remoteNode;
  }

 private:

  void validateDicomEntries();

  Peer * remoteNode;
  DcmSCU & scu;   // base class for all SCU classes

  const string &host;
  const uint16_t port;
  const string &calling_ae;
  const string &called_ae;
  OFList<OFString> transferSyntaxes;
  
};

}  // namespace ohs::dimse
