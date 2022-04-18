#ifndef CODEX_REFLECTION_SOA_DETAIL_HPP
#define CODEX_REFLECTION_SOA_DETAIL_HPP

#include <cstddef>
#include <tuple>
#include <type_traits>
#include <vector>

#include "../fields.hpp"
#include "utils.hpp"

namespace codex::refl::detail {

/**
 * Ultimate class for the storage of each field.
 * \tparam S The storage descriptor.
 * \tparam T The type of the field.
 * \tparam I Index of the type in the containing structure.
 */
template<typename S, std::size_t I, typename T>
class field_storage : public S::for_field<I, T> {
protected:
    static constexpr auto index = I;
};

template<typename S, typename Is, typename Ts> struct storage_impl;

/**
 * Storage for all fields of a structure.
 * \tparam S Storage descriptor.
 * \tparam Is Indices for each field of the structure.
 * \tparam Ts Types for each field of the structure.
 * \see storage
 */
template<typename S, std::size_t ...Is, typename ...Ts>
class storage_impl<S, std::index_sequence<Is...>, std::tuple<Ts...>>
    : public field_storage<S, Is, std::decay_t<Ts>>...
{
private:
    /** Tuple with (value/decayed) types of each field. */
    using tuple_type = std::tuple<std::decay_t<Ts>...>;
    /** (Value/decayed) type of field with index \p I. */
    template<std::size_t I>
    using value_type = std::tuple_element_t<I, tuple_type>;
    /**
     * Storage type of field with index \p I.
     * Used to cast this object to one of its bases to get access to the field
     * storage.
     */
    template<std::size_t I>
    using field_storage_type = field_storage<S, I, std::decay_t<value_type<I>>>;
public:
    /** Storage for field with index \p I. */
    template<std::size_t I>
    auto field(void) -> field_storage_type<I>& { return *this; }
    template<std::size_t I>
    auto field(void) const -> const field_storage_type<I>& { return *this; }
    /**
     * Applies \p f to each field storage.
     * \param f
     *     Callable which takes a field storage (i.e. \ref field_storage
     *     instantiated with <tt><I, T></tt>) as a parameter.  The argument type
     *     can be used to detect the field type and index.
     */
    void for_each(auto &&f) { (..., CODEX_FWD(f)(this->field<Is>())); }
    void for_each(auto &&f) const { (..., CODEX_FWD(f)(this->field<Is>())); }
    /**
     * Applies \p f to each field of the element with index \p i.
     * \param f
     *     Callable which takes two parameters: an \ref index_constant
     *     containing the field index and a (mutable) reference to the field
     *     value in its corresponding storage.
     */
    void for_each_i(std::size_t i, auto &&f);
    void for_each_i(std::size_t i, auto &&f) const;
};

template<typename S, std::size_t ...Is, typename ...Ts>
void storage_impl<
    S, std::index_sequence<Is...>, std::tuple<Ts...>
>::for_each_i(std::size_t i, auto &&f) {
    return this->for_each([i, &f]<typename V>(V &v) {
        CODEX_FWD(f)(index_constant<V::index>{}, v[i]);
    });
}

template<typename S, std::size_t ...Is, typename ...Ts>
void storage_impl<
    S, std::index_sequence<Is...>, std::tuple<Ts...>
>::for_each_i(std::size_t i, auto &&f) const {
    return this->for_each([i, &f]<typename V>(V &v) {
        CODEX_FWD(f)(index_constant<V::index>{}, v[i]);
    });
}

/**
 * Storage for all fields of a structure.
 * Convenient interface to instantiate \ref storage_impl for a type \p T and
 * storage descriptor \p S.
 */
template<typename T, typename S>
using storage = storage_impl<
    S, std::make_index_sequence<refl::field_count<T>()>,
    refl::field_tuple_t<T>>;

/** Default storage descriptor.  Stores field values in `std::vector`s. */
struct storage_descriptor {
    template<std::size_t, typename T> using for_field = std::vector<T>;
};

}

#endif
