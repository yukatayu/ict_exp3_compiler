#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data t1(INT, "T1", 0);
	Data t2(INT, "T2", 0);
	Data A(INT, "A", 55);
	Data B(INT, "B", 17);
	Data Q(INT, "Q", -1, MaybeUnused);
	Data R(INT, "R", 0, MaybeUnused);

	StatementList program{
		begin,

		B = -B,
		Statement{
			new While("Main", {
				new MoreEq(A, B, t1)
			}, {
				R = +A,
				++Q,
				A = A + B
			}, true)
		},
		halt,

		Data::stat_all(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
