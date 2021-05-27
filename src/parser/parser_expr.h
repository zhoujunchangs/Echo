#pragma once

#ifndef __PARSER_EXPR_H_
#define __PARSER_EXPR_H_

#include "parser_base.h"
#include "parser_object.h"

namespace AST {

	class OperatorAST : public BaseAST {
		BasePtr LHS, RHS;
		Token op;
	public:
		OperatorAST(const Token& _op, BasePtr l, BasePtr r);
		int Analyse(Analyzer::AnalyzerParam& param) override;
	};


	class _NullOperatorLHSAST : public BaseAST {
	public:
		_NullOperatorLHSAST();
		int Analyse(Analyzer::AnalyzerParam& param) override;
	};


	class _NullOperatorRHSAST : public BaseAST {
	public:
		_NullOperatorRHSAST();
		int Analyse(Analyzer::AnalyzerParam& param) override;
	};

	static BasePtr ParseExpr();

	static BasePtr ParseParenthesisExpr();

	static BasePtr ParseIdentifierExpr();

	static BasePtr ParsePrimary();

	static BasePtr ParseOpRHS(int exprprior, BasePtr LHS);

	static BasePtr ParseExpr();
}


namespace AST {

	OperatorAST::OperatorAST(const Token& _op, BasePtr l, BasePtr r)
		: op(_op), LHS(std::move(l)), RHS(std::move(r)) {
		type = AstOperator;
	}

	int OperatorAST::Analyse(Analyzer::AnalyzerParam& param) {
		if (op.type == TokenSet::And) {
			LHS->Analyse(param);
			param.opset.push_back(OpCodeType::OpGotoIfFalse);
			auto tag0 = param.opset.size();
			param.pushArg(0);
			RHS->Analyse(param);
			param.opset.push_back(OpCodeType::OpLogic_And);
			*((OpCodeArg*)(&param.opset[tag0])) = param.opset.size() - tag0 - sizeof(OpCodeArg) / sizeof(OpCode);
			return 0;
		}
		else if (op.type == TokenSet::Or) {
			LHS->Analyse(param);
			param.opset.push_back(OpCodeType::OpGotoIfTrue);
			auto tag0 = param.opset.size();
			param.pushArg(0);
			RHS->Analyse(param);
			param.opset.push_back(OpCodeType::OpLogic_Or);
			*((OpCodeArg*)(&param.opset[tag0])) = param.opset.size() - tag0 - sizeof(OpCodeArg) / sizeof(OpCode);
			return 0;
		}
		else
		if (op.type == TokenSet::Dot) {
			if (RHS->getType() != AstType::AstObjectName) {
				Result::output() << "cannot apply dot.\n";
				return -1;
			}
			if (LHS->Analyse(param) == -1) return -1;
			param.opset.push_back(OpCodeType::OpExtractDot);
			StringId id = StringSet::addString(((ObjectNameAST*)(RHS.get()))->name);
			param.pushArg(id);
			return 0;
		}

		if (op.type == TokenSet::Left_parenthesis) {
			param.opset.push_back(OpCodeType::OpPushCurrent);
			RHS->Analyse(param);
			LHS->Analyse(param);
			param.opset.push_back(OpCodeType::OpVisit);
			return 0;
		}

		OpCodeArg l, r;
		if (LHS) l = LHS->Analyse(param);
		if (RHS) r = RHS->Analyse(param);
		
		if (l == -1 || r == -1) return -1;

		switch (op.type) {
		case TokenSet::Dot:
			param.opset.push_back(OpCodeType::OpExtractDot);
			break;
		case TokenSet::Plus:
			param.opset.push_back(OpCodeType::OpPlus);
			break;
		case TokenSet::Minus:
			param.opset.push_back(OpCodeType::OpMinus);
			break;
		case TokenSet::Multiply:
			param.opset.push_back(OpCodeType::OpMultiply);
			break;
		case TokenSet::Devide:
			param.opset.push_back(OpCodeType::OpDevide);
			break;
		case TokenSet::Refer:
			param.opset.push_back(OpCodeType::OpRefer);
			break;
		case TokenSet::Assign:
			param.opset.push_back(OpCodeType::OpAssign);
			break;
		case TokenSet::Equal:
			param.opset.push_back(OpCodeType::OpLogic_Equal);
			break;
		case TokenSet::Notequal:
			param.opset.push_back(OpCodeType::OpLogic_NotEqual);
			break;
		case TokenSet::Greater:
			param.opset.push_back(OpCodeType::OpLogic_Greater);
			break;
		case TokenSet::Less:
			param.opset.push_back(OpCodeType::OpLogic_Less);
			break;
		case TokenSet::Greaterequal:
			param.opset.push_back(OpCodeType::OpLogic_GreaterEqual);
			break;
		case TokenSet::Lessequal:
			param.opset.push_back(OpCodeType::OpLogic_LessEqual);
			break;
		case TokenSet::Not:
			param.opset.push_back(OpCodeType::OpLogic_Not);
			break;
		default:
			return -1;
		}
		return 0;
	}

