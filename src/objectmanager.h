#pragma once

#ifndef __OBJECTMANAGER_H_
#define __OBJECTMANAGER_H_

#include <map>
#include "object.h"

class ObjectManager {
private:
	struct __ObjectDeallocator {
		void operator()(ObjectInfo* ptr) {
			// if (ptr->address) Allocation::deallocBlock(((char*)(ptr->address) - ptr->offset));
			delete ptr;
		}
	};

	Object __root__;
	Object __current__;
	
	static void createSubObject(const Object& field, const MetaObject& meta);
	int TestReDefinition(const StringId& name);
public:
	ObjectManager();
	Object getRoot();
	Object setRoot(Object root);

	Object getCurrent();
	Object setCurrent(Object current);

	static Object createObjectInfo(const StringId& nameid, MetaObject meta);
	static Object createObjectInfowithDeleter(const StringId& nameid, MetaObject meta);
	static Object createObject(const StringId& nameid, MetaObject meta, const Object& field);
	static Object findObject(Object current, const StringId& nameid);
	static void setParent(Object current, const UncntObject& parent);
	static void eraseObject(Object current);

	Object createObject(const StringId& nameid, MetaObject meta);
	Object findObject(const StringId& nameid);

	MetaObject addArrayMeta(const StringId& extracttype, const long long& size);
	
	MetaObject addSetMetabyId(const StringId& nameid, std::vector<StringId> metaid, std::vector<StringId> memname);
	MetaObject addSetMeta(const StringId& nameid, std::vector<Object> member);
	MetaObject addSetMetaUnnamedbyId(std::vector<StringId> child);
	MetaObject addSetMetaUnnamed(std::vector<Object> child);
	
	MetaObject addFuncMetabyId(const StringId& name, std::vector<StringId> args, std::vector<StringId> rets);
	MetaObject addFuncMeta(const StringId& name, std::vector<Object> args, std::vector<MetaObject> rets);
	MetaObject addFuncMetaUnnamedbyId(std::vector<StringId> args, std::vector<StringId> rets);
	MetaObject addFuncMetaUnnamed(std::vector<Object> args, MetaObject ret);
	
	MetaObject findMeta(const StringId& name);
};


