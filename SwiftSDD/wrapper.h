//
//  wrapper.h
//  SwiftSDD
//
//  Created by Dimitri Racordon on 03.02.17.
//  Copyright © 2017 University of Geneva. All rights reserved.
//

#ifndef wrapper_h
#define wrapper_h

#include <stdint.h>


/// An opaque structure to wrap pointers to c++ objects.
typedef struct swiftsdd_obj swiftsdd_obj;


// A struct representing a sized array of uint32.
typedef struct {
    uint32_t* values;
    size_t size;
} swiftsdd_uint32_array;


/// A pair of unsigned int.
typedef struct {
    unsigned int first;
    unsigned int second;
} swiftsdd_pair_of_unsigned_int;

#endif /* wrapper_h */
