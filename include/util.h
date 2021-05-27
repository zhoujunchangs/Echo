#pragma once

#ifndef __UTIL_H_
#define __UTIL_H_

#include <string>
#include <cstring>

using std::string;
using StringId = int;

#include <memory>

template<typename T>
using uniptr = std::unique_ptr<T>;
using std::make_unique;

#include <vector>

using std::vector;

#define IN_RANGE(var, lr, rr) (var >= lr && var <= rr)

#include <iostream>
#include <sstream>

#endif // !__UTIL_H_
