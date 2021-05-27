#pragma once

#ifndef __PARSER_H_
#define __PARSER_H_

#include "parser_base.h"
#include "parser_literal.h"
#include "parser_object.h"
#include "parser_type.h"
#include "parser_expr.h"
#include "parser_controlflow.h"
#include "parser_definition.h"
#include "parser_inout.h"

namespace AST {

	static BasePtr ParseOnce() {
		Token curtoken = getCurrentToken();
		BasePtr ret;
		switch (curtoken.type) {
		case TokenSet::In:
			getNextToken(); // consume >>
			ret = ParseIn();
			break;
		case TokenSet::Out:
			getNextToken(); // consume <<
			ret = ParseOut();
			break;
		case TokenSet::Definition:
			getNextToken(); // consume >>>
			ret = ParseDef();
			break;
		case TokenSet::Colon:
			getNextToken(); // consume :
			ret = ParseLoop();
			break;
		case TokenSet::If:
			getNextToken(); // consume ?
			ret = ParseCondition();
			break;
		case TokenSet::Left_brace:
			ret = ParseBlock();
			break;
		case TokenSet::Semicolon:
			while (getCurrentToken().type == TokenSet::Semicolon) getNextToken();
			break;
		case TokenSet::Not:
			getNextToken();
			ret = ParseOnce();
			ret = std::make_unique<PrintAST>(std::move(ret));
			break;
		default:
			ret = ParseExpr();
			break;
		}
		while (getCurrentToken().type == TokenSet::Semicolon) getNextToken();
		return std::move(ret);
	}

	static BlockPtr ParseBlock() {
		BaseBlock block;
		if (getCurrentToken().type != TokenSet::Left_brace) {
			auto expr = ParseOnce();
			if (expr) block.push_back(std::move(expr));
			return make_unique<BlockAST>(std::move(block));
		}
		else {
			getNextToken(); // consume {
			while (getCurrentToken().type != TokenSet::Right_brace) {
				auto sentence = ParseOnce();
				if(sentence) block.push_back(std::move(sentence));
			}
			getNextToken(); // consume }
		}
		return make_unique<BlockAST>(std::move(block));
	}

}

#endif // !__PARSER_H_
