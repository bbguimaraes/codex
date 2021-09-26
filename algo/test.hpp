#ifndef CODEX_ALGO_TEST_H
#define CODEX_ALGO_TEST_H

#include <QTest>

class AlgoTest : public QObject {
    Q_OBJECT
private slots:
    void rotate_empty();
    void rotate_end();
    void rotate_empty_end();
    void rotate_perm_data();
    void rotate_perm();
    void rotate_rec_empty();
    void rotate_rec_end();
    void rotate_rec_empty_end();
    void rotate_rec_perm_data();
    void rotate_rec_perm();
};

#endif
