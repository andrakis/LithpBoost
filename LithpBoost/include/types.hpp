#pragma once

#include <exception>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace lithp {

class LithpException : public std::exception {
	virtual const char* what() const throw() {
		return "Lithp Exception";
	}
};

typedef long long LithpInt;
typedef double LithpFlt;

enum LithpType {
	Integer,
	Float,
	String,
	List,
	Dict,
	OpChain,
	OpChainClosure,
	Atom
};

std::string GetLithpType(LithpType t);

// Forward declarations
class LithpObject;
	typedef std::shared_ptr<LithpObject> LithpObject_p;
class LithpInteger;
class LithpFloat;
class LithpString;
class LithpList;
	typedef std::shared_ptr<LithpList> LithpList_p;
	typedef std::vector<LithpObject_p> LithpList_t;
class LithpDict;
	typedef std::shared_ptr<LithpDict> LithpDict_p;
	typedef std::map<std::string, LithpObject_p> LithpDict_t;
class LithpOpChain;
	typedef std::shared_ptr<LithpOpChain> LithpOpChain_p;
class LithpClosure;
	typedef std::shared_ptr<LithpClosure> LithpClosure_p;

/// The base Lithp object.
class LithpObject {
public:
	LithpObject(void* v, LithpType t) : type(t), value(v) {
	}
	LithpObject(const LithpObject &o) : type(o.GetType()), value(o.value) {
	}
	virtual ~LithpObject();

	virtual bool can_coerce(LithpType to) = 0;
	virtual LithpObject *coerce(LithpType to) = 0;
	LithpType GetType() const { return this->type; }
	template<typename T> T GetValue() const {
		return (T)this->value;
	}
	template<class C> C* GetClass() const {
		return dynamic_cast<C*>(this);
	}

	virtual LithpInt IntValue() const = 0;
	virtual LithpFlt FloatValue() const = 0;
	virtual std::string StringValue() { return this->str(); }
	virtual LithpList_t *ListValue() const = 0;
	virtual LithpDict_t *DictValue() const = 0;

	std::string str() {
		return this->_str();
	}
protected:
	virtual std::string _str() = 0;
	const LithpType type;
	const void* value;
	virtual LithpObject *op_add(LithpObject *a, LithpObject *b) = 0;
	virtual LithpObject *op_sub(LithpObject *a, LithpObject *b) = 0;
	virtual LithpObject *op_mul(LithpObject *a, LithpObject *b) = 0;
	virtual LithpObject *op_div(LithpObject *a, LithpObject *b) = 0;
private:
};

LithpObject *lithp_int(LithpInt i);
LithpObject *lithp_flt(LithpFlt f);
LithpObject *lithp_str(std::string s);

///
/// An abstract Value class.
/// This supports math operators, but not container operators.
class LithpValue : public LithpObject {
public:
	LithpValue(void* v, LithpType t) : LithpObject(v, t) {
	}
	LithpValue(const LithpValue &o) : LithpObject(o) {
	}
	LithpList_t *ListValue() const { throw LithpException(); }
	LithpDict_t *DictValue() const { throw LithpException(); }
	bool can_coerce(LithpType to) {
		if (to == this->GetType())
			return true;
		switch (to) {
			case Float:
			case String:
				return true;
			default:
				return false;
		}
	}
	LithpObject *coerce(LithpType to) {
		// TODO
		throw LithpException();
	}
protected:
	LithpType optimal_coerce(LithpType a, LithpType b) {
		if (a == b)
			return a;
		if (a == String && b != String)
			return String;
		if (a == Float || b == Float)
			return Float;
		return a;
	}
	LithpObject *op_add(LithpObject *a, LithpObject *b) {
		LithpType optimal = this->optimal_coerce(a->GetType(), b->GetType());
		if (!a->can_coerce(optimal))
			throw LithpException();
		if (!b->can_coerce(optimal))
			throw LithpException();
		LithpObject *ca = a->coerce(optimal);
		LithpObject *cb = b->coerce(optimal);
		switch (optimal) {
			case String:
			{
				std::string result = ca->str();
				result += cb->str();
				return lithp_str(result);
			}
			case Integer:
			{
				LithpInt result = ca->IntValue();
				result += cb->IntValue();
				return lithp_int(result);
			}
			case Float:
			{
				LithpFlt result = ca->FloatValue();
				result += cb->FloatValue();
				return lithp_flt(result);
			}
			default:
				throw LithpException();
		}
	}
	LithpObject *op_sub(LithpObject *a, LithpObject *b) { throw LithpException(); }
	LithpObject *op_mul(LithpObject *a, LithpObject *b) { throw LithpException(); }
	LithpObject *op_div(LithpObject *a, LithpObject *b) { throw LithpException(); }
};

