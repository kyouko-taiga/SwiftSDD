//
//  main.swift
//  SwiftSDD
//
//  Created by Dimitri Racordon on 03.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

import SwiftSDD


let lib = swiftsdd_init()


let builder = OrderBuilder()
for i in 0 ... 3 {
    builder.push(identifier: UInt32(i))
}

let order = Order(using: builder)

var x = 42

let lhs = SDD(with: order, userData: &x) {
    _, scope in

    if let x = scope?.assumingMemoryBound(to: Int.self).pointee {
        return UInt32(x)
    }
    return 0
}

let rhs = SDD(with: order, userData: &x) {
    identifier, userData in

    if let x = userData?.assumingMemoryBound(to: Int.self).pointee {
        return UInt32(x) + identifier
    }
    return 0
}

struct Increment: UserFunction {

    public let is_a_selector = true
    public let is_a_shifter = false

    func call(values: Set<UInt32>) -> Set<UInt32> {
        return Set(values.map({ $0 + UInt32(1) }))
    }

}

var inc: UserFunction = Increment()

var hom = Homomorphism.function(with: order, on: 2, using: &inc)
hom = hom.union(order: order, Homomorphism.identity())

let res = hom.apply(to: lhs, with: order).union(rhs)

for path in res {
    print(path.map({ String(describing: $0) }).joined(separator: ", "))
}

print("\(res.count) object(s) encoded.")
print("\(res.nodes) node(s).")
print(res.dump())

swiftsdd_deinit(lib)
