#pragma once

#ifndef __EXECUTOR_H_
#define __EXECUTOR_H_

#include "opcode.h"
#include "objectmanager.h"
#include "analyzer.h"

#include <stack>
namespace Executor {

	static ObjectManager objmgr;
	static std::stack<Object> stack;
	
using ErrorReturnType = int;
namespace Detail {
	ErrorReturnType HandleAssign(Object r, Object l) {
		if (l->meta == r->meta) {
			if (l->meta->nameid == BasicMeta::MFunction) l->parent = r->parent;
			memcpy(l->get(), r->get(), l->meta->offset);
		}
		else if (l->meta->nameid == BasicMeta::MInt) {
			EXTRACTADDRESS(l, BasicInt) = Content::toInt(r);
		}
		else if (l->meta->nameid == BasicMeta::MFloat) {
			EXTRACTADDRESS(l, BasicFloat) = Content::toFloat(r);
		}
		else return -1;
		return 0;
	}

	static Object HandleMinus(Object r, Object l) {
		Object result;
		if (l->meta->nameid == BasicMeta::MFloat || r->meta->nameid == BasicMeta::MFloat) {
			result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MFloat]);
			EXTRACTADDRESS(result, BasicFloat) = Content::toFloat(l) - Content::toFloat(r);
		}
		else if (l->meta->nameid == BasicMeta::MInt || r->meta->nameid == BasicMeta::MInt) {
			result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MInt]);
			EXTRACTADDRESS(result, BasicInt) = Content::toInt(l) - Content::toInt(r);
		}
		return std::move(result);
	}

	static Object HandleMultiply(Object r, Object l) {
		Object result;
		if (l->meta->nameid == BasicMeta::MFloat || r->meta->nameid == BasicMeta::MFloat) {
			result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MFloat]);
			EXTRACTADDRESS(result, BasicFloat) = Content::toFloat(l) * Content::toFloat(r);
		}
		else if (l->meta->nameid == BasicMeta::MInt || r->meta->nameid == BasicMeta::MInt) {
			result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MInt]);
			EXTRACTADDRESS(result, BasicInt) = Content::toInt(l) * Content::toInt(r);
		}
		return std::move(result);
	}

	static Object HandleDevide(Object r, Object l) {
		Object result;
		if (l->meta->nameid == BasicMeta::MFloat || r->meta->nameid == BasicMeta::MFloat) {
			result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MFloat]);
			EXTRACTADDRESS(result, BasicFloat) = Content::toFloat(l) / Content::toFloat(r);
		}
		else if (l->meta->nameid == BasicMeta::MInt || r->meta->nameid == BasicMeta::MInt) {
			result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MInt]);
			EXTRACTADDRESS(result, BasicInt) = Content::toInt(l) / Content::toInt(r);
		}
		return std::move(result);
	}

	static Object HandlePlus(Object r, Object l) {
		Object result;
		if (l->meta->nameid == BasicMeta::MFloat || r->meta->nameid == BasicMeta::MFloat) {
			result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MFloat]);
			EXTRACTADDRESS(result, BasicFloat) = Content::toFloat(l) + Content::toFloat(r);
		}
		else if (l->meta->nameid == BasicMeta::MInt || r->meta->nameid == BasicMeta::MInt) {
			result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MInt]);
			EXTRACTADDRESS(result, BasicInt) = Content::toInt(l) + Content::toInt(r);
		}
		return std::move(result);
	}

