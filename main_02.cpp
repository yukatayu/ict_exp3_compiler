#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;

	Data t(INT, "t", 10);
	Data X(INT, "X", 55);
	Data Y(INT, "Y", 17);
	Data Q(INT, "Q", -1);
	Data R(INT, "R", 0);

	StatementList program{
		Statement{ new Const("ORG 10 / Entry Point") },
		Y = -Y,
		Statement{
			new While("Main", {
				new Negative(X)
			}, {
				R = +X,
				++Q,
				X = X + Y
			}, true)
		},
		Statement{ new Const("HLT") },
		X.stat(),
		Y.stat(),
		Q.stat(),
		R.stat(),
		Statement{ new Const("END") }
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
