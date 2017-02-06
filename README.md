# SwiftSDD
SwiftSDD is a Swift wrapper to [libsdd](https://github.com/ahamez/libsdd), a generic C++ library for Hierarchical Set Decision Diagrams.
It exposes an API very close to that of libsdd, while making use Swift's intuitive programming style.

A SDD is a structure capable of representing large sets of sequences of assignements.
They take advantage of the similarities between those sequences in such a way that they are compact in memory, and can be manipulated efficiently.

## Installation

You can drag and drop `SwiftSDD.xcodeproj` into your project and then link `SwiftSDD.framework` to your target.

Note that SwiftSDD's framework must be linked against [boost C++](http://www.boost.org), as it is a dependency of libsdd.

### Command line tools

Note that you can't use SwiftSDD as a framework if you're building a command line tool.
Until Swift's ABI stabilization, Swift libraries must be embedded in each app and command line tool are not bundles.

You can however compile SwiftSDD directly into your target:

- Make sure to include all SwiftSDD's sources (the `.swift` and `.cc` files) in your target.
- Make sure you compile C++ files in C++14 ("Apple LLVM 8.0 - Language - C++ :: C++ Language Dialect").
- Add `SwiftSDD-Bridging-Header.h` as your Objective-C Bridging Header ("Swift Compiler - General :: Objective-C Bridging Header").
- Add the search paths to your boost installation ("Search Paths :: Header/Library Search Paths").
- Link your target with `boost-system` and `boost-coroutines`.


## Usage

### Creating SDDs

As with libsdd, you first need to create an order on the variables your SDD will hold:

```swift
let builder = OrderBuilder()
for i in 0 ... 2 {
	builder.push(identifier: UInt32(i))
}

let order = Order(using: builder)
```

You can then create SDDs with their `init(with:using)` initializer, which takes an order and a function that assigns a value to each variable of the order:

```swift
let x = SDD(with: order) { identifier, _ in
	return identifier
}
```

You can get a string representation of your SDD with the `dump()` method.
Be sure to use it only on small SDDs, as the output can get really huge:

```swift
print(x.dump())
// Prints "2[{2} --> 1[{1} --> 0[{0} --> |1|]]]"
```

Because functions that are passed to libsdd can't capture variables (they're annotated with `@convention(c)`), we can't access any outside variable when calling the SDD's initializer.
To go around this limitation, you can pass a reference to some custom user data in the SDD's initializer, which will be available as the second parameter of the initializing function.

```swift
var someValue = 42

let y = SDD(with: order, userData: &someValue) { identifier, userData in
	let someValue userData!.assumingMemoryBound(to: Int.self).pointee
	return identifier + UInt32(someValue)
}

print(y.dump())
// Prints "2[{44} --> 1[{43} --> 0[{42} --> |1|]]]"
```

As in libsdd, SDDs support the union and intersection operations:

```swift
let result = lhs.union(order)
```

### Performing operations on SDDs

Homomorphisms allow you to perform operations on SDDs efficiently.
As libsdd, SwiftSDD exposes a collection of predefined homomorphisms, such as the identity or the fixed point, as well as an API to define your owns.

For instance, the *function homomorphism* allows you to change the valuation of a particular variable accross your SDD:

```swift
struct Increment: UserFunction {

	public let is_a_selector = true
	public let is_a_shifter = false

	func call(values: Set<UInt32>) -> Set<UInt32> {
		return Set(values.map({ $0 + UInt32(1) }))
	}

var f: UserFunction = Increment()
let inc = Homomorphism.function(with: order, on: 2, using: &inc)

print(hom.apply(to: y, with: order).dump())
// Prints "2[{45} --> 1[{43} --> 0[{42} --> |1|]]]"
```

Homomorphisms can be combined with the usual operations:

```swift
let incOrId = inc.union(order: order, Homomorphism.identity())
```

### Querying SDDs

SDDs conform to the `Sequence` protocol, so you can iterate on the paths they encode with a `for` loop:

```swift
for path in y {
	print(path.map({ String(describing: $0) }).joined(separator: ", "))
}
// Prints "[44], [43], [42]"
```

## Wrapper's Overhead

To keep the wrapping overhead as low as possible, most the interactions with libsdd are made by manipulating opaque pointers, which are nicely wrapped inside Swift classes.
Nevertheless, some operations can't fit to this model and require the wrapper to allocate memory.
This can get costly when using custom homomorphisms, as the wrapper will have to copy the valuation of the arcs in a good old C array so that Swift can read them.
However, if your homomorphisms benefit well from caching, this overhead can get unnoticeable.
