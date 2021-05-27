#pragma once

#ifndef __PARSER_BASE_H_
#define __PARSER_BASE_H_

#include "util.h"
#include "stringset.h"
#include "token.h"
#include "opcode.h"
#include "../objectmanager.h"
#include "analyzer.h"

namespace AST {

	enum AstType {
		AstUnknown,				
//		AstName,					
		AstObjectName,			
		AstTypeName,			
//		AstDefinition,				
		AstClassDefinition,		
		AstObjectDefinition,	
//		AstExpression,				
//		AstObject,					
		AstLiteral,				
		AstBool,			
		AstInt,				
		AstFloat,			
		AstChar,			
		AstLiteralEnd,
		AstOperator,				
		_AstNullOperatorLHS,	
		_AstNullOperatorRHS,	
//		AstControlflow,				
		AstBlock,				
		AstLoop,				
		AstCondition,			
		AstBreak,				
		AstContinue,			
		AstIn,	    			
		AstOut, 	    		
		AstEnd,						
		AstPrint,
	};

	static inline bool isNoPopAst(const AstType& asttype) {
		return asttype == AstType::AstBlock || asttype == AstType::AstCondition ||
			asttype == AstType::AstLoop || asttype == AstType::AstIn || asttype == AstType::AstOut;
	}

	class BaseAST {
	protected:
		AstType type;
	public:
		BaseAST() : type(AstType::AstUnknown) {}
		AstType getType() const { return type; }
		virtual int Analyse(Analyzer::AnalyzerParam& param) { return -1; }
		virtual ~BaseAST() {}
	};

	using BasePtr = uniptr<BaseAST>;
	using BaseBlock = vector<BasePtr>;

	class BlockAST :public BaseAST {
		friend class PrintBlockAST;
		BaseBlock block;
	public:
		BlockAST(BaseBlock _block)
			: block(std::move(_block)) {
			type = AstType::AstBlock;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			for (auto& it : block) {
				it->Analyse(param);
				if (!isNoPopAst(it->getType())) {
					param.opset.push_back(OpCodeType::OpPop);
				}
			}
			return block.size();
		}
	};

	class UnPopedBlockAST :public BaseAST {
		BaseBlock block;
	public:
		UnPopedBlockAST(BaseBlock _block)
			: block(std::move(_block)) {
			type = AstType::AstBlock;
		}
		int Analyse(Analyzer::AnalyzerParam& param) override {
			for (auto& it : block) {
				it->Analyse(param);
			}
			return block.size();
		}
	};

	using BlockPtr = uniptr<BlockAST>;
	using UnPopedBlockPtr = uniptr<UnPopedBlockAST>;

	static BasePtr ParseOnce();
	static BlockPtr ParseBlock();
}

#endif // !__PARSER_BASE_H_
