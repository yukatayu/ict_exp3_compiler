#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data A(INT, "A", 55); // 割られる数
	Data B(INT, "B", 17); // 割る数
	Data Q(INT, "Q", -1, MaybeUnused); // 商
	Data R(INT, "R", 0, MaybeUnused);  // 余り
	Data t1(INT, "T1", 0);

	StatementList program{
		begin,

		B = -B,  // Bの符号を反転
		// AがBより大きい限り、引き続ける
		While("Main", { MoreEq(A, B, t1) }, {
			R = +A,
			++Q,
			A = A + B
		}, true),
		halt,

		Data::stat_all(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
