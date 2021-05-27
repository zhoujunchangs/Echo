#pragma once

#ifndef __TESTUTIL_H_
#define __TESTUTIL_H_

#include "token.h"
#include "../parser/parser.h"
#include "analyzer.h"
#include "../executor.h"

namespace TestUtil {

	void beginTestOutput(string content) {
		Result::output() << "==============================" << std::endl;
		Result::output() << ">>> " << content << ": " << std::endl;
	}

	void outputOpCode(OpCode* opcode, int size) {
#define JUSTIFY0(ptr, type) \
	case OpCodeType::type:{ \
		Result::output() << #type << std::endl; \
		break; \
	}
#define JUSTIFY1(ptr, type) \
	case OpCodeType::type:{ \
		Result::output() << #type << " "; \
		OpCodeArg arg = *((OpCodeArg*)(++ptr)); \
		ptr = ptr + sizeof(OpCodeArg)/sizeof(OpCode) -1;\
		Result::output() << arg << std::endl; \
		break; \
	}
#define JUSTIFY2(ptr, type) \
	case OpCodeType::type:{ \
		Result::output() << #type << " "; \
OpCodeArg arg = *((OpCodeArg*)(++ptr)); \
ptr = ptr + sizeof(OpCodeArg) / sizeof(OpCode); \
Result::output() << arg << " "; \
OpCodeArg arg2 = *((OpCodeArg*)(ptr)); \
		ptr = ptr + sizeof(OpCodeArg)/sizeof(OpCode)-1; \
		Result::output() << arg2 << std::endl; \
		break; \
	}


		beginTestOutput("OpCode");
		OpCode* ptr = opcode;
		for (; ptr - opcode < size; ++ptr) {
			switch (*ptr) {
				JUSTIFY0(ptr, OpEnd)
				JUSTIFY0(ptr, OpPop)
				JUSTIFY0(ptr, OpPushCurrent)
				JUSTIFY0(ptr, OpVisit)
				JUSTIFY0(ptr, OpReturn)
				JUSTIFY0(ptr, OpPrint)
				JUSTIFY0(ptr, OpLogic_And)
				JUSTIFY0(ptr, OpLogic_Or)
				JUSTIFY0(ptr, OpLogic_Not)
				JUSTIFY0(ptr, OpLogic_Equal)
				JUSTIFY0(ptr, OpLogic_NotEqual)
				JUSTIFY0(ptr, OpLogic_Greater)
				JUSTIFY0(ptr, OpLogic_GreaterEqual)
				JUSTIFY0(ptr, OpLogic_Less)
				JUSTIFY0(ptr, OpLogic_LessEqual)
				JUSTIFY0(ptr, OpPlus)
				JUSTIFY0(ptr, OpMinus)
				JUSTIFY0(ptr, OpMultiply)
				JUSTIFY0(ptr, OpDevide)
				JUSTIFY0(ptr, OpAssign)
				JUSTIFY0(ptr, OpRefer)
				JUSTIFY0(ptr, OpExtractArray)
				JUSTIFY0(ptr, OpExtractReference)
				JUSTIFY0(ptr, OpLoadObjectMeta)

				JUSTIFY1(ptr, OpIf)
				JUSTIFY1(ptr, OpPush)
				JUSTIFY1(ptr, OpGoto)
				JUSTIFY1(ptr, OpGotoIfTrue)
				JUSTIFY1(ptr, OpGotoIfFalse)
				JUSTIFY1(ptr, OpExtractDot)
				JUSTIFY1(ptr, OpAssignTo)
				JUSTIFY1(ptr, OpAddFuncMeta)
				JUSTIFY1(ptr, OpAddObject)
				JUSTIFY1(ptr, OpLoadMeta)
				JUSTIFY1(ptr, OpLoadConsts)
				JUSTIFY1(ptr, OpLoadObject)

				JUSTIFY2(ptr, OpAddSetMeta)
				JUSTIFY2(ptr, OpAddArrayMeta)
				JUSTIFY2(ptr, OpAddFunction)
			default:
				Result::output() << "ERROROP" << std::endl;
			}
		}
#undef JUSTIFY0
#undef JUSTIFY1
#undef JUSTIFY2
	}

	void stacksize() {
		beginTestOutput("stackremains");
		Result::output() << Executor::stack.size() << std::endl;
	}

	void showStringSet() {
		beginTestOutput("StringSet");
		for (auto it : StringSet::stringset) {
			Result::output() << it.first << ' ' << it.second << std::endl;
		}
	}

}

#endif // !__TESTUTIL_H_
