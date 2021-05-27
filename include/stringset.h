#pragma once

#ifndef __STRINGSET_H_
#define __STRINGSET_H_

#include "util.h"
#include <map>
#include <vector>

namespace StringSet {

	static std::map<string, StringId> stringset;
	static std::vector<string> idtostring;

	static StringId addString(const string& str) {
		auto strit = stringset.find(str);
		StringId strid;
		if (strit != stringset.end()) strid = strit->second;
		else {
			strid = idtostring.size();
			idtostring.push_back(str);
			stringset.emplace(str, strid);
		}
		return strid;
	}

	static StringId findId(const string& str) {
		auto strit = stringset.find(str);
		if (strit != stringset.end()) return strit->second;
		return 0;
	}

	static string findString(const StringId& id) {
		if (id > idtostring.size()) return "";
		return idtostring[id];
	}

}

#endif // !__STRINGSET_H_
