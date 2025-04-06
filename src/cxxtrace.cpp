#include "cxxtrace/cxxtrace.hpp"

namespace cxxtrace {

void beginScopeSection(Tag tag, source_location loc) {
  ProcessContext::inst().current().enter({loc, tag});
}

void endScopeSection() {
  ProcessContext::inst().current().exit();
}

int add_one(int x){
  return x + 1;
}

} // namespace cxxtrace