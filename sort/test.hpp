#ifndef CODEX_SORT_TEST_H
#define CODEX_SORT_TEST_H

#include <QTest>

class SortTest : public QObject {
    Q_OBJECT
private slots:
    void selection_data(void);
    void selection(void);
    void insertion_data(void);
    void insertion(void);
};

#endif
