#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/variant/variant.hpp>

#include <exception>
#include <map>
#include <string>
#include <vector>

class LithpException : public std::exception {
	virtual const char* what() const throw() {
		return "Lithp Exception";
	}
};

typedef boost::multiprecision::cpp_int LithpInt;

enum LithpType {
	Integer,
	Float,
	String,
	List,
	Dict,
	OpChain,
	OpChainClosure
};

std::string GetLithpType(LithpType t);

// Forward declarations
class LithpObject;
typedef std::shared_ptr<LithpObject> LithpObject_p;
typedef std::vector<LithpObject_p> LithpList_t;
class LithpList;
typedef std::shared_ptr<LithpList> LithpList_p;
typedef std::map<std::string, LithpObject_p> LithpDict_t;
class LithpDict;
typedef std::shared_ptr<LithpDict> LithpDict_p;
class LithpOpChain;
class LithpClosure;
typedef std::shared_ptr<LithpOpChain> LithpOpChain_p;
typedef std::shared_ptr<LithpClosure> LithpClosure_p;

class LithpObject {
public:
	LithpObject(boost::any v, LithpType t) : value(v), type(t) {
	}
	LithpObject(const LithpObject &o) : value(o.GetValue()), type(o.GetType()) {
	}
	virtual ~LithpObject();

	virtual bool can_coerce(LithpType to) {
		return false;
	}
	virtual LithpObject *coerce(LithpType to) {
		throw LithpException();
	}
	LithpType GetType() const { return this->type; }
	boost::any GetValue() const { return this->value; }
	template<typename T> T GetValue() {
		return boost::any_cast<T>(this->value);
	}
	template<class C> C* GetClass() {
		return dynamic_cast<C*>(this);
	}

	virtual LithpInt *IntValue() { throw LithpException(); }
	virtual double *FloatValue() { throw LithpException(); }
	virtual LithpList_t *ListValue() { throw LithpException(); }
	virtual LithpDict_t *DictValue() { throw LithpException(); }
protected:
	const LithpType type;
	const boost::any value;
private:
};

class LithpInteger : public LithpObject {
public:
	LithpInteger(LithpInt v) : LithpObject(new LithpInt(v), Integer) {}
	LithpInteger(int v) : LithpInteger(LithpInt(v)) {}
	LithpInteger(long v) : LithpInteger(LithpInt(v)) {}
	LithpInteger(unsigned int v) : LithpInteger(LithpInt(v)) {}
	LithpInteger(unsigned long v) : LithpInteger(LithpInt(v)) {}
	~LithpInteger() { delete(this->IntValue()); }
	LithpInt *IntValue() { return this->GetValue<LithpInt*>(); }
	void Test();
	bool can_coerce(LithpType to) {
		switch (to) {
		case Float:
			return true;
		case String:
			return true;
		default:
			return false;
		}
	}
	LithpObject *coerce(LithpType to);
protected:
private:
};

class LithpFloat : public LithpObject {
public:
	LithpFloat(float v) : LithpObject(new double((double)v), Float) {}
	LithpFloat(double v) : LithpObject(new double(v), Float) {}
	LithpFloat(int v) : LithpObject(new double((double)v), Float) {}
	~LithpFloat() { delete(this->FloatValue()); }
	double* FloatValue() { return this->GetValue<double*>(); }
	bool can_coerce(LithpType to) {
		switch (to) {
		case Integer:
			return true;
		case String:
			return true;
		default:
			return false;
		}
	}
protected:
};

class LithpString : public LithpObject {
public:
	LithpString(std::string v) : LithpObject(new std::string(v), String) {}
	LithpString() : LithpObject(new std::string(""), String) {}
	~LithpString() { delete(this->StringValue()); }
	std::string* StringValue() { return this->GetValue<std::string*>(); }
protected:
private:
};

class LithpList : public LithpObject {
public:
	LithpList() : LithpObject(new LithpList_t(), List) { }
	~LithpList() {
		LithpList_t *l = this->ListValue();
		l->clear();
	}
	void push(LithpObject *v);
	LithpObject_p pop();
	LithpList_t* ListValue() { return this->GetValue<LithpList_t*>(); }
protected:
	LithpList(LithpType type) : LithpObject(new LithpList_t(), type) { }
	LithpList(LithpList_t v, LithpType type) : LithpObject(new LithpList_t(v), type) { }
private:
};

class LithpDict : public LithpObject {
public:
	LithpDict() : LithpObject(new LithpDict_t(), Dict) { 
	}
	~LithpDict() { delete(this->DictValue()); }
	LithpDict_t* DictValue() { return this->GetValue<LithpDict_t*>(); }
	LithpObject* Get(std::string name);
protected:
	LithpDict(LithpType type) : LithpObject(new LithpDict_t(), type) { }
	LithpDict(LithpDict_t v, LithpType type) : LithpObject(new LithpDict_t(v), type) { }
private:
};

class LithpClosure : public LithpDict {
public:
	LithpClosure(LithpOpChain* _owner) : LithpDict(OpChainClosure),
		owner(_owner), topmost(LithpClosure_p(this))
	{
	}
	LithpClosure(LithpOpChain* _owner, LithpClosure_p _parent) : LithpDict(OpChainClosure),
		parent(_parent), topmost(_parent.get()->parent),
		owner(_owner)
	{
	}
	LithpClosure_p parent = 0;
	LithpClosure_p topmost = 0;
	LithpOpChain_p owner = 0;
	LithpOpChain_p getOwner() { return this->owner; }
	LithpOpChain_p getTopOwner() {
		if (this->parent != 0) {
			LithpDict_t* d = this->DictValue();

		}
	}
protected:
private:
};

class LithpOpChain : protected LithpList {
public:
	LithpOpChain() : LithpList(OpChain) {
		this->closure = LithpClosure_p(new LithpClosure(this));
	}
	LithpOpChain(LithpOpChain_p parent, LithpList_t ops) : LithpList(ops, OpChain), parent(parent) {
		this->closure = LithpClosure_p(new LithpClosure(this, parent.get()->closure));
	}
	void rewind() { this->pos = -1; }
	LithpObject* next();
	LithpObject* get();
	void add(LithpObject* op);

protected:
	int pos = -1;
	LithpOpChain_p parent = 0;
	LithpClosure_p closure = 0;
private:
};