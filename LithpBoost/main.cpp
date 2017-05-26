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
		switch (o->GetType()) {
			case Integer:
			{
				LithpInteger *i = o->GetClass<LithpInteger>();
				i->Test();
				std::cout << "Int: " << *i->IntValue() << std::endl;
				break;
			}
			case Float:
			{
				LithpFloat *f = o->GetClass<LithpFloat>();
				std::cout << "Float: " << *f->FloatValue() << std::endl;
				break;
			}
		}
	}

	//std::string r;
	//std::cin >> r;

	return 0;
}