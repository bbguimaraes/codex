#include "test.hpp"

#include "test/common.hpp"

#include "base64.hpp"
#include "cmd.hpp"

#include "rfc/adaptor.hpp"

#include "common.hpp"

namespace {

void test_data(const std::vector<std::size_t> &sizes) {
    QTest::addColumn<std::size_t>("dec_size");
    std::string fmt = {};
    for(auto x : sizes) {
        fmt = std::to_string(x);
        QTest::newRow(fmt.c_str()) << x;
    }
}

}

Q_DECLARE_METATYPE(std::string_view)

Base64Test::Base64Test() :
    gen(std::random_device{}()),
    sizes(Base64Test::N_SAMPLES)
{
    std::generate(
        begin(this->sizes), end(this->sizes),
        std::bind_front(
            std::uniform_int_distribution<std::size_t>(0, Base64Test::MAX_SIZE),
            this->gen));
}

void Base64Test::rfc_4648_data() {
    QTest::addColumn<std::string_view>("input");
    QTest::addColumn<std::string_view>("output");
    QTest::newRow("") << ""sv << ""sv;
    QTest::newRow("f") << "f"sv << "Zg=="sv;
    QTest::newRow("fo") << "fo"sv << "Zm8="sv;
    QTest::newRow("foo") << "foo"sv << "Zm9v"sv;
    QTest::newRow("foob") << "foob"sv << "Zm9vYg=="sv;
    QTest::newRow("fooba") << "fooba"sv << "Zm9vYmE="sv;
    QTest::newRow("foobar") << "foobar"sv << "Zm9vYmFy"sv;
}

void Base64Test::rfc_4648() {
    QFETCH(const std::string_view, input);
    QFETCH(const std::string_view, output);
    std::vector<char> input_v(round_to_block_size<3>(input.size()));
    std::vector<char> encoded_v(output_size<4, 3>(input_v.size()));
    std::copy(begin(input), end(input), begin(input_v));
    std::copy(begin(output), end(output), begin(encoded_v));
    Base64::encode({input_v.data(), input.size()}, encoded_v.data());
    std::string_view ret = {encoded_v.data(), encoded_v.size()};
    QCOMPARE(ret, output);
    std::vector<char> decoded_v(input_v.size());
    Base64::decode({encoded_v.data(), output.size()}, decoded_v.data());
    ret = {decoded_v.data(), input.size()};
    QCOMPARE(ret, input);
}

void Base64Test::base64_encode_data() { test_data(this->sizes); }
void Base64Test::base64_decode_data() { test_data(this->sizes); }
void Base64Test::rfc_4648_encode_data() { test_data(this->sizes); }
void Base64Test::rfc_4648_decode_data() { test_data(this->sizes); }

void Base64Test::base64_encode()
    { encode_test<Base64, Base64RFC, false>(&this->gen); }
void Base64Test::base64_decode()
    { decode_test<Base64, Base64RFC, false>(&this->gen); }
void Base64Test::rfc_4648_encode()
    { encode_test<Base64RFC, Base64, false>(&this->gen); }
void Base64Test::rfc_4648_decode()
    { decode_test<Base64RFC, Base64, false>(&this->gen); }

void Base64Test::invalid() {
    QVERIFY(!Base64::decode("____", std::array<char, 3>{}.data()));
    QVERIFY(!Base64::decode("a___", std::array<char, 3>{}.data()));
    QVERIFY(!Base64::decode("aa__", std::array<char, 3>{}.data()));
    QVERIFY(!Base64::decode("aaa_", std::array<char, 3>{}.data()));
}

void Base64Test::pad_data() {
    QTest::addColumn<std::string_view>("input");
    QTest::addColumn<std::string_view>("expected");
    QTest::newRow("a") << "YQo="sv << "a"sv;
    QTest::newRow("aaa") << "YWFhCG=="sv << "aaa"sv;
}

void Base64Test::pad() {
    QFETCH(const std::string_view, input);
    QFETCH(const std::string_view, expected);
    std::vector<char> output(8);
    const auto ret = Base64::decode(input, output.data());
    QVERIFY(ret);
    QVERIFY(ret > output.data());
    const auto n = static_cast<std::size_t>(ret - output.data() - 1);
    QCOMPARE((std::string_view{output.data(), n}), expected);
}

QTEST_MAIN(Base64Test)
