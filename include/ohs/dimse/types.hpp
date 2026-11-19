#pragma once

#include <cstdint>
#include <string>

namespace ohs::dimse {

/// Remote Storage SCP endpoint and AE titles used for A-ASSOCIATE.
struct Peer {
  std::string host;
  std::uint16_t port{11112};
  std::string called_ae{"ANY-SCP"};
  std::string calling_ae{"OHS-SCU"};
};

/// Outcome of a single C-STORE attempt.
struct StoreResult {
  bool ok{false};
  std::uint16_t dimse_status{0xFFFF};
  std::string sop_class_uid;
  std::string sop_instance_uid;
  std::string transfer_syntax_uid;
  std::string message;
};

}  // namespace ohs::dimse
