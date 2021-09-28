#include "test.hpp"

extern "C" {
#include "list.h"
}

namespace {

constexpr std::size_t N = 5;

void link(struct node *l, std::size_t n) {
    if(!n--)
        return;
    for(std::size_t i = 0; i != n; ++i)
        l[i].next = l + i + 1;
}

}

void ListTest::reverse_data(void) {
    QTest::addColumn<std::size_t>("n");
    for(std::size_t n = 0; n <= N; ++n)
        QTest::newRow(QString::number(n).toUtf8().constData()) << n;
}

void ListTest::reverse(void) {
    QFETCH(const std::size_t, n);
    struct node l[N] = {{.i = 0}, {.i = 1}, {.i = 2}, {.i = 3}, {.i = 4}};
    link(l, n);
    const struct node r[N] = {
        {.next = l + 3, .i = 4},
        {.next = l + 2, .i = 3},
        {.next = l + 1, .i = 2},
        {.next = l + 0, .i = 1},
        {.i = 0},
    };
    struct node *head = list_reverse(n ? l : nullptr);
    const struct node *const rhead = r + N - n;
    for(std::size_t i = 0; i != n; ++i, head = head->next) {
        QCOMPARE(head->i, rhead[i].i);
        QCOMPARE(head->next, rhead[i].next);
    }
    QVERIFY(!head);
}

void ListTest::remove_data(void) {
    QTest::addColumn<std::size_t>("n");
    QTest::addColumn<int>("r");
    for(std::size_t n = 0; n <= N; ++n)
        for(int r = -1; r <= static_cast<int>(n); ++r)
            QTest::newRow(QString("n:%1 r:%2")
                .arg(n).arg(r).toUtf8().constData())
                << n << r;
}

void ListTest::remove(void) {
    QFETCH(const std::size_t, n);
    QFETCH(const int, r);
    struct node l[N] = {{.i = 0}, {.i = 1}, {.i = 2}, {.i = 3}, {.i = 4}};
    link(l, n);
    struct node *head = n ? l : nullptr;
    const struct node *const removed = list_remove(&head, r);
    QCOMPARE(!!removed, 0 <= r && r < static_cast<int>(n));
    int v[N] = {};
    for(std::size_t i = 0, w = 0; i != n; ++i)
        if(static_cast<int>(i) != r)
            v[w++] = i;
    for(std::size_t i = 0; i != n - !!removed; ++i, head = head->next)
        QCOMPARE(head->i, v[i]);
    QVERIFY(!head);
}

QTEST_MAIN(ListTest)
