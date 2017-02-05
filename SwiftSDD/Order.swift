//
//  Order.swift
//  SwiftSDD
//
//  Created by 4rch0dia on 04.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

public final class OrderBuilder {

    internal let pointer: OpaquePointer!

    private init(pointer: OpaquePointer!) {
        self.pointer = pointer
    }

    public convenience init() {
        self.init(pointer: swiftsdd_order_builder_create())
    }

    deinit {
        swiftsdd_order_builder_destroy(self.pointer)
    }

    /// Pushes a flat identifier at the top of this order.
    public func push(identifier: UInt32) {
        swiftsdd_order_builder_push(self.pointer, identifier)
    }

}


public final class Order {

    internal let pointer: OpaquePointer!

    private init(pointer: OpaquePointer!) {
        self.pointer = pointer
    }

    public convenience init(using builder: OrderBuilder) {
        self.init(pointer: swiftsdd_order_create(builder.pointer))
    }

    deinit {
        swiftsdd_order_destroy(self.pointer)
    }
    
}

