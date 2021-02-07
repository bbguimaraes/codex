#ifndef CODEX_ARITHMETIC_FOLDING_ROTATE_TEST_HPP
#define CODEX_ARITHMETIC_FOLDING_ROTATE_TEST_HPP

#include <QTest>

class RotateTest : public QObject {
    Q_OBJECT
private slots:
    void rotate();
    void rotate_y();
};

#endif
