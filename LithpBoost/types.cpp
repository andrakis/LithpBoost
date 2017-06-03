#include "include/types.hpp"

#include <iostream>

namespace lithp {

LithpObject *lithp_int(LithpInt i) {
	return new LithpInteger(i);
}

LithpObject *lithp_flt(LithpFlt f) {
	return new LithpFloat(f);
}

LithpObject *lithp_str(std::string s) {
	return new LithpString(s);
}

LithpObject::~LithpObject() {
	std::cout << "~LithpObject(" << GetLithpType(this->type) << ")" << std::endl;
}

std::string GetLithpType(LithpType t) {
	switch (t) {
		case Integer: return std::string("Integer");
		case Float: return std::string("Float");
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
	LithpInt v = this->IntValue();
	switch (to) {
		case Float:
			return new LithpFloat((double)v);
		case String:
			// Brackets required, orelse compiler error C2361 thrown.
			// C++, you confuse me.
		{
			std::string s;
			s = "TODO";
			return dynamic_cast<LithpObject*>(new LithpString(s));
		}
		default:
			throw LithpException();
	}
}

std::string LithpInteger::_str() {
	return "INT";
}

std::string LithpFloat::_str() {
	return "FLOAT";
}

std::string LithpString::_str() {
	std::string r = "\"";
	r += this->StringValue();
	r += "\"";
	return r;
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

std::string LithpList::_str() {
	return "LIST";
}

std::string LithpDict::_str() {
	return "DICT";
}

// Atoms
static std::map<std::string,LithpAtom_p> atoms_by_name;
static std::map<int, LithpAtom_p> atoms_by_id;
static int atom_id_counter = 0;
LithpAtom_p GetAtom(int id) {
	return atoms_by_id[id];
}
LithpAtom_p GetAtom(std::string name) {
	std::map<std::string, LithpAtom_p>::iterator it = atoms_by_name.find(name);
	if (it == atoms_by_name.end()) {
		int id = atom_id_counter++;
		LithpAtom *a = new LithpAtom(name, id);
		LithpAtom_p p = LithpAtom_p(a);
		atoms_by_name.emplace(name, p);
		atoms_by_id.emplace(id, p);
		return p;
	}
	return it->second;
}

// OpChains
LithpObject *LithpOpChain::next() {
	int id = ++this->pos;
	if ((unsigned)id >= this->length())
		return nullptr;
	LithpObject *p = this->at(++this->pos).get();
	this->current = p;
	return p;
}

LithpObject *LithpOpChain::get() {
	return this->current;
}

void LithpOpChain::add(LithpObject *op) {
	LithpObject_p p(op);

}
}
