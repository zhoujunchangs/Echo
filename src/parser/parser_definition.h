#pragma once

#ifndef __PARSER_DEFINITION_H_
#define __PARSER_DEFINITION_H_

#include "parser_base.h"
#include "parser_type.h"
#include "parser_object.h"

namespace AST {

	using TypeNamePtr = uniptr<TypeNameAST>;

	class ObjectDefAST :public BaseAST {
		TypeNamePtr typestr;
		string objectstr;
		BlockPtr body;
	public:
		ObjectDefAST(TypeNamePtr _type, string _object, BlockPtr _body = nullptr) 
			:typestr(std::move(_type)), objectstr(std::move(_object)), body(std::move(_body)) {
			type = AstType::AstObjectDefinition;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			size_t ret = typestr->Analyse(param);
			if(ret ==-1) return -1;

			if (typestr->name == "@Function") {
				StringId id = StringSet::addString(objectstr);
				param.opset.push_back(OpCodeType::OpAddFunction);
				param.pushArg(id);
				size_t loc1 = param.opset.size();
				param.pushArg(0);
				size_t beg = param.opset.size();

				if (body) {

					for (int i = ret - 1; i >= 0; --i) {
						param.opset.push_back(OpCodeType::OpAssignTo);
						param.pushArg(StringSet::addString("@arg" + std::to_string(i)));
					}
					body->Analyse(param);

					param.opset.push_back(OpCodeType::OpReturn);

				}

				size_t end = param.opset.size();
				*((OpCodeArg*)(&param.opset[loc1])) = end - beg;
				return 0;
			}

			StringId id = StringSet::addString(objectstr);

			param.opset.push_back(OpCodeType::OpAddObject);
			param.pushArg(id);

			return 0;
		}
	};
	using ObjectDefPtr = uniptr<ObjectDefAST>;


	using DefPack = vector<uniptr<BaseAST>>;

	class ClassDefAST : public BaseAST {
		DefPack member;
		string classname;
	public:
		ClassDefAST(const string& _classname, DefPack _member)
			:classname(_classname), member(std::move(_member)) {
			type = AstType::AstClassDefinition;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			for (auto& it : member) {
				if (it->Analyse(param) == -1) return -1;
			}
			StringId id = StringSet::addString(classname);

			param.opset.push_back(OpCodeType::OpAddSetMeta);
			param.pushArg(id);
			param.pushArg(member.size());

			return 0;
		}
	};

	using ClassDefPtr = uniptr<ClassDefAST>;

	BasePtr ParseDef();

	DefPack ParseDefBlock() {
		if (getCurrentToken().type != TokenSet::Left_brace) return DefPack();
		getNextToken(); // consume {
		DefPack pack;
		while (getCurrentToken().type != TokenSet::Right_brace) {
			auto def = ParseDef();
			if (!def) return DefPack();
			
			pack.push_back(std::move(def));
			while (getCurrentToken().type == TokenSet::Semicolon) getNextToken();
		}
		getNextToken(); // consume }
		return std::move(pack);
	}

	ObjectDefPtr ParseObject(const string& name) {
		getNextToken(); // consume :
		
		auto type = ParseType();
		BlockPtr body;
		if (!type) return nullptr;
		if (type->name == "@Function" && getCurrentToken().type == TokenSet::Left_brace) {
			body = ParseBlock();
		}
		return make_unique<ObjectDefAST>(std::move(type), name, std::move(body));
	}

	ClassDefPtr ParseClass(const string& name) {
		EXPECT_TOKEN(brace, TokenSet::Left_brace);

		DefPack pack = ParseDefBlock();
		if (pack.size() == 0) return nullptr;
		
		return make_unique<ClassDefAST>(name, std::move(pack));
	}

	BasePtr ParseDef() {
		while (getCurrentToken().type == TokenSet::Semicolon) getNextToken();
		EXPECT_TOKEN_RET(nametoken, TokenSet::Name);
		getNextToken(); // consume Name
		
		if (getCurrentToken().type == TokenSet::Colon) 
			return ParseObject(nametoken.data);
		else 
			return ParseClass(nametoken.data);
	}

}

#endif // !__PARSER_DEFINITION_H_
