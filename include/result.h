#pragma once

#ifndef __RESULT_H_
#define __RESULT_H_

#include "util.h"

#include <sstream>

namespace Result {

	std::stringstream sout;

	void clear() {
		sout.str("");
	}

	bool empty() {
		return sout.rdbuf()->in_avail() == 0;
	}

	string getResult() {
		return sout.str();
	}
	
	std::stringstream& output() {
		return sout;
	}

}

#endif // !__RESULT_H_
