#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data t(INT, "t", 0);
	Data X(INT, "X", 55);
	Data Y(INT, "Y", 17);
	Data Q(INT, "Q", -1);
	Data R(INT, "R", 0);
	Data cmp_res(INT, "CMPRES", 10);

	// X >= Y
	StatementList cmpXY = {
		cmp_res = One,
		Statement{ new If("Cmp__TOKEN__", {
			Y.load(),
		}, {
			t = -Y,
			Statement{ new Const("CLE\n") },
			t + X,
			cmp_res = GetE,
		})}
	};

	StatementList program{
		begin,
		Y = -Y,
		Statement{
			new While("Main", {
				cmpXY.stat("CMP"),
				cmp_res.load()
				//new Negative(X)
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
		t.stat(),
		cmp_res.stat(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
