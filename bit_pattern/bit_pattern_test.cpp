#include "bit_pattern_test.hpp"

#include "bit_pattern.hpp"

void BitPatternTest::test() {
    static_assert(BitPattern("11xxx1").matches(0b110101u));
    static_assert(!BitPattern("11xxx1").matches(0b110100u));
}

QTEST_MAIN(BitPatternTest)
