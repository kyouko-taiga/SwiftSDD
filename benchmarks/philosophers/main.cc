//
//  main.cc
//  Philo
//
//  Created by Dimitri Racordon on 06.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

#include <chrono>
#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include "sdd/sdd.hh"
#include "sdd/tools/nodes.hh"
#pragma clang pop

#define N 5


namespace philo {

template<typename C>
struct filter {

    using values_type = typename C::Values;
    using builder_type = typename sdd::values::values_traits<values_type>::builder;

    values_type operator()(const values_type& values) const {
        auto builder = builder_type();
        builder.reserve(values.size());
        for (auto value: values) {
            if (value > 0) {
                builder.insert(value);
            }
        }

        return std::move(builder);
    }

    bool selector() const noexcept {
        return true;
    }

    bool shifter() const noexcept {
        return false;
    }

    friend bool operator==(const filter<C>& lhs, const filter<C>& rhs) noexcept {
        return true;
    }

};


template<typename C>
struct consume {

    using values_type = typename C::Values;
    using builder_type = typename sdd::values::values_traits<values_type>::builder;

    values_type operator()(const values_type& values) const {
        auto builder = builder_type();
        builder.reserve(values.size());
        for (auto value: values) {
            builder.insert(value - 1);
        }

        return std::move(builder);
    }

    bool selector() const noexcept {
        return true;
    }

    bool shifter() const noexcept {
        return false;
    }

    friend bool operator==(const consume<C>& lhs, const consume<C>& rhs) noexcept {
        return true;
    }

};


template<typename C>
struct produce {

    using values_type = typename C::Values;
    using builder_type = typename sdd::values::values_traits<values_type>::builder;

    values_type operator()(const values_type& values) const {
        auto builder = builder_type();
        builder.reserve(values.size());
        for (auto value: values) {
            builder.insert(value + 1);
        }

        return std::move(builder);
    }

    bool selector() const noexcept {
        return true;
    }

    bool shifter() const noexcept {
        return false;
    }

    friend bool operator==(const produce<C>& lhs, const produce<C>& rhs) noexcept {
        return true;
    }

};


} // namespace philo


namespace std {

    template<typename C>
    struct hash<philo::filter<C>> {
        std::size_t operator()(const philo::filter<C>& h) const noexcept {
            return 0;
        }
    };
    
    template<typename C>
    struct hash<philo::consume<C>> {
        std::size_t operator()(const philo::consume<C>& h) const noexcept {
            return 0;
        }
    };
    
    template<typename C>
    struct hash<philo::produce<C>> {
        std::size_t operator()(const philo::produce<C>& h) const noexcept {
            return 0;
        }
    };
    
} // namespace std


struct conf: public sdd::flat_set_default_configuration {

    using Identifier = uint32_t;
    using Values = sdd::values::flat_set<uint32_t>;
    
};


// ---------------------------------------------------------------------------


using hom = sdd::homomorphism<conf>;
using values_type = conf::Values;


int main(int argc, const char * argv[]) {
    // Initialize the library.
    auto manager = sdd::init<conf>();

    // Create a variable ordering.
    auto builder = sdd::order_builder<conf>();
    for (uint32_t i = 0; i < 3 * N; ++i) {
        builder.push(i);
    }
    auto order = sdd::order<conf>(builder);

    // Create the transition homomorphisms.
    std::vector<hom> homomorphisms;

    for (uint32_t i = 0; i < N; ++i) {
        uint32_t think      = i;
        uint32_t eat        = (1 * N) + i;
        uint32_t left_fork  = (2 * N) + i;
        uint32_t right_fork = (2 * N) + ((i + 1) % N);

        // Encode transition 'think -> eat'.

        // Precondition
        auto te_check = std::vector<hom> {
            sdd::function(order, left_fork, philo::filter<conf> {}),
            sdd::function(order, right_fork, philo::filter<conf> {}),
            sdd::function(order, think, philo::filter<conf> {})
        };
        auto te_pre = sdd::composition(
            sdd::function(order, left_fork, philo::consume<conf> {}), sdd::composition(
            sdd::function(order, right_fork, philo::consume<conf> {}), sdd::composition(
            sdd::function(order, think, philo::consume<conf> {}),
            sdd::intersection(order, te_check.begin(), te_check.end()))));

        // Postcondition
        auto te_post = sdd::function(order, eat, philo::produce<conf> {});

        homomorphisms.push_back(sdd::composition(te_post, te_pre));

        // Encode transition 'eat -> think'.

        // Precondition.
        auto et_check = sdd::function(order, eat, philo::filter<conf> {});
        auto et_pre = sdd::composition(
            sdd::function(order, eat, philo::consume<conf> {}), et_check);

        // Postcondition.
        auto et_post = sdd::composition(
            sdd::function(order, left_fork, philo::produce<conf> {}), sdd::composition(
            sdd::function(order, right_fork, philo::produce<conf> {}),
            sdd::function(order, think, philo::produce<conf> {})));

        homomorphisms.push_back(sdd::composition(et_post, et_pre));
    }

    homomorphisms.push_back(sdd::id<conf>());
    auto transitions = sdd::fixpoint(
        sdd::sum(order, homomorphisms.begin(), homomorphisms.end()));
    transitions = sdd::rewrite(order, transitions);

    // Create the initial state.
    auto m0 = sdd::SDD<conf>(order, [](uint32_t identifier) {
        if (identifier < N) {
            return values_type {1};
        } else if (identifier < 2 * N) {
            return values_type {0};
        } else {
            return values_type {1};
        }
    });
    
    // Compute the state space.
    auto start = std::chrono::system_clock::now();
    auto ss = transitions(order, m0);
    auto end = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    
    // Print results.
    std::cout << ss.size() << " states ("
              << sdd::tools::nodes(ss).first << " nodes) in "
              << double(elapsed) / 1000 << " seconds" << std::endl;

    return 0;
}
