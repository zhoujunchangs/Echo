#pragma once

#ifndef __OPCODE_H_
#define __OPCODE_H_

enum OpCodeType {

	OpEnd = 0,

	OpLogic_And = 1,
	OpLogic_Or = 2,
	OpLogic_Not = 3,
	OpLogic_Equal = 4,
	OpLogic_NotEqual = 5,
	OpLogic_Greater = 6,
	OpLogic_GreaterEqual = 7,
	OpLogic_Less = 8,
	OpLogic_LessEqual = 9,

	OpPlus = 10,
	OpMinus = 11,
	OpMultiply = 12,
	OpDevide = 13,

	OpAssignTo = 37,
	OpAssign = 34,
	OpRefer = 35,

	OpPush = 14,
	OpPop = 15,
	OpVisit = 16,
	OpReturn = 17,
	OpExtractArray = 18,
	OpExtractReference = 19,
	OpExtractDot = 20,

	OpIf = 21,
	OpMoveIn = 22,
	OpMoveOut = 23,
	OpGoto = 24,

	OpLoadConsts = 25,
	OpLoadObject = 26,
	OpLoadMeta = 28,

	OpAddSetMeta = 29,
	OpAddFuncMeta = 30,
	OpAddArrayMeta = 31,

	OpAddObject = 33,
	OpAddFunction = 36,
	OpPushCurrent = 38,

	OpLoadObjectMeta = 39,

	OpPrint = 40,

	OpGotoIfTrue = 41,
	OpGotoIfFalse = 42,

	OpError = 255,

};

using OpCode = char;
using OpCodeArg = int;

#endif // !__OPCODE_H_
