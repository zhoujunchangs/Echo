#pragma once

#ifndef __PARSER_INOUT_H_
#define __PARSER_INOUT_H_

#include "parser_base.h"
#include "parser_expr.h"

namespace AST {

	class InAST :public BaseAST {
		std::vector<uniptr<ObjectNameAST>> par;
	public:
		InAST(std::vector<uniptr<ObjectNameAST>> _param) : par(std::move(_param)) {
			type = AstType::AstIn;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			for (int i = 0; i < par.size(); ++i) {
				param.opset.push_back(OpCodeType::OpLoadObject);
				param.pushArg(StringSet::addString("@arg" + std::to_string(i)));
				param.opset.push_back(OpCodeType::OpLoadObjectMeta);
				param.opset.push_back(OpCodeType::OpAddObject);
				param.pushArg(StringSet::addString(par[i]->name));
				param.opset.push_back(OpCodeType::OpLoadObject);
				param.pushArg(StringSet::addString("@arg" + std::to_string(i)));
				param.opset.push_back(OpCodeType::OpAssign);
				param.opset.push_back(OpCodeType::OpPop);
			}
			return 0;
		}
	};

	class PrintAST :public BaseAST {
		uniptr<BaseAST> par;
	public:
		PrintAST(uniptr<BaseAST> _param) : par(std::move(_param)) {
			type = AstType::AstPrint;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			if (par) {
				par->Analyse(param);
				if (!isNoPopAst(par->getType())) {
					param.opset.push_back(OpCodeType::OpPrint);
				}
			}
			return 0;
		}
	};

	class OutAST :public BaseAST {
		BasePtr par;
	public:
		OutAST(BasePtr _param) : par(std::move(_param)) {
			type = AstType::AstOut;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			param.opset.push_back(OpCodeType::OpLoadObject);
			param.pushArg(StringSet::addString("@ret"));
			par->Analyse(param);
			param.opset.push_back(OpCodeType::OpAssign);
			param.opset.push_back(OpCodeType::OpPop);
			return 0;
		}
	};

	BasePtr ParseIn() {
		std::vector<uniptr<ObjectNameAST>> par;
		while (getCurrentToken().type == TokenSet::Name) {
			par.emplace_back(make_unique<ObjectNameAST>(getNextToken().data));
			if (getCurrentToken().type != TokenSet::Comma) break;
			getNextToken(); // cosume ,
		}
		return make_unique<InAST>(std::move(par));
	}

	BasePtr ParseOut() {
		auto par = ParseExpr();
		return make_unique<OutAST>(std::move(par));
	}
}

#endif // !__PARSER_INOUT_H_