	_NullOperatorLHSAST::_NullOperatorLHSAST() {
		type = _AstNullOperatorLHS;
	}

	int _NullOperatorLHSAST::Analyse(Analyzer::AnalyzerParam& param) { return 0; }

	_NullOperatorRHSAST::_NullOperatorRHSAST() {
		type = _AstNullOperatorRHS;
	}

	int _NullOperatorRHSAST::Analyse(Analyzer::AnalyzerParam& param) { return 0; }

	static BasePtr ParseParenthesisExpr() {
		getNextToken(); // consume '('
		auto result = ParseExpr();
		if (!result) return nullptr;

		if (getCurrentToken().type != TokenSet::Right_parenthesis) {
			Result::output() << "Expected a ')'";
		}
		else getNextToken(); // consume ')'

		return result;
	}

	static BasePtr ParseIdentifierExpr() {
		EXPECT_TOKEN_RET(curtoken, TokenSet::Name);
		getNextToken();
		return make_unique<ObjectNameAST>(curtoken.data);
	}

	static BasePtr ParseFuncArgs() {
		getNextToken(); // cunsume '('
		BaseBlock args;
		if (getCurrentToken().type != TokenSet::Right_parenthesis) {
			while (true) {
				BasePtr arg;
				if (arg = ParseExpr()) {
					args.push_back(std::move(arg));
				}
				else return nullptr;
				if (getCurrentToken().type == TokenSet::Right_parenthesis) break;
				if (getCurrentToken().type != TokenSet::Comma) {
					Result::output() << "expected a ',' or ')'";
				}
				else getNextToken(); // consume ','
			}
		}
		getNextToken(); // consume ')'
		return make_unique<UnPopedBlockAST>(std::move(args));
	}

	static BasePtr ParseArray() {
		getNextToken(); // consume '['
		auto expr = ParseExpr();
		EXPECT_TOKEN_RET(cur, TokenSet::Right_bracket);
		getNextToken();
		return std::move(expr);
	}

	static BasePtr ParseSingleOpRHS() {
		auto op = getCurrentToken(); // consume '(' or '['
		switch (op.type) {
		case TokenSet::Left_parenthesis:
			return ParseFuncArgs();
		case TokenSet::Left_bracket:
			return ParseArray();
		default:
			Result::output() << "wrong syntax.";
			return nullptr;
		}
	}

	static BasePtr ParsePrimary() {
		auto type = getCurrentToken().type;
		if (TOKEN_IN_RANGE(type, Literal)) {
			return ParseLiteral();
		}
		if (type == TokenSet::Name) {
			return ParseIdentifierExpr();
		}
		if ((TOKEN_IN_RANGE(lasttoken.type, Operator) || lasttoken.type == TokenSet::Left_parenthesis)
			&& type == TokenSet::Left_parenthesis) {
			return ParseParenthesisExpr();
		}
		if (isSingleOperatorLHS(type)) {
			return make_unique<_NullOperatorLHSAST>();
		}
		if (isSingleOperatorRHS(type)) {
			return ParseSingleOpRHS();
		}
		Result::output() << "unknown token found in an expression.";
		return nullptr;
	}

	static BasePtr ParseOpRHS(int exprprior, BasePtr LHS) {
		while (true) {
			Token curoptoken = getCurrentToken();
			int curprior = getTokenPrior(curoptoken.type);

			if (curprior < exprprior) return LHS;

			if (!isSingleOperatorRHS(curoptoken.type)) {
				getNextToken(); // consume current operator ( not single operator RHS )
			}

			auto RHS = ParsePrimary();
			if (!RHS) return nullptr;

			Token nextoptoken = getCurrentToken();
			int nextprior = getTokenPrior(nextoptoken.type);

			if ((nextprior > curprior || isSingleOperatorLHS(nextoptoken.type)) && !isSingleOperatorRHS(curoptoken.type)) {
				RHS = ParseOpRHS(curprior + 1, std::move(RHS));
				if (!RHS) return nullptr;
			}
			LHS = make_unique<OperatorAST>(curoptoken, std::move(LHS), std::move(RHS));
		}
	}

	static BasePtr ParseExpr() {
		auto LHS = ParsePrimary();
		if (!LHS) {
			return nullptr;
		}
		return ParseOpRHS(0, std::move(LHS));
	}
}

#endif // !__PARSER_EXPR_H_