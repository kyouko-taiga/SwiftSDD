//
//  Homomorphism.swift
//  SwiftSDD
//
//  Created by Dimitri Racordon on 05.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

public final class Homomorphism {

    internal let pointer: OpaquePointer!

    private init(pointer: OpaquePointer!) {
        self.pointer = pointer
    }

    deinit {
        swiftsdd_hom_destroy(self.pointer)
    }

    public func apply(to sdd: SDD, with order: Order) -> SDD {
        return SDD(pointer: swiftsdd_hom_apply(order.pointer, self.pointer, sdd.pointer))
    }

    /// Rewrites this homomorphism to enable saturation.
    public func rewrite(with order: Order) -> Homomorphism {
        return Homomorphism(pointer: swiftsdd_hom_rewrite(order.pointer, self.pointer))
    }

    // Returns the composition of this homomorphism with another.
    public func composition(_ other: Homomorphism) -> Homomorphism {
        return Homomorphism(pointer: swiftsdd_hom_composition(self.pointer, other.pointer))
    }

    /// Returns the union of this homomorphism with others.
    public func union(order: Order, _ others: Homomorphism...) -> Homomorphism {
        precondition(others.count > 0)

        var pointers = [self.pointer] + others.map { $0.pointer }
        return Homomorphism(
            pointer: swiftsdd_hom_sum(order.pointer, &pointers, pointers.count))
    }

    /// Returns the intersection of this homomorphism with others.
    public func intersection(order: Order, _ others: Homomorphism...) -> Homomorphism {
        precondition(others.count > 0)

        var pointers = [self.pointer] + others.map { $0.pointer }
        return Homomorphism(
            pointer: swiftsdd_hom_intersection(order.pointer, &pointers, pointers.count))
    }

    /// Returns the identity homomorphism.
    public static func identity() -> Homomorphism {
        return Homomorphism(pointer: swiftsdd_hom_identity_create())
    }

    /// Returns a constant homomorphism.
    public static func constant(_ sdd: SDD) -> Homomorphism {
        return Homomorphism(pointer: swiftsdd_hom_constant_create(sdd.pointer))
    }

    // Returns a fixpoint homomorphism.
    public static func fixpoint(_ f: Homomorphism) -> Homomorphism {
        return Homomorphism(pointer: swiftsdd_hom_fixpoint_create(f.pointer))
    }

    /// Returns an homomorphism that applies the given function on the values
    /// assigned to the specified identifier.
    public static func function(
        with order: Order,
        on identifier: UInt32,
        using userFunction: inout UserFunction) -> Homomorphism
    {
        // Create a swiftsdd_user_function so that we can encapsule the given
        // user function.
        let fnStruct = swiftsdd_user_function(
            fn: userFunctionWrapper,
            user_data: &userFunction,
            is_a_selector: userFunction.is_a_selector,
            is_a_shifter: userFunction.is_a_shifter)

        return Homomorphism(
            pointer: swiftsdd_hom_function_create(order.pointer, identifier, fnStruct))
    }

}


public protocol UserFunction {

    var is_a_selector: Bool { get }
    var is_a_shifter: Bool { get }

    func call(values: Set<UInt32>) -> Set<UInt32>

}


/// Wrapper to a user function.
fileprivate func userFunctionWrapper(
    cArray: swiftsdd_uint32_array,
    user_data: UnsafeMutableRawPointer?) -> swiftsdd_uint32_array
{
    if let receiver = user_data?.assumingMemoryBound(to: UserFunction.self).pointee {
        let values = Set([UInt32].fromCArray(values: cArray.values, size: cArray.size))
        var res = receiver.call(values: values).map { $0 }

        return swiftsdd_uint32_array(values: &res, size: res.count)
    }

    return cArray
}
