#include "bench.hpp"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <random>

#include <QProcess>
#include <QTemporaryFile>

#include "../../bench/bench.hpp"

#include "base64.hpp"
#include "cmd.hpp"

#include "rfc/adaptor.hpp"

#include "base64/test/common.hpp"

namespace {

constexpr std::size_t BUFFER_SIZE = 1024;

void test_data() {
    QTest::addColumn<std::size_t>("dec_size");
    if(const auto *env = std::getenv(codex::BENCHMARK_SIZES_ENV)) {
        QVERIFY(codex::set_benchmark_sizes_from_env(env));
        return;
    }
    std::string fmt = {};
    for(auto i : {5, 10, 15, 20, 25, 30}) {
        fmt = "2^" + std::to_string(i);
        QTest::newRow(fmt.c_str())
            << (std::size_t{1} << static_cast<std::size_t>(i));
    }
}

bool write_rnd_file(
    std::mt19937 *rnd, QTemporaryFile *f, char *buffer, std::size_t size
) {
    const auto gen =
        [&rnd, d = std::uniform_int_distribution<uint8_t>()]() mutable
            { return d(*rnd); };
    auto isize = static_cast<qint64>(size);
    while(isize) {
        auto n = std::min(isize, static_cast<qint64>(BUFFER_SIZE));
        isize -= n;
        std::generate(buffer, buffer + n, gen);
        auto *p = buffer;
        while(n) {
            const auto nw = f->write(p, n);
            if(nw == -1)
                return false;
            n -= nw;
            p += nw;
        }
    }
    f->flush();
    f->seek(0);
    return true;
}

bool exec_cmd(
    const QStringList &args, const QString &input, const QString &output
) {
    QProcess p;
    p.setProcessChannelMode(QProcess::ForwardedErrorChannel);
    p.setStandardInputFile(input);
    p.setStandardOutputFile(output);
    p.start("base64", args);
    return p.waitForFinished(-1);
}

bool read_all(QTemporaryFile *f, char *buffer, qint64 n) {
    while(n) {
        const auto nr = f->read(buffer, n);
        if(nr == -1)
            return false;
        n -= nr;
        buffer += nr;
    }
    return true;
}

bool files_equal(
    QTemporaryFile *f0, QTemporaryFile *f1, char *buffer0, char *buffer1
) {
    auto size = f0->size();
    if(size != f1->size())
        return false;
    while(size) {
        auto n = std::min(size, static_cast<qint64>(BUFFER_SIZE));
        if(!(read_all(f0, buffer0, n) && read_all(f1, buffer1, n)))
            return false;
        if(std::memcmp(buffer0, buffer1, static_cast<std::size_t>(n)) != 0)
            return false;
        size -= n;
    }
    return true;
}

void exec_test(std::mt19937 *rnd, char *buffer0, char *buffer1) {
    QFETCH(std::size_t, dec_size);
    dec_size = round_to_block_size<3>(dec_size);
    QTemporaryFile input, encoded, decoded;
    QVERIFY(input.open());
    QVERIFY(encoded.open());
    QVERIFY(decoded.open());
    QVERIFY(write_rnd_file(rnd, &input, buffer0, dec_size));
    QBENCHMARK {
        QVERIFY(exec_cmd({}, input.fileName(), encoded.fileName()));
        QVERIFY(exec_cmd({"-d"}, encoded.fileName(), decoded.fileName()));
    }
    QCOMPARE(encoded.size(), (output_size<4, 3>(dec_size)));
    QCOMPARE(decoded.size(), dec_size);
    QVERIFY(files_equal(&input, &decoded, buffer0, buffer1));
}

}

Q_DECLARE_METATYPE(std::string_view)

Base64Bench::Base64Bench() :
    gen(std::mt19937{std::random_device{}()}),
    buffer0(BUFFER_SIZE),
    buffer1(BUFFER_SIZE) {}

void Base64Bench::base64_encode_data() { test_data(); }
void Base64Bench::base64_decode_data() { test_data(); }
void Base64Bench::rfc_encode_data() { test_data(); }
void Base64Bench::rfc_decode_data() { test_data(); }
void Base64Bench::system_exec_data() { test_data(); }
void Base64Bench::base64_exec_data() { test_data(); }
void Base64Bench::rfc_exec_data() { test_data(); }

#define T(f, T, C) f ## _test<T, C, true>(&this->gen)
void Base64Bench::base64_encode() { T(encode, Base64, Base64RFC); }
void Base64Bench::base64_decode() { T(decode, Base64, Base64RFC); }
void Base64Bench::rfc_encode() { T(encode, Base64RFC, Base64); }
void Base64Bench::rfc_decode() { T(decode, Base64RFC, Base64); }
#undef T

#define T exec_test(&this->gen, this->buffer0.data(), this->buffer1.data());
void Base64Bench::system_exec() { T }
void Base64Bench::base64_exec() { T }
void Base64Bench::rfc_exec() { T }
#undef T

QTEST_MAIN(Base64Bench)
