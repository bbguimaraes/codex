#ifndef CODEX_MATH_POWER_TEST_H
#define CODEX_MATH_POWER_TEST_H

#include <QTest>

class PowerTest : public QObject {
    Q_OBJECT
private slots:
    void power_data(void);
    void power(void);
};

#endif
