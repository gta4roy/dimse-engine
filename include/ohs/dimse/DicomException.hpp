
#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcmetinf.h>
#include <dcmtk/ofstd/ofcond.h>


namespace ohs::dimse {

class DicomException : public std::exception {
 public:
  DicomException(const std::string& message): message(message) {}
  const char* what() const noexcept override {
    return message.c_str();
  }
 private:
  std::string message;
};

}  // namespace ohs::dimse
