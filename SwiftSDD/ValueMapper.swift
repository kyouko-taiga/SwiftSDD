//
//  ValueMapper.swift
//  SwiftSDD
//
//  Created by Dimitri Racordon on 04.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

public protocol ValueMapper {

    associatedtype SourceType

    func project(_ value: SourceType) -> UInt32

    func reverse(_ value: UInt32) -> SourceType?

}


public final class ValueMap<T: Hashable>: ValueMapper {

    public typealias SourceType = T

    private var map = [T: UInt32]()
    private var it: UInt32 = 0

    public func project(_ value: T) -> UInt32 {
        if let projection = self.map[value] {
            return projection
        }

        self.map[value] = self.it
        self.it += 1
        return self.map[value]!
    }

    public func reverse(_ projection: UInt32) -> SourceType? {
        for (value, result) in self.map {
            if projection == result {
                return value
            }
        }

        return nil
    }
    
}
