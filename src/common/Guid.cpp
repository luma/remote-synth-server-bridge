#include <string>
#include "common/Guid.h"

GuidGenerator guidGenerator;

Guid NewGuid() {
  return guidGenerator.newGuid();
}

std::string NewGuidStr() {
  std::stringstream stream;
  stream << NewGuid();
  return stream.str();
}
