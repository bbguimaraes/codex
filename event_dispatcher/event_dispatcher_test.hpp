#ifndef CODEX_EVENT_DISPATCHER_EVENT_DISPATCHER_TEST_HPP
#define CODEX_EVENT_DISPATCHER_EVENT_DISPATCHER_TEST_HPP

#include <QTest>

class EventDispatcherTest : public QObject {
    Q_OBJECT
private slots:
    void trigger();
};

#endif