///
/// An abstract Container value.
/// This supports container operations, but not math operations.
class LithpContainer : public LithpObject {
public:
	LithpContainer(void* v, LithpType t) : LithpObject(v, t) {
	}
	LithpContainer(const LithpContainer &o) : LithpObject(o) {
	}
	bool can_coerce(LithpType to) {
		if (to == this->GetType())
			return true;
		return false;
	}
	LithpObject *coerce(LithpType to) {
		// TODO
		throw LithpException();
	}
	LithpInt IntValue() const { throw LithpException(); }
	LithpFlt FloatValue() const { throw LithpException(); }
	std::string StringValue() {
		return this->_str();
	}
protected:
	LithpObject *op_add(LithpObject *a, LithpObject *b) { throw LithpException(); }
	LithpObject *op_sub(LithpObject *a, LithpObject *b) { throw LithpException(); }
	LithpObject *op_mul(LithpObject *a, LithpObject *b) { throw LithpException(); }
	LithpObject *op_div(LithpObject *a, LithpObject *b) { throw LithpException(); }
};

class LithpInteger : public LithpValue {
public:
	LithpInteger(LithpInt v) : LithpValue(new LithpInt(v), Integer) {}
	LithpInteger(int v) : LithpInteger(LithpInt(v)) {}
	LithpInteger(long v) : LithpInteger(LithpInt(v)) {}
	LithpInteger(unsigned int v) : LithpInteger(LithpInt(v)) {}
	LithpInteger(unsigned long v) : LithpInteger(LithpInt(v)) {}
	~LithpInteger() { delete(this->value); }
	LithpInt IntValue() const { return *this->GetValue<LithpInt*>(); }
	LithpFlt FloatValue() const { throw LithpException(); }
	void Test();
	LithpObject *coerce(LithpType to);
protected:
	LithpInteger(LithpInt v, LithpType t) : LithpValue(new LithpInt(v), t) {}
	std::string _str();
private:
};

typedef std::shared_ptr<LithpInteger> LithpInteger_p;

class LithpAtom : public LithpInteger {
public:
	LithpAtom(std::string name, LithpInt id) : LithpInteger(id, Atom), name(name) {}
	std::string getName() const { return this->name; }
protected:
	const std::string name;
};

typedef std::shared_ptr<LithpAtom> LithpAtom_p;

LithpAtom_p GetAtom(int id);
LithpAtom_p GetAtom(std::string name);

class LithpFloat : public LithpValue {
public:
	LithpFloat(float v) : LithpValue(new LithpFlt((LithpFlt)v), Float) {}
	LithpFloat(LithpFlt v) : LithpValue(new LithpFlt(v), Float) {}
	LithpFloat(int v) : LithpValue(new LithpFlt((LithpFlt)v), Float) {}
	~LithpFloat() { delete(this->value); }
	LithpInt IntValue() const { throw LithpException(); }
	LithpFlt FloatValue() const { return *this->GetValue<LithpFlt*>(); }
protected:
	std::string _str();
};

class LithpString : public LithpValue {
public:
	LithpString(std::string v) : LithpValue(new std::string(v), String) {}
	LithpString() : LithpValue(new std::string(""), String) {}
	~LithpString() { delete(this->value); }
	LithpInt IntValue() const { /* TODO: Implement int parsing */ throw LithpException(); }
	LithpFlt FloatValue() const { /* TODO: Implement float parsing */ throw LithpException(); }
	std::string StringValue() const { return *this->GetValue<std::string*>(); }
protected:
	std::string _str();
private:
};

class LithpList : public LithpContainer {
public:
	LithpList() : LithpContainer(new LithpList_t(), List) { }
	~LithpList() {
		LithpList_t *list = this->ListValue();
		list->clear();
		delete list;
	}
	void push(LithpObject *v);
	LithpObject_p pop();
	LithpList_t* ListValue() const { return this->GetValue<LithpList_t*>(); }
	size_t length() const {
		return this->ListValue()->size();
	}
	LithpObject_p at(int index) {
		return this->ListValue()->at(index);
	}
	LithpDict_t *DictValue() const { throw LithpException(); }
protected:
	LithpList(LithpType type) : LithpContainer(new LithpList_t(), type) { }
	LithpList(LithpList_t v, LithpType type) : LithpContainer(new LithpList_t(v), type) { }
	std::string _str();
private:
};

