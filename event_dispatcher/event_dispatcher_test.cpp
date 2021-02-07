#include "event_dispatcher_test.hpp"

#include "event_dispatcher.hpp"

void EventDispatcherTest::trigger() {
    int click_called = 0, key_called = 0;
    codex::EventDispatcher<codex::OnClick, codex::OnKey> d;
    d.add_listener<codex::OnClick>(
        [&click_called](auto e) { click_called = e.button; });
    d.add_listener<codex::OnKey>(
        [&key_called](auto e) { key_called = e.key; });
    d.trigger(codex::OnClick{.button = 42});
    QCOMPARE(click_called, 42);
    QCOMPARE(key_called, 0);
    d.trigger(codex::OnKey{.key = 43});
    QCOMPARE(click_called, 42);
    QCOMPARE(key_called, 43);
}

QTEST_MAIN(EventDispatcherTest)
