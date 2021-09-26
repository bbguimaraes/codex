#include "test.hpp"

#include <list>

#include "rotate.hpp"

namespace {

using list = std::list<int>;
using value_type = list::value_type;
using iterator = list::iterator;

constexpr auto rotate = codex::rotate<iterator, iterator>;
constexpr auto rotate_rec = codex::rotate_rec<iterator, iterator>;

const list L = {0, 1, 2, 3, 4, 5, 6, 7};
const auto N = L.size();

template<auto rotate, typename T>
bool check(T v, std::size_t b, std::size_t n, std::size_t e) {
    assert(b <= N);
    assert(n <= N);
    assert(e <= N);
    auto vc = v;
    const auto vb = begin(vc);
    const value_type i = static_cast<value_type>((b == n || n == e) ? 0 : n);
    rotate(std::next(vb, b), std::next(vb, n), std::next(vb, e));
    return std::ranges::all_of(vc, [i = i](auto x) mutable {
        return x == std::exchange(i, (i + 1) % N);
    });
}

template<auto f> void rotate_empty() { QVERIFY(check<f>(L, 0, 0, 0)); }
template<auto f> void rotate_end() { QVERIFY(check<f>(L, N, N, N)); }
template<auto f> void rotate_empty_end() { QVERIFY(check<f>(L, N - 1, N, N)); }

template<auto f> void rotate_perm() {
    QFETCH(const std::size_t, n);
    QVERIFY(check<f>(L, 0, n, L.size()));
}

void rotate_perm_data() {
    QTest::addColumn<std::size_t>("n");
    for(std::size_t i = 0; i < N; ++i)
        QTest::newRow(QString::number(i).toUtf8().constData()) << i;
}

}

void AlgoTest::rotate_empty() { ::rotate_empty<::rotate>(); }
void AlgoTest::rotate_end() { ::rotate_end<::rotate>(); }
void AlgoTest::rotate_empty_end() { ::rotate_empty_end<::rotate>(); }
void AlgoTest::rotate_perm() { ::rotate_perm<::rotate>(); }
void AlgoTest::rotate_perm_data() { ::rotate_perm_data(); }

void AlgoTest::rotate_rec_empty() { ::rotate_empty<::rotate_rec>(); }
void AlgoTest::rotate_rec_end() { ::rotate_end<::rotate_rec>(); }
void AlgoTest::rotate_rec_empty_end() { ::rotate_empty_end<::rotate_rec>(); }
void AlgoTest::rotate_rec_perm() { ::rotate_perm<::rotate_rec>(); }
void AlgoTest::rotate_rec_perm_data() { ::rotate_perm_data(); }

QTEST_MAIN(AlgoTest)
