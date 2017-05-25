#include <iostream>

#include "include/types.hpp"

int main(void) {
	LithpList l;

	l.push(new LithpInteger(123));
	LithpObject_p v = l.pop();

	LithpObject *o = v.get();
	LithpInteger *i = dynamic_cast<LithpInteger*>(o);
	i->Test();

	//std::string r;
	//std::cin >> r;

	return 0;
}