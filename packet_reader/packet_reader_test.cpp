#include "packet_reader_test.hpp"

#include <utility>

#include "packet_reader.hpp"

namespace {

struct foo { int x = {}, y = {}; float z = {}; };

template<typename ...Ts>
void fill_buffer(std::span<char> s, Ts ...ts) {
    auto f = [p = s.data()]<codex::Readable T>(T v) mutable {
        constexpr auto t = codex::type_to_enum<T>;
        std::memcpy(std::exchange(p, p + sizeof(t)), &t, sizeof(t));
        std::memcpy(std::exchange(p, p + sizeof(v)), &v, sizeof(v));
    };
    (..., f(ts));
}

}

void PacketReaderTest::get() {
    codex::Packet packet;
    fill_buffer(packet.data(), 42, 43, 44.0f);
    foo f;
    QVERIFY(packet.get(&f.x, &f.y, &f.z));
    QCOMPARE(f.x, 42);
    QCOMPARE(f.y, 43);
    QCOMPARE(f.z, 44.0f);
}

void PacketReaderTest::get_error() {
    codex::Packet packet;
    foo f;
    fill_buffer(packet.data(), 42, 43, 44.0f);
    QVERIFY(packet.get(&f.x, &f.y, &f.z));
    fill_buffer(packet.data(), 42, 43, 44);
    QVERIFY(!packet.get(&f.x, &f.y, &f.z));
}

QTEST_MAIN(PacketReaderTest)
