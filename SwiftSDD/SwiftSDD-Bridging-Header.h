
#include <stdint.h>

#include "wrapper.h"


// MARK: Interface for user types utilies.

void          swiftsdd_uint32_set_clear(swiftsdd_uint32_set* set);


// MARK: Interface for sdd::init.

swiftsdd_obj* swiftsdd_init();
void          swiftsdd_deinit(swiftsdd_obj* lib_ptr);


// MARK: Interface for sdd::order_builder.

swiftsdd_obj* swiftsdd_order_builder_create();
void          swiftsdd_order_builder_destroy(swiftsdd_obj* builder_ptr);

void          swiftsdd_order_builder_push(swiftsdd_obj* builder_ptr, uint32_t identifier);


// MARK: Interface for sdd::order.

swiftsdd_obj* swiftsdd_order_create(swiftsdd_obj* builder_ptr);
void          swiftsdd_order_destroy(swiftsdd_obj* order_ptr);


// MARK: Interface for sdd:SDD.

swiftsdd_obj* swiftsdd_sdd_create();
swiftsdd_obj* swiftsdd_sdd_create_with_order(
    swiftsdd_obj* order_ptr,
    const void* user_data,
    uint32_t (*initializer)(uint32_t, const void*));

void          swiftsdd_sdd_destroy(swiftsdd_obj* sdd_ptr);

swiftsdd_obj* swiftsdd_sdd_zero();
swiftsdd_obj* swiftsdd_sdd_one();

bool          swiftsdd_sdd_eq(swiftsdd_obj* lhs_ptr, swiftsdd_obj* rhs_ptr);

size_t        swiftsdd_sdd_hash(swiftsdd_obj* sdd_ptr);

swiftsdd_obj* swiftsdd_sdd_union(swiftsdd_obj* lhs_ptr, swiftsdd_obj* rhs_ptr);
swiftsdd_obj* swiftsdd_sdd_intersection(swiftsdd_obj* lhs_ptr, swiftsdd_obj* rhs_ptr);

uint64_t      swiftsdd_sdd_size(swiftsdd_obj* sdd_ptr);

const char*   swiftsdd_sdd_str_create(swiftsdd_obj* sdd_ptr);
void          swiftsdd_sdd_str_destroy(const char* str_ptr);


// MARK: Interface for SDD paths

swiftsdd_obj* swiftsdd_path_generator_create(swiftsdd_obj* sdd_ptr);
void          swiftsdd_path_generator_destroy(swiftsdd_obj* generator_ptr);

swiftsdd_obj* swiftsdd_path_generator_get(swiftsdd_obj* generator_ptr, bool* did_end);

void          swiftsdd_path_destroy(swiftsdd_obj* path_ptr);

size_t        swiftsdd_path_size(swiftsdd_obj* path_ptr);
swiftsdd_uint32_set swiftsdd_path_at(swiftsdd_obj* path_ptr, size_t index);

void          swiftsdd_uint32_array_destroy(swiftsdd_uint32_set array);


// MARK: Interface for sdd::homomorphism

swiftsdd_obj* swiftsdd_hom_identity_create();
swiftsdd_obj* swiftsdd_hom_constant_create(swiftsdd_obj* sdd_ptr);

swiftsdd_obj* swiftsdd_hom_cons_create(
    swiftsdd_obj* order_ptr,
    swiftsdd_uint32_set valuation,
    swiftsdd_obj* hom_ptr);

swiftsdd_obj* swiftsdd_hom_fixpoint_create(swiftsdd_obj* hom_ptr);

swiftsdd_obj* swiftsdd_hom_function_create(
    swiftsdd_obj* order_ptr,
    uint32_t identifier,
    swiftsdd_user_function user_function);

swiftsdd_obj* swiftsdd_hom_rewrite(swiftsdd_obj* order_ptr, swiftsdd_obj* hom_ptr);

swiftsdd_obj* swiftsdd_hom_composition(swiftsdd_obj* lhs_ptr, swiftsdd_obj* rhs_ptr);

swiftsdd_obj* swiftsdd_hom_sum(
    swiftsdd_obj* order_ptr,
    swiftsdd_obj** hom_array_ptr,
    size_t operand_number);

swiftsdd_obj* swiftsdd_hom_intersection(
    swiftsdd_obj* order_ptr,
    swiftsdd_obj** hom_array_ptr,
    size_t operand_number);

void swiftsdd_hom_destroy(swiftsdd_obj* hom_ptr);

swiftsdd_obj* swiftsdd_hom_apply(
    swiftsdd_obj* order_ptr,
    swiftsdd_obj* hom_ptr,
    swiftsdd_obj* sdd_ptr);


// MARK: Interface for sdd::tools

swiftsdd_pair_of_unsigned_int swiftsdd_tools_nodes(swiftsdd_obj* sdd_ptr);