class LithpDict : public LithpContainer {
public:
	LithpDict() : LithpContainer(new LithpDict_t(), Dict) {
	}
	~LithpDict() {
		LithpDict_t *dict = this->DictValue();
		dict->clear();
		delete dict;
	}
	LithpList_t* ListValue() const { throw LithpException(); }
	LithpDict_t* DictValue() const { return this->GetValue<LithpDict_t*>(); }
	LithpObject* Get(std::string name);
protected:
	LithpDict(LithpType type) : LithpContainer(new LithpDict_t(), type) { }
	LithpDict(LithpDict_t v, LithpType type) : LithpContainer(new LithpDict_t(v), type) { }
	std::string _str();
private:
};

class LithpClosure : public LithpDict {
public:
	LithpClosure(LithpOpChain* _owner) : LithpDict(OpChainClosure),
		parent(LithpClosure_p(this)), topmost(LithpClosure_p(this)), owner(_owner)
	{ }
	LithpClosure(LithpOpChain* _owner, LithpClosure_p _parent) : LithpDict(OpChainClosure),
		parent(_parent), topmost(_parent.get()->parent), owner(_owner)
	{ }

	LithpClosure_p parent;
	LithpClosure* getParent() { return this->parent.get(); }
	LithpClosure_p topmost;
	LithpClosure* getTopmost() { return this->topmost.get(); }
	LithpOpChain_p owner;
	LithpOpChain* getOwner() { return this->owner.get(); }

	// Implement these functions inline for speed.
	// These functions, especially get, are some of the most costly
	// and often-called functions.


	LithpClosure* getTopOwner() {
		if (this->parent != 0) {
			return this->getParent()->getTopOwner();
		}
		return this;
	}
	bool has_key(std::string key) {
		LithpDict_t *dict = this->DictValue();
		LithpDict_t::iterator it = dict->find(key);
		if (it == dict->end())
			return false;
		return true;
	}
	bool do_set(std::string key, LithpObject_p item) {
		if (this->has_key(key)) {
			this->set_immediate(key, item);
			return true;
		} else if (this->parent != 0) {
			if (this->getParent()->do_set(key, item))
				return true;
		}
		this->set_immediate(key, item);
		return true;
	}
	void set(std::string key, LithpObject_p item) {
		if (this->try_set(key, item))
			return;
		if (this->parent != 0) {
			if (this->getParent()->try_set(key, item))
				return;
		}
		this->set_immediate(key, item);
	}
	void set_immediate(std::string key, LithpObject_p item) {
		LithpDict_t *dict = this->DictValue();
		dict->emplace(key, item);
	}
	bool try_set(std::string key, LithpObject_p item) {
		if (this->has_key(key)) {
			this->set_immediate(key, item);
			return true;
		} else if (this->parent != 0) {
			if (this->getParent()->try_set(key, item))
				return true;
		}
		return false;
	}
	LithpObject_p get(std::string key) {
		LithpDict_t *dict = this->DictValue();
		LithpDict_t::iterator it = dict->find(key);
		if (it != dict->end())
			return it->second;
		if (this->parent != 0)
			return this->getParent()->get(key);
		throw LithpException();
	}
	int length() {
		return this->ListValue()->size();
	}
protected:
private:
};

class LithpOpChain : protected LithpList {
public:
	LithpOpChain() : LithpList(OpChain),
		closure(LithpClosure_p(new LithpClosure(this))) {
	}
	LithpOpChain(LithpOpChain_p parent, LithpList_t ops) :
		LithpList(ops, OpChain), parent(parent),
		closure(LithpClosure_p(new LithpClosure(this, parent.get()->closure))) {
	}
	void rewind() { this->pos = -1; }
	LithpObject* next();
	LithpObject* get();
	void add(LithpObject* op);

	LithpOpChain* getParent() const { return this->parent.get(); }
	LithpClosure* getClosure() const { return this->closure.get(); }

protected:
	int pos = -1;
	LithpOpChain_p parent;
	const LithpClosure_p closure;
	LithpObject *current = nullptr;
private:
};
}
