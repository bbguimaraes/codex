#ifndef CODEX_BIT_PATTERN_H
#define CODEX_BIT_PATTERN_H

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

class BitPattern {
public:
    template<std::size_t N>
    explicit consteval BitPattern(const char (&pattern)[N]) {
        for(const auto *c = pattern; *c; ++c) {
            this->mask <<= 1;
            this->expected <<= 1;
            switch(*c) {
            case 'x':
            case 'X': break;
            case '1': this->expected |= 1; [[fallthrough]];
            case '0': this->mask |= 1; break;
            default: throw std::domain_error("invalid input");
            }
        }
    }

    constexpr bool matches(std::unsigned_integral auto v)
        { return (v & this->mask) == this->expected; }
private:
    std::uint64_t mask = {}, expected = {};
};

#endif
