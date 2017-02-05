//
//  SDD.swift
//  SwiftSDD
//
//  Created by Dimitri Racordon on 04.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

public final class SDD {

    internal let pointer: OpaquePointer!

    private init(pointer: OpaquePointer) {
        self.pointer = pointer
    }

    public convenience init() {
        self.init(pointer: swiftsdd_sdd_create())
    }

    public convenience init(
        with: Order, userData: UnsafeRawPointer? = nil,
        initializer: @escaping @convention(c) (UInt32, UnsafeRawPointer?) -> UInt32)
    {
        self.init(pointer: swiftsdd_sdd_create_with_order(order.pointer, &x, initializer))
    }

    deinit {
        swiftsdd_sdd_destroy(self.pointer)
    }

    // MARK: Properties of the decision diagram

    /// The number of combinations stored in this SDD.
    public var count: UInt64 {
        return swiftsdd_sdd_size(self.pointer)
    }

    /// Returns the number of unique nodes in this SDD.
    public var nodes: (flat: UInt32, hierarchical: UInt32) {
        let res = swiftsdd_tools_nodes(self.pointer)
        return (flat: res.first, hierarchical: res.second)
    }

    // MARK: Set operations

    public func formUnion(_ other: SDD) -> SDD {
        return SDD(pointer: swiftsdd_sdd_union(self.pointer, other.pointer))
    }

    public func formIntersection(_ other: SDD) -> SDD {
        return SDD(pointer: swiftsdd_sdd_intersection(self.pointer, other.pointer))
    }

    // MARK: Terminal symbols

    /// Returns the |0| terminal.
    public static func zero() -> SDD {
        return SDD(pointer: swiftsdd_sdd_zero())
    }

    /// Returns the |1| terminal.
    public static func one() -> SDD {
        return SDD(pointer: swiftsdd_sdd_one())
    }

    // MARK: Debug helpers

    /// Returns the textual representation of this SDD.
    ///
    /// Use only with small SDD, output can be huge.
    public func dump() -> String {
        if let str = swiftsdd_sdd_str_create(self.pointer) {
            let res = String(cString: str)
            swiftsdd_sdd_str_destroy(str)
            return res
        }

        return ""
    }

}


extension SDD: Hashable {

    public var hashValue: Int {
        return swiftsdd_sdd_hash(self.pointer)
    }

    public static func == (left: SDD, right: SDD) -> Bool {
        return swiftsdd_sdd_eq(left.pointer, right.pointer)
    }

}


extension SDD: Sequence {

    public typealias Iterator = SDDIterator

    public func makeIterator() -> SDDIterator {
        return SDDIterator(pointer: swiftsdd_path_generator_create(self.pointer))
    }

}


public final class SDDIterator: IteratorProtocol {

    fileprivate let pointer: OpaquePointer!

    fileprivate init(pointer: OpaquePointer) {
        self.pointer = pointer
    }

    deinit {
        swiftsdd_path_generator_destroy(self.pointer)
    }

    public func next() -> Path? {
        var didEnd = false
        let pathPointer = swiftsdd_path_generator_get(self.pointer, &didEnd)

        guard !didEnd else {
            return nil
        }

        return Path(pointer: pathPointer!)
    }
    
}
