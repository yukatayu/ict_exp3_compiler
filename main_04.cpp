#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data N(INT, "N", 65535);
	Data Nc1(INT, "NC1", 0);
	Data Nc2(INT, "NC2", 0);

	Data i_init(INT, "IINIT", 2);
	Data i2_init(INT, "I2INIT", 4);

	// Note: Always i < 256
	Data i(INT, "I", 0);
	Data mi(INT, "MI", 0);
	// Note: i^2 < 65536
	Data i2(INT, "I2", 0);

	// Temporary
	Data t1(INT, "CMPTMP1", 0);

	// Nが素数なら終了、合成数ならリターンする
	StatementList checkPrime = {
		i = +i_init,
		mi = -i_init,
		i2 = +i2_init,
		While("CPLoop", { MoreEq(N, i2, t1) }, {
			Nc2 = +N,
			Nc1 = N + mi,
			// アンダーフローしない限り `i` を引く事を繰り返す
			While("CPLoop2", { MoreEq(Nc1, Nc2, t1) }, {
				Nc2 = +Nc1,
				Nc1 = Nc1 + mi
			}, true),
			// 余りが0なら返却
			If("CPBreak", { +Nc2 },{
				Goto("CPEnd")
			}, true),
			// インクリメント
			i2 = (i<<1) + i2,
			++i2,
			++i,
			--mi
		}),
		halt,
		Const("CPEnd,")
	};

	// Nを減らしながら `checkPrine` 関数を呼ぶ
	StatementList program = {
		begin,

		While("MainLoop", { +N }, {
			checkPrime.stat("CP"),
			Const("L1,"),
			--N
		}),
		halt,
		Const("\n\n"),

		Data::stat_all(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
