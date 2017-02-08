//
//  wrapper.cpp
//  SwiftSDD
//
//  Created by Dimitri Racordon on 03.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

#include <sstream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include "sdd/sdd.hh"
#include "sdd/tools/nodes.hh"
#pragma clang pop

#include "user_types.hh"
#include "wrapper.h"


struct conf: public sdd::flat_set_default_configuration {

    using Identifier = uint32_t;
    using Values = sdd::values::flat_set<uint32_t>;

};


extern "C" {

    // MARK: Interface for user types utilities.

    void swiftsdd_uint32_set_clear(swiftsdd_uint32_set* set) {
        delete set->data;
        set->data = nullptr;
        set->count = 0;
    }


    // MARK: Interface for sdd::init

    swiftsdd_obj* swiftsdd_init() {

        // We reimplement sdd::init here because we don't have access to the copy
        // constructor of sdd::manager, yet we need to keep the object alive after
        // we leave the scope of this function.

        using values_type = typename conf::Values;
        conf configuration {};

        if (*sdd::global_ptr<conf>() == nullptr and *sdd::global_ptr<values_type>() == nullptr) {
            auto vm_ptr = std::make_unique<sdd::values_manager<values_type>>(configuration);
            *sdd::global_values_ptr<values_type>() = vm_ptr.get();

            auto im_ptr = std::make_unique<sdd::internal_manager<conf>>(configuration);
            *sdd::global_ptr<conf>() = im_ptr.get();

            return reinterpret_cast<swiftsdd_obj*>(
                new sdd::manager_impl<conf>(std::move(vm_ptr), std::move(im_ptr)));
        } else {
            throw std::runtime_error("SDD library already initialized.");
        }
    }

    void swiftsdd_deinit(swiftsdd_obj* lib_ptr) {
        delete reinterpret_cast<sdd::manager_impl<conf>*>(lib_ptr);
    }


    // MARK: Interface for sdd::order_builder.

    swiftsdd_obj* swiftsdd_order_builder_create() {
        auto order_builder = new sdd::order_builder<conf>();
        return reinterpret_cast<swiftsdd_obj*>(order_builder);
    }

    void swiftsdd_order_builder_destroy(swiftsdd_obj* builder_ptr) {
        delete reinterpret_cast<sdd::order_builder<conf>*>(builder_ptr);
    }

    void swiftsdd_order_builder_push(swiftsdd_obj* builder_ptr, uint32_t identifier) {
        reinterpret_cast<sdd::order_builder<conf>*>(builder_ptr)->push(identifier);
    }


    // MARK: Interface fos sdd::order.

    swiftsdd_obj* swiftsdd_order_create(swiftsdd_obj* builder_ptr) {
        auto builder = reinterpret_cast<sdd::order_builder<conf>*>(builder_ptr);
        auto order = new sdd::order<conf>(*builder);
        return reinterpret_cast<swiftsdd_obj*>(order);
    }

    void swiftsdd_order_destroy(swiftsdd_obj* order_ptr) {
        delete reinterpret_cast<sdd::order<conf>*>(order_ptr);
    }


    // MARK: Interface for sdd:SDD.

    swiftsdd_obj* swiftsdd_sdd_create() {
        auto sdd = new sdd::SDD<conf>();
        return reinterpret_cast<swiftsdd_obj*>(sdd);
    }

    swiftsdd_obj* swiftsdd_sdd_create_with_order(
        swiftsdd_obj* order_ptr,
        const void* user_data,
        uint32_t (*initializer)(uint32_t, const void*))
    {
        auto order = reinterpret_cast<sdd::order<conf>*>(order_ptr);

        auto sdd = new sdd::SDD<conf>(*order, [&](uint32_t identifier) {
            return conf::Values{ (*initializer)(identifier, user_data) };
        });

        return reinterpret_cast<swiftsdd_obj*>(sdd);
    }

    void swiftsdd_sdd_destroy(swiftsdd_obj* sdd_ptr) {
        delete reinterpret_cast<sdd::SDD<conf>*>(sdd_ptr);
    }

    swiftsdd_obj* swiftsdd_sdd_zero() {
        auto sdd = new sdd::SDD<conf>();
        *sdd = sdd::zero<conf>();
        return reinterpret_cast<swiftsdd_obj*>(sdd);
    }

    swiftsdd_obj* swiftsdd_sdd_one() {
        auto sdd = new sdd::SDD<conf>();
        *sdd = sdd::one<conf>();
        return reinterpret_cast<swiftsdd_obj*>(sdd);
    }

    bool swiftsdd_sdd_eq(swiftsdd_obj* lhs_ptr, swiftsdd_obj* rhs_ptr) {
        auto lhs = reinterpret_cast<sdd::SDD<conf>*>(lhs_ptr);
        auto rhs = reinterpret_cast<sdd::SDD<conf>*>(rhs_ptr);

        return *lhs == *rhs;
    }

    size_t swiftsdd_sdd_hash(swiftsdd_obj* sdd_ptr) {
        auto sdd = reinterpret_cast<sdd::SDD<conf>*>(sdd_ptr);
        auto hasher = std::hash<sdd::SDD<conf>>();
        return hasher(*sdd);
    }

    swiftsdd_obj* swiftsdd_sdd_union(swiftsdd_obj* lhs_ptr, swiftsdd_obj* rhs_ptr) {
        auto lhs = reinterpret_cast<sdd::SDD<conf>*>(lhs_ptr);
        auto rhs = reinterpret_cast<sdd::SDD<conf>*>(rhs_ptr);

        auto res = new sdd::SDD<conf>();
        *res = *lhs + *rhs;
        return reinterpret_cast<swiftsdd_obj*>(res);
    }

    swiftsdd_obj* swiftsdd_sdd_intersection(swiftsdd_obj* lhs_ptr, swiftsdd_obj* rhs_ptr) {
        auto lhs = reinterpret_cast<sdd::SDD<conf>*>(lhs_ptr);
        auto rhs = reinterpret_cast<sdd::SDD<conf>*>(rhs_ptr);

        auto res = new sdd::SDD<conf>();
        *res = *lhs & *rhs;
        return reinterpret_cast<swiftsdd_obj*>(res);
    }

    uint64_t swiftsdd_sdd_size(swiftsdd_obj* sdd_ptr) {
        auto sdd = reinterpret_cast<sdd::SDD<conf>*>(sdd_ptr);
        return static_cast<uint64_t>(sdd->size());
    }

    const char* swiftsdd_sdd_str_create(swiftsdd_obj* sdd_ptr) {
        auto sdd = reinterpret_cast<sdd::SDD<conf>*>(sdd_ptr);

        std::ostringstream oss;
        oss << *sdd;

        auto str = oss.str();
        auto len = str.length();
        auto res = new char[len + 1]{};
        std::copy(str.c_str(), str.c_str() + len, res);

        return res;
    }

    void swiftsdd_sdd_str_destroy(const char* str_ptr) {
        delete str_ptr;
    }


    // MARK: Interface for SDD paths

    swiftsdd_obj* swiftsdd_path_generator_create(swiftsdd_obj* sdd_ptr) {
        auto sdd = reinterpret_cast<sdd::SDD<conf>*>(sdd_ptr);
        auto generator = new sdd::path_generator<conf>(sdd->paths());
        return reinterpret_cast<swiftsdd_obj*>(generator);
    }

    void swiftsdd_path_generator_destroy(swiftsdd_obj* generator_ptr) {
        delete reinterpret_cast<sdd::path_generator<conf>*>(generator_ptr);
    }

    swiftsdd_obj* swiftsdd_path_generator_get(swiftsdd_obj* generator_ptr, bool* did_end) {
        auto generator = reinterpret_cast<sdd::path_generator<conf>*>(generator_ptr);

        if (*generator) {
            // Fetch the next values from the coroutine.
            auto path = new std::vector<conf::Values>(generator->get());
            (*generator)();

            *did_end = false;
            return reinterpret_cast<swiftsdd_obj*>(path);
        }

        *did_end = true;
        return nullptr;
    }

    void swiftsdd_path_destroy(swiftsdd_obj* path_ptr) {
        delete reinterpret_cast<std::vector<conf::Values>*>(path_ptr);
    }

    size_t swiftsdd_path_size(swiftsdd_obj* path_ptr) {
        auto path = reinterpret_cast<std::vector<conf::Values>*>(path_ptr);
        return path->size();
    }

    swiftsdd_uint32_set swiftsdd_path_at(swiftsdd_obj* path_ptr, size_t index) {
        auto path = reinterpret_cast<std::vector<conf::Values>*>(path_ptr);
        auto values = path->at(index);

        // Note that it's the caller's responsibility should deallocated this
        // memory.
        auto res = swiftsdd_uint32_set {
            .data = new uint32_t[values.size()]{},
            .count = values.size()
        };
        std::copy(values.begin(), values.end(), res.data);

        return res;
    }


    // MARK: Interface for sdd::homomorphism

    swiftsdd_obj* swiftsdd_hom_identity_create() {
        auto hom = new sdd::homomorphism<conf>(sdd::id<conf>());
        return reinterpret_cast<swiftsdd_obj*>(hom);
    }

    swiftsdd_obj* swiftsdd_hom_constant_create(swiftsdd_obj* sdd_ptr) {
        auto sdd = reinterpret_cast<sdd::SDD<conf>*>(sdd_ptr);
        auto hom = new sdd::homomorphism<conf>(sdd::constant(*sdd));
        return reinterpret_cast<swiftsdd_obj*>(hom);
    }

    swiftsdd_obj* swiftsdd_hom_cons_create(
        swiftsdd_obj* order_ptr,
        swiftsdd_uint32_set valuation,
        swiftsdd_obj* hom_ptr)
    {
        auto order = reinterpret_cast<sdd::order<conf>*>(order_ptr);
        auto next_hom = reinterpret_cast<sdd::homomorphism<conf>*>(hom_ptr);

        auto builder = sdd::values::values_traits<conf::Values>::builder();
        builder.reserve(valuation.count);
        for (std::size_t i = 0; i < valuation.count; ++i) {
            builder.insert(valuation.data[i]);
        }
        auto values = conf::Values(std::move(builder));

        auto hom = new sdd::homomorphism<conf>(sdd::cons(*order, values, *next_hom));
        return reinterpret_cast<swiftsdd_obj*>(hom);
    }

    swiftsdd_obj* swiftsdd_hom_fixpoint_create(swiftsdd_obj* hom_ptr) {
        auto f = reinterpret_cast<sdd::homomorphism<conf>*>(hom_ptr);
        auto hom = new sdd::homomorphism<conf>(sdd::fixpoint(*f));
        return reinterpret_cast<swiftsdd_obj*>(hom);
    }

    swiftsdd_obj* swiftsdd_hom_function_create(
        swiftsdd_obj* order_ptr,
        uint32_t identifier,
        swiftsdd_user_function user_function)
    {
        auto order = reinterpret_cast<sdd::order<conf>*>(order_ptr);
        auto hom = new sdd::homomorphism<conf>(
            sdd::function<conf>(
                *order, identifier, swiftsdd::homomorphism_function<conf> { user_function }));
        return reinterpret_cast<swiftsdd_obj*>(hom);
    }

    swiftsdd_obj* swiftsdd_hom_rewrite(swiftsdd_obj* order_ptr, swiftsdd_obj* hom_ptr) {
        auto order = reinterpret_cast<sdd::order<conf>*>(order_ptr);
        auto f = reinterpret_cast<sdd::homomorphism<conf>*>(hom_ptr);
        auto hom = new sdd::homomorphism<conf>(sdd::rewrite(*order, *f));
        return reinterpret_cast<swiftsdd_obj*>(hom);
    }

    swiftsdd_obj* swiftsdd_hom_composition(swiftsdd_obj* lhs_ptr, swiftsdd_obj* rhs_ptr) {
        auto lhs = reinterpret_cast<sdd::homomorphism<conf>*>(lhs_ptr);
        auto rhs = reinterpret_cast<sdd::homomorphism<conf>*>(rhs_ptr);

        auto res = new sdd::homomorphism<conf>(sdd::composition(*lhs, *rhs));
        return reinterpret_cast<swiftsdd_obj*>(res);
    }

    swiftsdd_obj* swiftsdd_hom_sum(
        swiftsdd_obj* order_ptr,
        swiftsdd_obj** hom_array_ptr,
        size_t operand_number)
    {
        auto order = *reinterpret_cast<sdd::order<conf>*>(order_ptr);

        std::vector<sdd::homomorphism<conf>> operands;
        operands.reserve(operand_number);
        for (std::size_t i = 0; i < operand_number; ++i) {
            auto hom = *reinterpret_cast<sdd::homomorphism<conf>*>(hom_array_ptr[i]);
            operands.push_back(hom);
        }

        auto res = new sdd::homomorphism<conf>(
            sdd::sum(order, operands.begin(), operands.end()));
        return reinterpret_cast<swiftsdd_obj*>(res);
    }

    swiftsdd_obj* swiftsdd_hom_intersection(
        swiftsdd_obj* order_ptr,
        swiftsdd_obj** hom_array_ptr,
        size_t operand_number)
    {
        auto order = *reinterpret_cast<sdd::order<conf>*>(order_ptr);

        std::vector<sdd::homomorphism<conf>> operands;
        operands.reserve(operand_number);
        for (std::size_t i = 0; i < operand_number; ++i) {
            auto hom = *reinterpret_cast<sdd::homomorphism<conf>*>(hom_array_ptr[i]);
            operands.push_back(hom);
        }

        auto res = new sdd::homomorphism<conf>(
            sdd::intersection(order, operands.begin(), operands.end()));
        return reinterpret_cast<swiftsdd_obj*>(res);
    }

    void swiftsdd_hom_destroy(swiftsdd_obj* hom_ptr) {
        delete reinterpret_cast<sdd::homomorphism<conf>*>(hom_ptr);
    }

    swiftsdd_obj* swiftsdd_hom_apply(
        swiftsdd_obj* order_ptr,
        swiftsdd_obj* hom_ptr,
        swiftsdd_obj* sdd_ptr)
    {
        auto order = *reinterpret_cast<sdd::order<conf>*>(order_ptr);
        auto hom = *reinterpret_cast<sdd::homomorphism<conf>*>(hom_ptr);
        auto sdd = *reinterpret_cast<sdd::SDD<conf>*>(sdd_ptr);

        auto res = new sdd::SDD<conf>(hom(order, sdd));
        return reinterpret_cast<swiftsdd_obj*>(res);
    }

    // MARK: Interface for sdd::tools
    swiftsdd_pair_of_unsigned_int swiftsdd_tools_nodes(swiftsdd_obj* sdd_ptr) {
        auto sdd = reinterpret_cast<sdd::SDD<conf>*>(sdd_ptr);
        auto res = sdd::tools::nodes(*sdd);

        return swiftsdd_pair_of_unsigned_int {
            .first = res.first,
            .second = res.second
        };
    }

}
