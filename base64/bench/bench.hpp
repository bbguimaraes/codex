#ifndef CODEX_BASE64_BENCH_H
#define CODEX_BASE64_BENCH_H

#include <random>
#include <vector>

#include <QTest>

class Base64Bench : public QObject {
    Q_OBJECT
    std::mt19937 gen;
    std::vector<char> buffer0, buffer1;
public:
    Base64Bench();
private slots:
    void base64_encode_data();
    void base64_encode();
    void base64_decode_data();
    void base64_decode();
    void rfc_encode_data();
    void rfc_encode();
    void rfc_decode_data();
    void rfc_decode();
    void system_exec_data();
    void system_exec();
    void base64_exec_data();
    void base64_exec();
    void rfc_exec_data();
    void rfc_exec();
};

#endif