ObjectManager::ObjectManager()
	: __root__(new ObjectInfo, __ObjectDeallocator()), __current__(__root__) {
	__root__->address = nullptr;
	__root__->parent = nullptr;
	__root__->meta = nullptr;
	__root__->nameid = 0;
	__root__->offset = 0;

	StringSet::addString("None");

#define RegisterBasicType(classtype, classname) \
		StringId _id##classname = StringSet::addString(#classname); \
		MetaObject _type##classname = createObjectInfo(_id##classname, nullptr); \
		_type##classname->parent = __root__.get(); \
		__root__->metachild.emplace(_id##classname, _type##classname); \
		_type##classname->offset = sizeof(classtype);
	RegisterBasicType(BasicInt, Int);
	RegisterBasicType(BasicFloat, Float);
	RegisterBasicType(BasicChar, Char);
	RegisterBasicType(BasicBool, Bool);
	RegisterBasicType(Object, Reference);
	RegisterBasicType(void*, Function);
#undef RegisterBasicType
#define RegisterBasicTypezerosize(classname) \
		StringId _id##classname = StringSet::addString(#classname); \
		MetaObject _type##classname = createObjectInfo(_id##classname, nullptr); \
		_type##classname->parent = __root__.get(); \
		__root__->metachild.emplace(_id##classname, _type##classname); \
		_type##classname->offset = 0;
	RegisterBasicTypezerosize(Array);
	RegisterBasicTypezerosize(Set);
	RegisterBasicTypezerosize(Void);
#undef RegisterBasicTypezerosize
}

inline Object ObjectManager::getRoot() {
	return __root__;
}

inline Object ObjectManager::setRoot(Object root) {
	__root__ = root;
	__root__->parent = nullptr;
	return __root__;
}

inline Object ObjectManager::getCurrent() {
	return __current__;
}

inline Object ObjectManager::setCurrent(Object current) {
	__current__ = current;
	return __current__;
}

inline Object ObjectManager::createObjectInfo(const StringId& nameid, MetaObject meta) {
	Object ret = Object(new ObjectInfo);
	ret->nameid = nameid;
	ret->meta = std::move(meta);
	return std::move(ret);
}

inline Object ObjectManager::createObjectInfowithDeleter(const StringId& nameid, MetaObject meta) {
	Object ret = Object(new ObjectInfo, __ObjectDeallocator());
	ret->nameid = nameid;
	ret->meta = std::move(meta);
	return std::move(ret);
}

//inline Object ObjectManager::createTempObject(Object src) {
//	Object ret(createObjectInfo(src->nameid, src->meta));
//	ret->address = src->address;
//	ret->offset = src->offset;
//	ret->
//	return std::move(ret);
//}

inline void ObjectManager::createSubObject(const Object& field, const MetaObject& meta) {
	for (auto it : meta->extrachild) {
		Object cur(createObjectInfo(it.second->nameid, it.second->meta));
		setParent(cur, field.get());
		cur->address = (char*)(field->address) + it.second->offset;
		cur->offset = it.second->offset;
		cur->meta = it.second->meta;
		createSubObject(cur, cur->meta);
	}
}

inline void ObjectManager::setParent(Object current, const UncntObject& parent) {
	parent->extrachild[current->nameid] = current;
	current->parent = parent;
}

inline Object ObjectManager::createObject(const StringId& nameid, MetaObject meta, const Object& field) {
	void* addr = Allocation::allocBlock(meta->offset);
	Object res(createObjectInfowithDeleter(nameid, meta));
	res->address = addr;
	setParent(res, field.get());
#ifdef __DEBUG
	__DEBUG logout("creating Object: \"" + std::to_string(nameid) + 
		"\", size = " + std::to_string(meta->offset) +
		", meta = " + std::to_string(meta->nameid));
#endif // __DEBUG

	createSubObject(res, meta);

	for (auto it : meta->metachild) {
		createObject(it.first, it.second, res);
	}

	return res;
}

inline Object ObjectManager::createObject(const StringId& nameid, MetaObject meta) {
	return createObject(nameid, meta, getCurrent());
}

inline Object ObjectManager::findObject(Object current, const StringId& nameid) {
	return current->findChild(nameid);
}

inline Object ObjectManager::findObject(const StringId& nameid) {
	return getCurrent()->findChild(nameid);
}

inline void ObjectManager::eraseObject(Object current) {

#ifdef __DEBUG

	__DEBUG logout("deleting Object: \"" + std::to_string(current->nameid) +
		"\", size = " + std::to_string(current->meta->offset) +
		", meta = " + std::to_string(current->meta->nameid));

#endif // __DEBUG
	if (current->parent == nullptr) {
		current->extrachild.clear();
	}
	else {
		auto it = current->parent->extrachild.find(current->nameid);
		if (it != current->parent->extrachild.end()) {
			current->parent->extrachild.erase(it);
		}
	}
}


inline Object ObjectManager::addArrayMeta(const StringId& extracttype, const long long& size) {

	MetaObject extract = getCurrent()->findMetaChild(extracttype);

	if (extract == nullptr) return nullptr;

	MetaObject cur = createObjectInfo(BasicMeta::MArray, nullptr);
	setParent(cur, extract->parent);

	cur->metachild.emplace(extract->nameid, extract);

	cur->offset = extract->offset * size;
	return cur;
}

inline int ObjectManager::TestReDefinition(const StringId& name) {
	MetaObject cur = getCurrent()->findMetaChild(name);
	if (cur) {
		Result::output() << "Redefinition of '" + StringSet::findString(name) + "'.\n";
		return -1;
	}
	return 0;
}

inline MetaObject ObjectManager::addSetMetaUnnamedbyId(std::vector<StringId> child) {
	string name = "Set(";
	for (int i = child.size() - 1; i >= 0; --i) {
		name += StringSet::findString(child[i]);
		if (i > 0) name += ',';
	}
	name += ")";

	StringId curid = StringSet::findId(name);
	MetaObject cur;
	if (curid != BasicMeta::MNone) {
		cur = getCurrent()->findMetaChild(curid);
		if (cur) return cur;
	}
	curid = StringSet::addString(name);
	cur = createObjectInfo(curid, getRoot()->metachild[BasicMeta::MSet]);
	size_t offset = 0;
	int unknownname = 0;

	for (int i = child.size() - 1; i >= 0; --i) {
		MetaObject memmeta = getCurrent()->findMetaChild(child[i]);
		if (memmeta == nullptr) {
			Result::output() << "unknown Meta Object\n";
			return nullptr;
		}
		Object mem = createObject(StringSet::addString(std::to_string(unknownname++)), memmeta, cur);
		mem->address = (char*)(mem->address) + offset;
		mem->offset = offset;
		offset += mem->meta->offset;
	}
	cur->offset = offset;
	getCurrent()->metachild.emplace(curid, cur);
	cur->parent = getCurrent().get();
	return cur;
}

inline MetaObject ObjectManager::addSetMetaUnnamed(std::vector<MetaObject> child) {
	string name = "Set(";
	for (int i = child.size() - 1; i >= 0; --i) {
		name += StringSet::findString(child[i]->nameid);
		if (i > 0) name += ',';
	}
	name += ")";

	StringId curid = StringSet::findId(name);
	MetaObject cur;
	if (curid != BasicMeta::MNone) {
		cur = getCurrent()->findMetaChild(curid);
		if (cur) return cur;
	}
	curid = StringSet::addString(name);
	cur = createObjectInfo(curid, getRoot()->metachild[BasicMeta::MSet]);
	size_t offset = 0, size = 0;
	int unknownname = 0;

	for (int i = child.size() - 1; i >= 0; --i) {
		size += child[i]->offset;
	}

	for (int i = child.size() - 1; i >= 0; --i) {
		StringId nameid = StringSet::addString(std::to_string(unknownname++));
		Object member = createObjectInfo(nameid, child[i]);
		setParent(member, cur.get());
		member->offset = offset;
		
		cur->extrachild.emplace(nameid, member);
		offset += child[i]->offset;
	}
	cur->offset = offset;
	getCurrent()->metachild.emplace(curid, cur);
	cur->parent = getCurrent().get();
	return cur;
}

inline MetaObject ObjectManager::addFuncMetaUnnamedbyId(std::vector<StringId> args, std::vector<StringId> rets) {
	string name = "Function(";
	for (int i = args.size() - 1; i >= 0; --i) {
		name += StringSet::findString(args[i]);
		if (i > 0) name += ',';
	}
	name += ")=>(";
	for (int i = rets.size() - 1; i >= 0; --i) {
		name += StringSet::findString(rets[i]);
		if (i > 0) name += ',';
	}
	name += ")";

	StringId curid = StringSet::findId(name);
	MetaObject cur;
	if (curid != BasicMeta::MNone) {
		cur = getCurrent()->findMetaChild(curid);
		if (cur) return cur;
	}
	curid = StringSet::addString(name);

	cur = createObjectInfo(curid, getRoot()->metachild[BasicMeta::MFunction]);

	int argc = 0;
	for (int i = args.size() - 1; i >= 0; --i) {
		MetaObject memmeta = getCurrent()->findMetaChild(args[i]);
		if (memmeta == nullptr) {
			Result::output() << "unknown Meta Object\n";
			return nullptr;
		}
		Object mem = createObject(StringSet::addString(std::to_string(argc++)), memmeta, cur);
	}
	int retc = 0;
	for (int i = rets.size() - 1; i >= 0; --i) {
		MetaObject memmeta = getCurrent()->findMetaChild(args[i]);
		if (memmeta == nullptr) {
			Result::output() << "unknown Meta Object\n";
			return nullptr;
		}
		StringId retname = StringSet::addString(std::to_string(argc++));
		Object mem = createObjectInfo(retname, memmeta);
		cur->metachild.emplace(retname, mem);
	}

	cur->offset = sizeof(BasicInt);
	getCurrent()->metachild.emplace(curid, cur);
	cur->parent = getCurrent().get();
	return cur;
}

inline MetaObject ObjectManager::addFuncMetaUnnamed(std::vector<MetaObject> args, MetaObject ret) {
	string name = "Function(";
	for (int i = 0; i < args.size(); ++i) {
		name += args[i]->getMetaName();
		if (i > 0) name += ',';
	}
	name += ")=>";
	name += ret->getMetaName();

	StringId curid = StringSet::findId(name);
	MetaObject cur;

	if (curid != BasicMeta::MNone) {
		cur = getCurrent()->findMetaChild(curid);
		if (cur) return cur;
	}
	else curid = StringSet::addString(name);

	cur = createObjectInfo(BasicMeta::MFunction, nullptr);

	int argc = 0;
	for (int i = 0; i < args.size(); ++i) {
		args[i]->parent = cur.get();
		cur->metachild.emplace(StringSet::addString("@arg" + std::to_string(argc++)), args[i]);
	}
	ret->parent = cur.get();
	cur->metachild.emplace(StringSet::addString("@ret"), ret);

	cur->offset = sizeof(void*);
	auto rec = getCurrent().get();
	while (rec->parent != nullptr) {
		for (auto it1 : rec->metachild) {
			for (auto it2 : args) {
				if (it1.second == it2) goto Target;
			}
			if (it1.second == ret) goto Target;
		}
		rec = rec->parent;
	}
	Target:
	rec->metachild.emplace(curid, cur);
	cur->parent = rec;
	return cur;
}

inline MetaObject ObjectManager::addSetMetabyId(const StringId& nameid, std::vector<StringId> metanameid, std::vector<StringId> memnameid) {

	if (TestReDefinition(nameid) == -1) return nullptr;

	MetaObject cur = createObjectInfo(nameid, getRoot()->metachild[BasicMeta::MSet]);

	size_t offset = 0;
	int unknownname = 0;
	auto nameit = memnameid.begin();

	for (auto it : metanameid) {
		MetaObject mem = getCurrent()->findMetaChild(it);

		StringId name;
		if (nameit != memnameid.end()) name = *(nameit++);
		else name = StringSet::addString(std::to_string(unknownname++));

		MetaObject member = createObject(name, mem, cur);
		member->address = (char*)(member->address) + offset;
		member->offset = offset;
		offset += member->meta->offset;
	}
	cur->offset = offset;

	cur->parent = getCurrent().get();
	getCurrent()->metachild.emplace(nameid, cur);
	return cur;
}

inline MetaObject ObjectManager::addSetMeta(const StringId& nameid, std::vector<Object> member) {
	if (TestReDefinition(nameid) == -1) return nullptr;

	MetaObject cur = createObjectInfo(BasicMeta::MSet, nullptr);

	size_t offset = 0;

	for (auto& mem : member) {

		mem->parent = cur.get();
		cur->extrachild.emplace(mem->nameid, mem);

		mem->address = (char*)(mem->address) + offset;
		mem->offset = offset;
		offset += mem->meta->offset;
	}
	cur->offset = offset;

	auto rec = getCurrent().get();
	while (rec->parent != nullptr) {
		for (auto it1 : rec->metachild) {
			for (auto it2 : member) {
				if (it1.second == it2) goto Target;
			}
		}
		rec = rec->parent;
	}
Target:
	rec->metachild.emplace(nameid, cur);
	cur->parent = rec;
	return cur;
}

inline MetaObject ObjectManager::addFuncMetabyId(const StringId& nameid, std::vector<StringId> args, std::vector<StringId> rets) {
	
	if (TestReDefinition(nameid) == -1) return nullptr;

	MetaObject cur = createObjectInfo(nameid, getRoot()->metachild[BasicMeta::MFunction]);

	int argc = 0;
	for (auto it : args) {
		MetaObject mem = getCurrent()->findMetaChild(it);
		StringId memnameid = StringSet::addString(std::to_string(argc++));
		MetaObject member = createObject(memnameid, mem, cur);
	}
	int retc = 0;
	for (auto it : rets) {
		MetaObject mem = getCurrent()->findMetaChild(it);
		StringId memnameid = StringSet::addString(std::to_string(retc++));
		MetaObject member = createObjectInfo(memnameid, mem);
		member->parent = cur.get();
		cur->metachild.emplace(memnameid, member);
	}
	cur->offset = sizeof(void*);
	cur->parent = getCurrent().get();
	getCurrent()->metachild.emplace(nameid, cur);
	return cur;
}

inline MetaObject ObjectManager::addFuncMeta(const StringId& nameid, std::vector<Object> args, std::vector<MetaObject> rets) {
	
	MetaObject cur = createObjectInfo(nameid, getRoot()->metachild[BasicMeta::MFunction]);

	for (auto mem : args) {
		mem->parent = cur.get();
		cur->extrachild.emplace(mem->nameid, mem);
	}
	int retc = 0;
	for (auto mem : rets) {
		mem->parent = cur.get();
		StringId memnameid = StringSet::addString(std::to_string(retc++));
		mem->nameid = memnameid;
		cur->metachild.emplace(mem->nameid, mem);
	}
	cur->offset = sizeof(void*);
	cur->parent = getCurrent().get();
	getCurrent()->metachild.emplace(nameid, cur);
	return cur;
}

MetaObject ObjectManager::findMeta(const StringId& nameid) {
	return getCurrent()->findMetaChild(nameid);
}

#endif // !__OBJECTMANAGER_H_
