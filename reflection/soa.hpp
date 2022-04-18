#ifndef CODEX_REFLECTION_SOA_HPP
#define CODEX_REFLECTION_SOA_HPP

#include <span>

#include "detail/soa.hpp"
#include "fields.hpp"

namespace codex::refl {

/**
 * Transparently stores \p T objects as contiguous arrays of each field.
 * The storage is a "transposition" of a `T[]`: a structure of arrays instead of
 * an array of structures.  This allows each field array to be processed
 * sequentially (e.g. in SIMD fashion).
 *
 * An interface similar to `std::vector` is also provided to add/set/remove
 * elements of type \p T as if that were the actual storage.  Access methods are
 * also provided, although they return values that are manufactured on demand,
 * not references as is the case for `std::vector`.
 */
template<typename T, typename S = detail::storage_descriptor>
class SOA : detail::storage<T, S> {
    using base_type = detail::storage<T, S>;
public:
    /** (Value/decayed) type of field with index \p I. */
    // Should be `base_type::value_type`, but that generates an ICE(!?).
    template<std::size_t I>
    using field_type =
        std::decay_t<std::tuple_element_t<I, field_tuple_t<T>>>;
    // Container interface.
    std::size_t size(void) const { return this->field<0>().size(); }
    void push_back(T &&t = {});
    // Idiosyncratic container interface.
    /**
     * Indexed access to an element.
     * The returned value (n.b.: not a reference) is manufactured from the
     * values of each field gathered from the storage.
     */
    T operator[](std::size_t i) const;
    /** Stores the value of each field of \p t. */
    void set(std::size_t i, const T &t);
    /** Stores the value of each field of \p t. */
    void set(std::size_t i, T &&t);
    /** Provides access to the contiguous storage of field \p I. */
    template<std::size_t I> std::span<field_type<I>> field(void);
    template<std::size_t I> std::span<const field_type<I>> field(void) const;
};

template<typename T, typename S>
T SOA<T, S>::operator[](std::size_t i) const {
    T ret = {};
    this->for_each_i(i, [t = field_tuple(ret)](auto fi, auto &x) {
        std::get<fi()>(t) = x;
    });
    return ret;
}

template<typename T, typename S>
void SOA<T, S>::push_back(T &&t) {
    using detail::field_storage;
    this->for_each([&t]<typename F, std::size_t I>(field_storage<S, I, F> &v) {
        v.push_back(std::get<I>(field_tuple(CODEX_FWD(t))));
    });
}

template<typename T, typename S>
void SOA<T, S>::set(std::size_t i, const T &t) {
    this->for_each_i(i, [&t](auto vi, auto &x) {
        x = std::get<vi()>(field_tuple(t));
    });
}

template<typename T, typename S>
void SOA<T, S>::set(std::size_t i, T &&t) {
    this->for_each_i(i, [&t](auto vi, auto &x) {
        x = std::get<vi()>(field_tuple(std::move(t)));
    });
}

template<typename T, typename S>
template<std::size_t I>
auto SOA<T, S>::field(void) -> std::span<field_type<I>> {
    using F = field_type<I>;
    return *static_cast<detail::field_storage<S, I, F>*>(this);
}

template<typename T, typename S>
template<std::size_t I>
auto SOA<T, S>::field(void) const -> std::span<const field_type<I>> {
    using F = field_type<I>;
    return *static_cast<const detail::field_storage<S, I, F>*>(this);
}

}

#endif
