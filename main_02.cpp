#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data t1(INT, "T1", 0);
	Data t2(INT, "T2", 0);
	Data X(INT, "X", 55);
	Data Y(INT, "Y", 17);
	Data Q(INT, "Q", -1, MaybeUnused);
	Data R(INT, "R", 0, MaybeUnused);

	StatementList program{
		begin,
		Y = -Y,
		Statement{
			new While("Main", {
				new MoreEq(X, Y, t1, t2)
			}, {
				R = +X,
				++Q,
				X = X + Y
			}, true)
		},
		halt,

		X.stat(),
		Y.stat(),
		Q.stat(),
		R.stat(),
		t1.stat(),
		t2.stat(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
