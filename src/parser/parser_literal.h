#pragma once

#ifndef __PARSER_LITERAL_H_
#define __PARSER_LITERAL_H_

#include "parser_base.h"

namespace AST {

// Literal AST Node

class IntLiteralAST : public BaseAST {
	BasicInt val;
public:
	IntLiteralAST(const BasicInt& _v) : val(_v) {
		type = AstInt;
	}
	int Analyse(Analyzer::AnalyzerParam& param) override {
		StringId id = param.consts.size();
		Object result = ObjectManager::createObjectInfowithDeleter(id, param.innermeta[BasicMeta::MInt]);
		result->address = new char[result->meta->offset];
		EXTRACTADDRESS(result, BasicInt) = val;
		
		param.consts.push_back(std::move(result));

		param.opset.push_back(OpCodeType::OpLoadConsts);
		param.pushArg(id);
		return 0;
	}
};


class FloatLiteralAST : public BaseAST {
	BasicFloat val;
public:
	FloatLiteralAST(const BasicFloat& _v) : val(_v) {
		type = AstFloat;
	}
	int Analyse(Analyzer::AnalyzerParam& param) override {
		StringId id = param.consts.size();
		Object result = ObjectManager::createObjectInfowithDeleter(id, param.innermeta[BasicMeta::MFloat]);
		result->address = new char[result->meta->offset];
		EXTRACTADDRESS(result, BasicFloat) = val;

		param.consts.push_back(std::move(result));

		param.opset.push_back(OpCodeType::OpLoadConsts);
		param.pushArg(id);
		return 0;
	}
};


class CharLiteralAST : public BaseAST {
	BasicChar val;
public:
	CharLiteralAST(const BasicChar& _v) : val(_v) {
		type = AstChar;
	}
	int Analyse(Analyzer::AnalyzerParam& param) override {
		StringId id = param.consts.size();
		Object result = ObjectManager::createObjectInfowithDeleter(id, param.innermeta[BasicMeta::MChar]);
		result->address = new char[result->meta->offset];
		EXTRACTADDRESS(result, BasicChar) = val;
		
		param.consts.push_back(std::move(result));

		param.opset.push_back(OpCodeType::OpLoadConsts);
		param.pushArg(id);
		return 0;
	}
};


class BoolLiteralAST : public BaseAST {
	BasicBool val;
public:
	BoolLiteralAST(const BasicBool& _v) : val(_v) {
		type = AstBool;
	}
	int Analyse(Analyzer::AnalyzerParam& param) override {
		StringId id = param.consts.size();
		Object result = ObjectManager::createObjectInfowithDeleter(id, param.innermeta[BasicMeta::MBool]);
		result->address = new char[result->meta->offset];
		EXTRACTADDRESS(result, BasicBool) = (val) ? (1) : (0);

		param.consts.push_back(std::move(result));

		param.opset.push_back(OpCodeType::OpLoadConsts);
		param.pushArg(id);
		return 0;
	}
};

// ! Literal AST Node

// Parse Literal

static BasePtr ParseFloatLiteral() {
	auto result = make_unique<FloatLiteralAST>(stod(getCurrentToken().data));
	getNextToken();
	return std::move(result);
}

static BasePtr ParseIntLiteral() {
	auto result = make_unique<IntLiteralAST>(stoll(getCurrentToken().data));
	getNextToken();
	return std::move(result);
}

static BasePtr ParseCharLiteral() {
	auto result = make_unique<CharLiteralAST>(getCurrentToken().data[0]);
	getNextToken();
	return std::move(result);
}

static BasePtr ParseBoolLiteral() {
	if (getCurrentToken().data == "true") {
		getNextToken();
		return make_unique<BoolLiteralAST>(1);
	}
	else {
		getNextToken();
		return make_unique<BoolLiteralAST>(0);
	}
}

static BasePtr ParseLiteral() {
	switch (getCurrentToken().type) {
	case TokenSet::LInt:
		return ParseIntLiteral();
	case TokenSet::LBool:
		return ParseBoolLiteral();
	case TokenSet::LChar:
		return ParseCharLiteral();
	case TokenSet::LFloat:
		return ParseFloatLiteral();
	default:
		return nullptr;
	}
}

// ! Parse Literal

}

#endif // !__PARSER_LITERAL_H_
