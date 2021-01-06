#ifndef CODEX_TEST_COMMON_H
#define CODEX_TEST_COMMON_H

#include <string>
#include <string_view>

#include <QTest>

namespace std {

/**
 * Adaptor function to display `string_view` values in test assertion messages.
 */
char *toString(string_view s) {
    return QTest::toString(
        QString::fromUtf8(s.data(),
        static_cast<int>(s.size())));
}

/**
 * Adaptor function to display `string` values in test assertion messages.
 */
char *toString(string s)
    { return toString(static_cast<std::string_view>(s)); }

}

#endif
