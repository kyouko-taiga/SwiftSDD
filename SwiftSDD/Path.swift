//
//  Path.swift
//  SwiftSDD
//
//  Created by Dimitri Racordon on 04.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

public final class Path {

    internal let pointer: OpaquePointer!

    internal init(pointer: OpaquePointer) {
        self.pointer = pointer
    }

    deinit {
        swiftsdd_path_destroy(self.pointer)
    }

    public var count: Int {
        return swiftsdd_path_size(self.pointer)
    }

}


public class PathIterator: IteratorProtocol {

    fileprivate let path: Path
    private var position = 0
    private var end: Int

    fileprivate init(path: Path) {
        self.path = path
        self.end = path.count
    }

    public func next() -> Set<UInt32>? {
        guard self.position < self.end else {
            return nil
        }

        defer { self.position += 1 }

        var values = swiftsdd_path_at(self.path.pointer, position)
        let result = Set(values)
        swiftsdd_uint32_set_clear(&values)

        return result
    }

}


extension Path: Sequence {

    public typealias Iterator = PathIterator

    public func makeIterator() -> Iterator {
        return PathIterator(path: self)
    }

}


extension Path: Collection {

    public typealias Index = Int

    public var startIndex: Index {
        return 0
    }

    public var endIndex: Index {
        return self.count
    }

    public subscript(position: Index) -> Iterator.Element {
        var values = swiftsdd_path_at(self.pointer, position)
        let result = Set(values)
        swiftsdd_uint32_set_clear(&values)

        return result
    }

    public func index(after i: Index) -> Index {
        return i + 1
    }

}
