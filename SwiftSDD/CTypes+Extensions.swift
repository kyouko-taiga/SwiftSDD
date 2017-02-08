//
//  CTypes+Extensions.swift
//  SwiftSDD
//
//  Created by Dimitri Racordon on 08.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

public protocol CArray: Collection {

    associatedtype Element

    var count: Int { get }
    var data: UnsafeMutablePointer<Element>! { get }

    init(data: UnsafeMutablePointer<Element>, count: Int)

    init<C: Collection>(_ elements: C)
        where C.Iterator.Element == Element, C.IndexDistance == Int

}


extension CArray {

    /// Copies the given collection to initialize a new CArray.
    ///
    /// Note that the allocated memory won't be automatically deallocated!
    /// Make sure it's always deinitialized and deallocated.
    public init<C: Collection>(_ elements: C)
        where C.Iterator.Element == Self.Element, C.IndexDistance == Int
    {
        let pointer = UnsafeMutablePointer<Self.Element>.allocate(capacity: elements.count)
        pointer.initialize(from: elements)
        self.init(data: pointer, count: elements.count)
    }

    public var startIndex: Int {
        return 0
    }

    public var endIndex: Int {
        return self.count
    }

    public subscript(index: Int) -> Self.Element {
        precondition((index >= 0) && (index < self.endIndex), "index is out of range")
        return self.data[index]
    }

    public func index(after i: Int) -> Int {
        precondition(i < self.endIndex, "can't advance beyond endIndex")
        return i + 1
    }

}


extension swiftsdd_uint32_set: CArray {

    public typealias Element = UInt32

    public init(data: UnsafeMutablePointer<UInt32>, count: Int) {
        self.data = data
        self.count = count
    }

}
