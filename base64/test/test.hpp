#ifndef CODEX_BASE64_TEST_H
#define CODEX_BASE64_TEST_H

#include <random>
#include <vector>

#include <QTest>

class Base64Test : public QObject {
    Q_OBJECT
    static constexpr std::size_t N_SAMPLES = 8, MAX_SIZE = 1 << 13;
    std::mt19937 gen;
    std::vector<std::size_t> sizes;
public:
    Base64Test();
private slots:
    void rfc_4648_data();
    void rfc_4648();
    void base64_encode_data();
    void base64_encode();
    void base64_decode_data();
    void base64_decode();
    void rfc_4648_encode_data();
    void rfc_4648_encode();
    void rfc_4648_decode_data();
    void rfc_4648_decode();
    void invalid();
    void pad_data();
    void pad();
};

#endif
