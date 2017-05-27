#include <iostream>

#include "include/types.hpp"

using namespace lithp;

int main(void) {
	LithpList l;

	l.push(new LithpInteger(123));
	l.push(new LithpFloat(123));

	for (int x = 0; x < 2; x++) {
		LithpObject_p v = l.pop();
		LithpObject *o = v.get();
		std::cout << "Type: " << GetLithpType(o->GetType()) << ", value: " << o->str() << std::endl;
	}

	//std::string r;
	//std::cin >> r;

	return 0;
}