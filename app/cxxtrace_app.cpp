#include "cxxtrace/cxxtrace.hpp"
#include <iostream>

int main(){
  int result = cxxtrace::add_one(1);
  std::cout << "1 + 1 = " << result << std::endl;
}