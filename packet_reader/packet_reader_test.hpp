#ifndef CODEX_PACKET_READER_PACKET_READER_TEST_HPP
#define CODEX_PACKET_READER_PACKET_READER_TEST_HPP

#include <QTest>

class PacketReaderTest : public QObject {
    Q_OBJECT
private slots:
    void get();
    void get_error();
};

#endif
