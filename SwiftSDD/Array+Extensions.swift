//
//  Array+Extensions.swift
//  SwiftSDD
//
//  Created by Dimitri Racordon on 05.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

extension Array {

    internal static func fromCArray(values: UnsafePointer<Element>, size: Int) -> [Element] {
        let buffer = UnsafeBufferPointer(start: values, count: size)
        return Array(buffer)
    }

}
