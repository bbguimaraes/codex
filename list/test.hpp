#ifndef CODEX_LIST_TEST_H
#define CODEX_LIST_TEST_H

#include <QTest>

class ListTest : public QObject {
    Q_OBJECT
private slots:
    void reverse_data(void);
    void reverse(void);
    void remove_data(void);
    void remove(void);
};

#endif
