#pragma once

#ifndef __OBJECT_H_
#define __OBJECT_H_

#include "stringset.h"
#include "result.h"
#include "metaobject.h"
#include "allocation.h"
#include <string>
#include <map>

#define EXTRACTADDRESS(object, meta) (*((meta*)(object->get())))

struct ObjectInfo;

using Object = std::shared_ptr<ObjectInfo>;
using MetaObject = Object;
using UncntObject = ObjectInfo*;

struct ObjectInfo {
	MetaObject meta;
	StringId nameid;

	Allocation::MemoryBlock address;
	size_t offset;

	std::map<StringId, Object> metachild;
	std::map<StringId, Object> extrachild;
	UncntObject parent;

	ObjectInfo() : meta(nullptr), address(nullptr), parent(nullptr), offset(0), nameid(0) {}
	
	Object findExtraChild(const StringId& nameid) {
		auto it = extrachild.find(nameid);
		if (it != extrachild.end()) return it->second;
		if (parent != nullptr) return parent->findExtraChild(nameid);
		return nullptr;
	}
	
	Object findPrimitiveChild(const StringId& nameid) {
		return meta->findExtraChild(nameid);
	}
	
	Object findChild(const StringId& nameid) {
		Object ret = findExtraChild(nameid);
		if (ret == nullptr && meta) ret = findPrimitiveChild(nameid);
		return ret;
	}

	Object findMetaChild(const StringId& nameid) {
		auto it = metachild.find(nameid);
		if (it != metachild.end()) return it->second;
		if (parent != nullptr) return parent->findMetaChild(nameid);
		return nullptr;
	}

	void* get() {
		return address;
	}

	string getMetaName() {
		if (meta != nullptr) return meta->getMetaName();

		string ret = "";
		switch (nameid) {
		case BasicMeta::MSet: {
			auto rec = parent;
			while (rec != nullptr) {
				for (auto it : rec->metachild) {
					if (it.second.get() == this) {
						auto str = StringSet::findString(it.first);
						if (!str.empty() && str[0] != '@') {
							return str;
						}
					}
				}
				rec = rec->parent;
			}
			int id = 0; bool flag = true;
			ret += "Set(";
			for (auto it : extrachild) {
				ret += StringSet::findString(it.first);
				ret += ':';
				ret += it.second->getMetaName();
				ret += ',';
				flag = false;
			}
			if (flag == false) ret[ret.size() - 1] = ')';
			else ret += ')';
			break;
		}
		case BasicMeta::MFunction: {
			auto rec = parent;
			if (rec != nullptr) {
				for (auto it : rec->metachild) {
					if (it.second.get() == this) {
						auto str = StringSet::findString(it.first);
						if (!str.empty() && str[0] != '@') {
							return str;
						}
					}
				}
				rec = rec->parent;
			}
			ret += "Function(";
			int argc = 0;
			bool flag = 0;
			while (1) {
				StringId arg = StringSet::addString("@arg" + std::to_string(argc++));
				auto it = metachild.find(arg);
				if (it == metachild.end()) break;
				if (flag) ret += ',';
				else flag = 1;
				ret += it->second->getMetaName();
			}
			ret += ")=>";
			StringId arg = StringSet::addString("@ret");
			auto it = metachild.find(arg);
			if (it != metachild.end()) ret += it->second->getMetaName();
			else ret += "()";
			break;
		}
		default: {
			ret += StringSet::findString(nameid);
			break;
		}
		}
		return std::move(ret);
	}

	void print(std::ostream& out) {
		if (meta == nullptr) out << "Meta: " << getMetaName();
		else {
			switch (meta->nameid) {
			case BasicMeta::MSet:
				out << "Set(";
				for (auto it = extrachild.begin(); it != extrachild.end(); ++it) {
					if (it != extrachild.begin()) out << ",";
					it->second->print(out);
				}
				out << ")";
				break;
			case BasicMeta::MInt: out << *((BasicInt*)(address)); break;
			case BasicMeta::MFloat: out << *((BasicFloat*)(address)); break;
			case BasicMeta::MChar: out << *((BasicChar*)(address)); break;
			case BasicMeta::MBool: out << ((*((BasicChar*)(address)) == 0) ? ("false") : ("true")); break;
			case BasicMeta::MFunction: {
				out << "<0x" << address << ">";
				break;
			}
			case BasicMeta::MNone: out << "None"; break;
			}
			out << " @" << getMetaName();
		}
	}
};

#define ConvertPtr(ptr, type) ((type*)(ptr))

namespace Content {

	static BasicInt toInt(Object node);

	static BasicChar toChar(Object node);

	static BasicFloat toFloat(Object node);

	static BasicBool toBool(Object node);

	static BasicFloat UppertoFloat(Object node);

	static BasicInt UppertoInt(Object node);
};

static BasicInt Content::toInt(Object node) {
	StringId nameid = node->meta->nameid;
	if (nameid == BasicMeta::MInt) {
		return *(ConvertPtr(node->get(), BasicInt));
	}
	else if (nameid == BasicMeta::MFloat) {
		return static_cast<BasicInt>(*(ConvertPtr(node->get(), BasicFloat)));
	}
	else {
		Result::output() << "Cannot convert to Int.\n";
		return 0;
	}
}

static BasicChar Content::toChar(Object node) {
	StringId nameid = node->meta->nameid;
	if (nameid == BasicMeta::MChar) {
		return *ConvertPtr(node->get(), BasicChar);
	}
	else {
		Result::output() << "Cannot convert to Char.\n";
		return 0;
	}
}

static BasicFloat Content::toFloat(Object node) {
	StringId nameid = node->meta->nameid;
	if (nameid == BasicMeta::MFloat) {
		return *ConvertPtr(node->get(), BasicFloat);
	}
	else if (nameid == BasicMeta::MInt) {
		return (BasicFloat)(*(ConvertPtr(node->get(), BasicInt)));
	}
	else {
		Result::output() << "Cannot convert to Float.\n";
		return 0;
	}
}

static BasicBool Content::toBool(Object node) {
	StringId nameid = node->meta->nameid;
	if (nameid == BasicMeta::MBool) {
		return *(ConvertPtr(node->get(), BasicChar));
	}
	else {
		Result::output() << "Cannot convert to Bool.\n";
		return 0;
	}
}


static BasicFloat Content::UppertoFloat(Object node) {
	if (node->meta->nameid == BasicMeta::MFloat) return EXTRACTADDRESS(node, BasicFloat);
	if (node->meta->nameid == BasicMeta::MInt) return EXTRACTADDRESS(node, BasicInt);
	if (node->meta->nameid == BasicMeta::MChar) return EXTRACTADDRESS(node, BasicChar);
	Result::output() << "Cannot convert to Float.\n";
	return 0;
}

static BasicInt Content::UppertoInt(Object node) {
	if (node->meta->nameid == BasicMeta::MFloat) return EXTRACTADDRESS(node, BasicFloat);
	if (node->meta->nameid == BasicMeta::MInt) return EXTRACTADDRESS(node, BasicInt);
	if (node->meta->nameid == BasicMeta::MChar) return EXTRACTADDRESS(node, BasicChar);
	Result::output() << "Cannot convert to Int.\n";
	return 0;
}

#endif // !__OBJECT_H_
