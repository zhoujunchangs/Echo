#pragma once

#ifndef __PARSER_OBJECT_H_
#define __PARSER_OBJECT_H_

#include "parser_literal.h"

namespace AST {

	class ObjectNameAST : public BaseAST {
	public:
		string name;
		ObjectNameAST(const string& str) : name(str) {
			type = AstObjectName;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			auto id = StringSet::addString(name);
			param.opset.push_back(OpCodeType::OpLoadObject);
			param.pushArg(id);
			return id;
		}
	};

}

#endif // !__PARSER_OBJECT_H_
