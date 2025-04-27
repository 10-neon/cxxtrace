#include <iostream>

#include "cxxtrace/cxxtrace.h"
#include "version.h"

int main() {
    std::cout << buildinfo::version << std::endl;
    std::cout << neon::hello_world() << std::endl;
    return 0;
}
