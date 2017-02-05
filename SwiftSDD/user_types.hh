//
//  user_types.hh
//  SwiftSDD
//
//  Created by Dimitri Racordon on 05.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include "sdd/sdd.hh"
#pragma clang pop


namespace swiftsdd {

template<typename C>
struct homomorphism_function {

    using values_type = typename C::Values;
    using builder_type = typename sdd::values::values_traits<values_type>::builder;

    swiftsdd_user_function fn_holder;

    values_type operator()(const values_type& values) const {
        // Convert the input set of values into a swiftsdd_uint32_array.
        auto arr = swiftsdd_uint32_array {
            .values = new uint32_t[values.size()]{},
            .size = values.size()
        };
        std::copy(values.begin(), values.end(), arr.values);

        // Call the user function.
        auto res = (*(this->fn_holder.fn))(arr, this->fn_holder.user_data);
        delete arr.values;
        arr.values = nullptr;

        // Convert back the result of the user function to a set of values.
        auto builder = builder_type();
        builder.reserve(res.size);
        for (std::size_t i = 0; i < res.size; ++i) {
            builder.insert(res.values[i]);
        }

        return std::move(builder);
    }

    bool selector() const noexcept {
        return this->fn_holder.is_a_selector;
    }

    bool shifter() const noexcept {
        return this->fn_holder.is_a_shifter;
    }

    friend bool operator==(
        const homomorphism_function<C>& lhs,
        const homomorphism_function<C>& rhs) noexcept
    {
        return
            (lhs.fn_holder.fn == rhs.fn_holder.fn) &&
            (lhs.fn_holder.user_data == rhs.fn_holder.user_data) &&
            (lhs.fn_holder.is_a_selector == rhs.fn_holder.is_a_selector) &&
            (lhs.fn_holder.is_a_shifter == rhs.fn_holder.is_a_shifter);
    }

};


} // namespace swiftsdd


namespace std {

template<typename C>
struct hash<swiftsdd::homomorphism_function<C>> {
    std::size_t operator()(const swiftsdd::homomorphism_function<C>& h) const noexcept {
        using namespace sdd::hash;
        return seed(3464152273)
            (val(h.fn_holder.fn))
            (val(h.fn_holder.user_data))
            (val(h.fn_holder.is_a_selector))
            (val(h.fn_holder.is_a_shifter));
    }
};

} // namespace std
