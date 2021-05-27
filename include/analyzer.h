#pragma once

#ifndef __ANALYZER_H_
#define __ANALYZER_H_

#include "util.h"
#include "object.h"
#include <sstream>

namespace Analyzer {

	struct AnalyzerParam {
		vector<OpCode> opset;
		std::vector<Object> consts;
		std::vector<MetaObject> innermeta;

		void pushArg(OpCodeArg arg) {
			OpCode* trans = (OpCode*)(&arg);
			for (int i = 0; i < sizeof(OpCodeArg) / sizeof(OpCode); ++i) {
				opset.push_back(trans[i]);
			}
		}
	};

	AnalyzerParam __param;

	AnalyzerParam& instance() {
		return __param;
	}

	static inline int init() {
		return 0;
	}

}
#endif // !__ANALYZER_H_
