#pragma once

#ifndef __PARSER_TYPE_H_
#define __PARSER_TYPE_H_

#include "parser_base.h"

namespace AST {

	class TypeNameAST :public BaseAST {
		friend uniptr<TypeNameAST> ParseType();
		using TypeNamePack = vector<uniptr<TypeNameAST>>;
        TypeNamePack args;
        uniptr<TypeNameAST> ret;
    private:
        vector<StringId> AnalyseArgs(Analyzer::AnalyzerParam& param) {
            vector<StringId> type;
            for (auto& it : args) {
                auto id = it->Analyse(param);
                if (id == -1) {
                    return { -1 };
                }
                else {
                    type.push_back(id);
                }
            }
            return std::move(type);
        }
	public:
        string name;
		TypeNameAST(const string& _name, TypeNamePack _args = TypeNamePack(), uniptr<TypeNameAST> _ret = nullptr)
			: name(_name), args(std::move(_args)), ret(std::move(_ret)) {
			type = AstType::AstTypeName;
		}
        int Analyse(Analyzer::AnalyzerParam& param) override {
            if (name == "@Function") {
                vector<StringId> argtype = AnalyseArgs(param);
                if (argtype.size() == 1 && argtype[0] == -1) return -1;
                StringId rettype = ret->Analyse(param);
                if (rettype == -1) return -1;

                param.opset.push_back(OpCodeType::OpAddFuncMeta);
                param.pushArg(argtype.size());
                return argtype.size();
            }
            else if (name == "@Set") {
                vector<StringId> typepack = AnalyseArgs(param);
                if (typepack.size() == 1 && typepack[0] == -1) return -1;
                StringId setname = StringSet::addString(name);

                param.opset.push_back(OpCodeType::OpAddSetMeta);
                param.pushArg(setname);
                param.pushArg(typepack.size());
                return 0;
            }
            else {
                StringId type = StringSet::addString(name);
                param.opset.push_back(OpCodeType::OpLoadMeta);
                param.pushArg(type);
                return 0;
            }
        }
	};

	uniptr<TypeNameAST> ParseType();

}


namespace AST {

    using TypeNamePack = vector<uniptr<TypeNameAST>>;

    uniptr<TypeNameAST> ParseParam() {
        Token curtoken = getCurrentToken();
        if (curtoken.type != TokenSet::Name) {
            Result::output() << "expected a typename, found '" + curtoken.data + "'.";
            return nullptr;
        }
        return make_unique<TypeNameAST>(getNextToken().data);
    }

    TypeNamePack ParseParamPack() {
        TypeNamePack args;
        Token curtoken = getCurrentToken();
        if (curtoken.type == TokenSet::Left_parenthesis) {
            getNextToken(); // consume '('
            while (true) {
                if (getCurrentToken().type == TokenSet::Right_parenthesis) {
                    getNextToken(); // consume ')'
                    break;
                }
                auto arg = ParseType();
                if (!arg) return TypeNamePack();

                args.push_back(std::move(arg));

                if (getCurrentToken().type != TokenSet::Comma) {
                    Result::output() << "expected a ',' or ')'.";
                }
                else getNextToken(); // consume ','
            }
        }
        else {
            auto par = ParseParam();
            args.push_back(std::move(par));
            if (getCurrentToken().type == TokenSet::Deduction) {
                getNextToken();
                auto ret = ParseType();
                TypeNamePack pak;
                pak.push_back(std::make_unique<TypeNameAST>("@Function", std::move(args), std::move(ret)));
                return pak;
            }
            
        }
        return args;
    }

    uniptr<TypeNameAST> ParseType() {
        auto args = ParseParamPack();
        uniptr<TypeNameAST> temp, ret;
        if (getCurrentToken().type == TokenSet::Deduction) {
            getNextToken();
            ret = ParseType();
            temp = make_unique<TypeNameAST>("@Function", std::move(args), std::move(ret));
        }
        else {
            temp = make_unique<TypeNameAST>("@Set", std::move(args));
            while (temp->args.size() == 1 && temp->name == "@Set") {
                temp = std::move(temp->args[0]);
                if (temp == nullptr) break;
            }
        }
        return std::move(temp);
    }

}

#endif // !__PARSER_TYPE_H_