#define HANDLECONDITION(name, condition) \
	static Object Handle##name(Object r, Object l) { \
	Object result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MBool]); \
	if ((condition)) { \
		EXTRACTADDRESS(result, BasicBool) = 1; \
	} \
	else EXTRACTADDRESS(result, BasicBool) = 0; \
	return std::move(result); \
	}

	HANDLECONDITION(Equal,
		((l->meta->nameid == BasicMeta::MFloat || r->meta->nameid == BasicMeta::MFloat) &&
			Content::UppertoFloat(l) == Content::UppertoFloat(r)) ||
		((l->meta->nameid == BasicMeta::MInt || r->meta->nameid == BasicMeta::MInt) &&
			Content::UppertoInt(l) == Content::UppertoInt(r)) ||
		((l->meta->nameid == r->meta->nameid) && memcmp(l->get(), r->get(), l->meta->offset) == 0)
	);

	HANDLECONDITION(NotEqual,
		!(((l->meta->nameid == BasicMeta::MFloat || r->meta->nameid == BasicMeta::MFloat) &&
				Content::UppertoFloat(l) == Content::UppertoFloat(r)) ||
			((l->meta->nameid == BasicMeta::MInt || r->meta->nameid == BasicMeta::MInt) &&
				Content::UppertoInt(l) == Content::UppertoInt(r)) ||
			((l->meta->nameid == r->meta->nameid) && memcmp(l->get(), r->get(), l->meta->offset) == 0)
			)
	);

	static Object HandleAnd(Object r, Object l) {
		if (r->meta == nullptr || r->meta->nameid != StringSet::findId("Bool")) return nullptr;
		if (l->meta == nullptr || l->meta->nameid != StringSet::findId("Bool")) return nullptr;
		Object result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MBool]);
		if (EXTRACTADDRESS(l, BasicBool) != 0 && EXTRACTADDRESS(r, BasicBool) != 0) {
			EXTRACTADDRESS(result, BasicBool) = 1;
		}
		else EXTRACTADDRESS(result, BasicBool) = 0;
		return std::move(result);
	}

	static Object HandleOr(Object r, Object l) {
		if (r->meta == nullptr || r->meta->nameid != StringSet::findId("Bool")) return nullptr;
		if (l->meta == nullptr || l->meta->nameid != StringSet::findId("Bool")) return nullptr;
		Object result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MBool]);
		if (EXTRACTADDRESS(l, BasicBool) != 0 || EXTRACTADDRESS(r, BasicBool) != 0) {
			EXTRACTADDRESS(result, BasicBool) = 1;
		}
		else EXTRACTADDRESS(result, BasicBool) = 0;
		return std::move(result);
	}

	HANDLECONDITION(Greater,
		Content::UppertoFloat(l) > Content::UppertoFloat(r)
	);

	HANDLECONDITION(Less,
		Content::UppertoFloat(l) < Content::UppertoFloat(r)
	);

	HANDLECONDITION(Greaterequal,
		Content::UppertoFloat(l) >= Content::UppertoFloat(r)
	);

	HANDLECONDITION(Lessequal,
		Content::UppertoFloat(l) <= Content::UppertoFloat(r)
	);

	static Object HandleNot(Object par) {
		Object result = objmgr.createObject(0, objmgr.getRoot()->metachild[BasicMeta::MBool]);
		if (EXTRACTADDRESS(par, BasicBool) == 0) {
			EXTRACTADDRESS(result, BasicBool) = 1;
		}
		else EXTRACTADDRESS(result, BasicBool) = 0;
		return std::move(result);
	}
}





	static inline int init() {
		Analyzer::instance().innermeta.push_back(nullptr);
		for (int i = 1; i < Executor::objmgr.getRoot()->metachild.size(); ++i) {
			Analyzer::instance().innermeta.push_back(Executor::objmgr.getRoot()->metachild.find(i)->second);
		}
		return 0;
	}

	static inline int Execute(Analyzer::AnalyzerParam& param, OpCode* flow) {

		OpCodeArg arg, arg2;
		OpCode op;
		Object obj1, obj2, obj3;


#define NEXTOP() break;
#define POP() if(!stack.empty()) stack.pop();
#define PUSH(obj) stack.push(obj);
#define TOP() ((stack.empty())?(nullptr):(stack.top()))
#define FINDOBJECT(ret, arg) \
	do { \
		ret = objmgr.findObject(arg); \
		if (ret == nullptr) { \
			Result::output() << "cannot find object\n"; \
			goto Error; \
		} \
	} while (0);
#define FINDMETA(ret, arg) \
	do { \
		ret = objmgr.findMeta(arg); \
		if (ret == nullptr) { \
			Result::output() << "cannot find meta\n"; \
			goto Error; \
		} \
	} while (0);
#define FETCHARGS(opcodeflow, arg) \
	do { \
		++opcodeflow; \
		arg = *((OpCodeArg*)(opcodeflow)); \
		flow = &(flow[sizeof(OpCodeArg)/sizeof(OpCode)-1]); \
	} while (0);
#define INNERMETA(name) \
	(objmgr.getCurrent()->findMetaChild(name))



		for (; flow -&(param.opset[0]) < param.opset.size(); ++flow) {
			switch (op = *flow) {
			case OpCodeType::OpLoadObject: {
				FETCHARGS(flow, arg);
				FINDOBJECT(obj1, arg);
				PUSH(obj1);
				NEXTOP();
			}
			case OpCodeType::OpPrint: {
				Result::output() << ">";
				if (TOP()) TOP()->print(Result::output());
				Result::output() << std::endl;
				NEXTOP();
			}
			case OpCodeType::OpLoadConsts: {
				FETCHARGS(flow, arg);
				obj1 = param.consts[arg];
				if (obj1 == nullptr) {
					Result::output() << "Unknown Consts.\n";
					goto Error;
				}
				PUSH(obj1);
				NEXTOP();
			}
			case OpCodeType::OpLoadMeta: {
				FETCHARGS(flow, arg);
				FINDMETA(obj1, arg);
				PUSH(obj1);
				NEXTOP();
			}
			case OpCodeType::OpIf: {
				FETCHARGS(flow, arg);
				obj1 = TOP(); POP();
				if (obj1->meta != INNERMETA(BasicMeta::MBool)) {
					Result::output() << "If test must be a bool object.\n";
					goto Error;
				}
				if (EXTRACTADDRESS(obj1, BasicBool) == 0) {
					flow = flow + arg;
				}
				NEXTOP();
			}
			case OpCodeType::OpPop: {
				//std::cout << ">Pop: ";
				//TOP()->print();
				//std::cout << std::endl;
				POP();
				NEXTOP();
			}
			case OpCodeType::OpVisit: {
				obj1 = TOP(); POP(); 
				objmgr.setCurrent(obj1);
				Execute(param, &param.opset[EXTRACTADDRESS(obj1, BasicInt)]);
				NEXTOP();
			}
			case OpCodeType::OpPushCurrent: {
				PUSH(objmgr.getCurrent());
				NEXTOP();
			}
			case OpCodeType::OpAssign: {
				obj1 = TOP();
				POP();
				if (Detail::HandleAssign(std::move(obj1), TOP()) == -1) {
					goto Error;
				}
				NEXTOP();
			}
			case OpCodeType::OpAssignTo: {
				FETCHARGS(flow, arg);
				FINDOBJECT(obj1, arg);
				if (Detail::HandleAssign(TOP(), std::move(obj1)) == -1) {
					goto Error;
				}
				POP();
				NEXTOP();
			}
			case OpCodeType::OpPlus: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandlePlus(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpMinus: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleMinus(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpMultiply: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleMultiply(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpDevide: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleDevide(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpExtractDot: {
				FETCHARGS(flow, arg);
				obj1 = TOP(); POP();
				auto it = obj1->extrachild.find(arg);
				if (it == obj1->extrachild.end()) {
					Result::output() << "doesn't have child.\n";
					goto Error;
				}
				PUSH(it->second);
				NEXTOP();
			}
			case OpCodeType::OpAddSetMeta: {
				FETCHARGS(flow, arg);
				FETCHARGS(flow, arg2);

				vector<Object> member;
				for (int i = 0; i < arg2; ++i) {
					member.push_back(TOP()); POP();
				}
				PUSH(objmgr.addSetMeta(arg, std::move(member)));
				NEXTOP();
			}

			case OpCodeType::OpAddFuncMeta: {
				FETCHARGS(flow, arg);

				vector<Object> args(arg);
				Object ret = TOP(); POP();
				for (int i = arg-1; i >=0; --i) {
					args[i] = TOP(); POP();
				}
				Object newfunc = objmgr.addFuncMetaUnnamed(args, ret);
				PUSH(newfunc);
				NEXTOP();
			}
			case OpCodeType::OpEnd: {
				goto Finally;
			}
			case OpCodeType::OpAddObject: {
				FETCHARGS(flow, arg);
				obj1 = TOP(); POP();
				obj2 = objmgr.createObject(arg, obj1); 
				if (obj2 == nullptr) {
					Result::output() << "cannot create object.\n";
					goto Error;
				}
				PUSH(obj2);
				NEXTOP();
			}
			case OpCodeType::OpAddFunction: {
				FETCHARGS(flow, arg);
				FETCHARGS(flow, arg2);
				obj1 = TOP(); POP();
				obj2 = objmgr.createObject(arg, obj1);
				if (obj2 == nullptr) {
					Result::output() << "cannot create function.\n";
					goto Error;
				}
				EXTRACTADDRESS(obj2, BasicInt) = (flow - &param.opset[0]) + 1;
				flow = flow + arg2;
				PUSH(obj2);
				NEXTOP();
			}
			case OpCodeType::OpReturn: {
				obj1 = TOP(); POP();
				FINDOBJECT(obj2, StringSet::findId("@ret"));
				PUSH(obj2);
				objmgr.setCurrent(obj1);
				goto Finally;
			}
			case OpCodeType::OpLoadObjectMeta: {
				obj1 = TOP(); POP();
				PUSH(obj1->meta);
				NEXTOP();
			}
			case OpCodeType::OpLogic_Less: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleLess(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpLogic_Not: {
				obj1 = TOP(); POP();
				obj2 = Detail::HandleNot(std::move(obj1));
				PUSH(obj2);
				NEXTOP();
			}
			case OpCodeType::OpLogic_Greater: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleGreater(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpLogic_Equal: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleEqual(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpLogic_NotEqual: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleNotEqual(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpLogic_LessEqual: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleLessequal(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpLogic_And: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleAnd(std::move(obj1), std::move(obj2));
				if (obj3 == nullptr) {
					Result::output() << "Not a bool expression.\n";
				}
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpLogic_Or: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleOr(std::move(obj1), std::move(obj2));
				if (obj3 == nullptr) {
					Result::output() << "Not a bool expression.\n";
				}
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpLogic_GreaterEqual: {
				obj1 = TOP(); POP();
				obj2 = TOP(); POP();
				obj3 = Detail::HandleGreaterequal(std::move(obj1), std::move(obj2));
				PUSH(obj3);
				NEXTOP();
			}
			case OpCodeType::OpGoto: {
				FETCHARGS(flow, arg);
				flow = (char*)(flow)+arg;
				NEXTOP();
			}
			case OpCodeType::OpGotoIfTrue: {
				FETCHARGS(flow, arg);
				if (TOP()->meta->nameid != StringSet::findId("Bool")) {
					Result::output() << "Not a bool expression.\n";
					goto Error;
				}
				if (EXTRACTADDRESS(TOP(), BasicBool) != 0) {
					flow = (char*)(flow)+arg;
				}
				NEXTOP();
			}
			case OpCodeType::OpGotoIfFalse: {
				FETCHARGS(flow, arg);
				if (TOP()->meta->nameid != StringSet::findId("Bool")) {
					Result::output() << "Not a bool expression.\n";
					goto Error;
				}
				if (EXTRACTADDRESS(TOP(), BasicBool) == 0) {
					flow = (char*)(flow)+arg;
				}
				NEXTOP();
			}
			default:
				Result::output() << "Unknown Op.\n";
				goto Error;
			}
			obj1 = nullptr; obj2 = nullptr; obj3 = nullptr;
		}



Finally:
		return 0;
Error:
		Result::output() << "Fatal Error.\n" << std::endl;
		return -1;
	}
}

#endif // !__EXECUTOR_H_
