#pragma once

#ifndef __TYPE_HPP_
#define __TYPE_HPP_

#include <string>
#include <vector>
#include <map>

#include "util.h"
#include "token.h"

using BasicInt = long long;
using BasicChar = char;
using BasicFloat = double;
using BasicBool = char;

enum MetaIdSet {
	Errortype = 0,
	Uninitialized = 1,

	// Basic type
	Void = 2,
	Int,   // long long
	Float, // double
	Bool,  // bool
	Char,  // char

	// Reference type
	Reference,

	// Compound type
	Array,    // Child1: Type;
		      // Child2: _Array_size; Its size means the count of the array;
	Set,      // Struct: named set;
	          // (Type, ...): Unnamed set;
	Function, // Child1: Parameter type;
			  // Child2: Return type;
	// Internal use
	Field,
	Arraysize,
	ArgstoRets,
};

enum BasicMeta {
	MNone = 0,
	MInt = 1,
	MFloat = 2,
	MChar = 3,
	MBool = 4,
	MReference = 5,
	MFunction = 6,
	MArray = 7,
	MSet = 8,
	MVoid = 9,
};


#endif // !__TYPE_HPP_
