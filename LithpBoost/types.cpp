#include "include/types.hpp"

#include <iostream>

LithpObject::~LithpObject() {
	std::cout << "~LithpObject(" << GetLithpType(this->type) << ")" << std::endl;
}

std::string GetLithpType(LithpType t) {
	switch (t) {
	case Integer: return std::string("Integer");
	case String: return std::string("String");
	case List: return std::string("List");
	case Dict: return std::string("Dict");
	default: return std::string("Unknown");
	}
}

void LithpInteger::Test() {
	std::cout << "LithpInteger::Test()" << std::endl;
}

LithpObject* LithpInteger::coerce(LithpType to) {
	LithpInt* v = this->IntValue();
	switch (to) {
	case Float:
		return new LithpFloat((double)(*v));
	case String:
		// Brackets required, orelse compiler error C2361 thrown.
		// C++, you confuse me.
		{
			std::string s;
			s.append(v->str());
			return dynamic_cast<LithpObject*>(new LithpString(s));
		}
	default:
		throw LithpException();
	}
}

void LithpList::push(LithpObject *v) {
	LithpObject_p p(v);
	LithpList_t* l = this->ListValue();
	l->push_back(p);
}
LithpObject_p LithpList::pop() {
	LithpList_t* l = this->GetValue<LithpList_t*>();
	LithpObject_p p = l->back();
	l->pop_back();
	return p;
}

