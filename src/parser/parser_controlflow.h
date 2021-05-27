#pragma once

#ifndef __PARSER_CONTROLFLOW_H_
#define __PARSER_CONTROLFLOW_H_

#include "parser_base.h"
#include "parser_expr.h"

namespace AST {
	class LoopAST : public BaseAST {
		BasePtr condition;
		BlockPtr body;
	public:
		LoopAST(BasePtr _condition, BlockPtr _body)
			:condition(std::move(_condition)), body(std::move(_body)) {
			type = AstType::AstLoop;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			int tok = param.opset.size();
			condition->Analyse(param);
			
			param.opset.push_back(OpCodeType::OpIf);
			int tok2 = param.opset.size();
			param.pushArg(0);
			
			body->Analyse(param);
			param.opset.push_back(OpCodeType::OpGoto);
			param.pushArg(tok - param.opset.size() - sizeof(OpCodeArg)/sizeof(OpCode));
			
			*((OpCodeArg*)(&param.opset[tok2])) = param.opset.size() - tok2 - sizeof(OpCodeArg) / sizeof(OpCode);
			return 0;
		}
	};

	class ConditionAST : public BaseAST {
		BasePtr condition;
		BlockPtr truebody;
		BlockPtr falsebody;
	public:
		ConditionAST(BasePtr _condition, BlockPtr _truebody, BlockPtr _falsebody)
			:condition(std::move(_condition)), truebody(std::move(_truebody)), falsebody(std::move(_falsebody)) {
			type = AstType::AstCondition;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			condition->Analyse(param);
			
			param.opset.push_back(OpCodeType::OpIf);
			int ifbeg = param.opset.size(), iftrueend, ifend;
			param.pushArg(0);

			truebody->Analyse(param);

			if (falsebody) {
				param.opset.push_back(OpCodeType::OpGoto);
				iftrueend = param.opset.size();
				param.pushArg(0);

				falsebody->Analyse(param);
			}

			ifend = param.opset.size();
			
			if (!falsebody) iftrueend = ifend - sizeof(OpCodeArg) / sizeof(OpCode);

			*((OpCodeArg*)(&param.opset[ifbeg])) = iftrueend - ifbeg;

			if(falsebody) *((OpCodeArg*)(&param.opset[iftrueend])) = ifend - iftrueend - sizeof(OpCodeArg) / sizeof(OpCode);
			return 0;
		}
	};

	BasePtr ParseLoop() {
		auto condition = ParseExpr();
		if (!condition) return nullptr;

		auto body = ParseBlock();
		if (!body) return nullptr;
		
		return make_unique<LoopAST>(std::move(condition), std::move(body));
	}

	BasePtr ParseCondition() {
		auto condition = ParseExpr();
		if (!condition) return nullptr;

		BlockPtr truebody, falsebody;
		
		truebody = ParseBlock();
		if (!truebody) return nullptr;
		
		if (getCurrentToken().type == TokenSet::Else) {
			getNextToken(); // consume |
			falsebody = ParseBlock();
			if (!falsebody) return nullptr;
		}

		return make_unique<ConditionAST>(std::move(condition), std::move(truebody), std::move(falsebody));
	}
}

#endif // !__PARSER_CONTROLFLOW_H_