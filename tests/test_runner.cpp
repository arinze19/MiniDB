#include "test_framework.h"

// All test files register themselves automatically via static initializers
// We just need to include them (done via CMake GLOB) and run!

int main() {
    return runAllTests();
}