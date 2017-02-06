//
//  main.swift
//  SwiftSDD
//
//  Created by Dimitri Racordon on 03.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

import Foundation


// Number of philosophers.
var n = 5


// Homomorphisms.
struct Filter: UserFunction {

    public let is_a_selector = true
    public let is_a_shifter = false

    func call(values: Set<UInt32>) -> Set<UInt32> {
        return Set(values.flatMap { return $0 > 0 ? $0 : nil })
    }

}

struct Consume: UserFunction {

    public let is_a_selector = true
    public let is_a_shifter = false

    func call(values: Set<UInt32>) -> Set<UInt32> {
        return Set(values.map { $0 - UInt32(1) })
    }

}

struct Produce: UserFunction {

    public let is_a_selector = true
    public let is_a_shifter = false

    func call(values: Set<UInt32>) -> Set<UInt32> {
        return Set(values.map { $0 + UInt32(1) })
    }

}


var filter: UserFunction = Filter()
var consume: UserFunction = Consume()
var produce: UserFunction = Produce()


// Initialize the library.
let lib = swiftsdd_init()

// Create a variable ordering.
let builder = OrderBuilder()
for i in 0 ..< 3 * n {
    builder.push(identifier: UInt32(i))
}
let order = Order(using: builder)

// Create the transition homomorphisms.
var homomorphisms = [Homomorphism]()

for i in 0 ..< n {
    let think     = UInt32(i)
    let eat       = UInt32((1 * n) + i)
    let leftFork  = UInt32((2 * n) + i)
    let rightFork = UInt32((2 * n) + ((i + 1) % n))

    // Encode transition 'think -> eat'.

    // Precondition
    let teCheck = Homomorphism.identity().intersection(order: order,
        Homomorphism.function(with: order, on: leftFork, using: &filter),
        Homomorphism.function(with: order, on: rightFork, using: &filter),
        Homomorphism.function(with: order, on: think, using: &filter))

    let tePre =      Homomorphism.function(with: order, on: leftFork, using: &consume)
        .composition(Homomorphism.function(with: order, on: rightFork, using: &consume))
        .composition(Homomorphism.function(with: order, on: think, using: &consume))
        .composition(teCheck)

    // Postcondition
    let tePost = Homomorphism.function(with: order, on: eat, using: &produce)

    homomorphisms.append(tePost.composition(tePre))

    // Encode transition 'eat -> think'.

    // Precondition.
    let etCheck = Homomorphism.function(with: order, on: eat, using: &filter)

    let etPre = Homomorphism.function(with: order, on: eat, using: &consume)
        .composition(etCheck)

    // Postcondition.
    let etPost =     Homomorphism.function(with: order, on: leftFork, using: &produce)
        .composition(Homomorphism.function(with: order, on: rightFork, using: &produce))
        .composition(Homomorphism.function(with: order, on: think, using: &produce))

    homomorphisms.append(etPost.composition(etPre))
}

var transitions = Homomorphism.identity()
for hom in homomorphisms {
    transitions = transitions.union(order: order, hom)
}
transitions = Homomorphism.fixpoint(transitions)
transitions = transitions.rewrite(with: order)

// Create the initial state.
let m0 = SDD(with: order, userData: &n) {
    identifier, userData in

    let n = UInt32(userData!.assumingMemoryBound(to: Int.self).pointee)

    switch identifier {
    case _ where identifier < n:
        return 1
    case _ where identifier < 2 * n:
        return 0
    default:
        return 1
    }
}

// Compute the state space.
let start = DispatchTime.now()
let ss = transitions.apply(to: m0, with: order)
let elapsed =
    Double(DispatchTime.now().uptimeNanoseconds - start.uptimeNanoseconds) / 1_000_000_000

// Print results.
print("\(ss.count) states (\(ss.nodes.flat) nodes) in \(elapsed) seconds")

swiftsdd_deinit(lib)
