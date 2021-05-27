#pragma once

#ifndef __INTERPRETER_H_
#define __INTERPRETER_H_

#include "token.h"
#include "parser/parser.h"
#include "analyzer.h"
#include "executor.h"
#include "result.h"
#include "test/testutil.h"

string ExecuteCode(string code) {

	string compressedcode = "{" + std::move(code) + "}";
	
	Result::clear();

	lexToken(compressedcode);
	if (!Result::empty()) {
		while (!tokenbuf.empty()) tokenbuf.pop();
		return "Syntax Lexer Error: \n" + Result::getResult();
	}

	while (!(Analyzer::instance().opset.empty()) && *(Analyzer::instance().opset.rbegin()) == OpCodeType::OpEnd) Analyzer::instance().opset.pop_back();
	
	auto size = Analyzer::instance().opset.size();
	
	auto expr = AST::ParseBlock();
	if (expr == nullptr) {
		return "Syntax Parser Error: \n" + Result::getResult();
	}

	if (expr->Analyse(Analyzer::instance()) == -1) {
		Analyzer::instance().opset.resize(size);
		return "Syntax Analyzer Error: \n" + Result::getResult();
	}

	Analyzer::instance().opset.push_back(OpCodeType::OpEnd);

	Executor::Execute(Analyzer::instance(), &(Analyzer::instance().opset[size]));

	return Result::getResult();
}

#endif // !__INTERPRETER_H_
