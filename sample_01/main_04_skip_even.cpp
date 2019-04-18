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
		Statement{
			new While("CPLoop", {
				new MoreEq(N, i2, t1)
			}, {
				// `i` が4以上の偶数ならcontinue
				Statement {
					new If("CPSkip1", {
							i >> 2
					}, {
						i.load(),
						Statement{
							new Const(
								"CIR\n"
								"CME\n"
								"SZE\n"
								"BUN CPCont\n"
							)
						}
					})
				},

				Nc2 = +N,
				Nc1 = N + mi,
				// アンダーフローしない限り `i` を引く事を繰り返す
				Statement{
					new While("CPLoop2", {
						new MoreEq(Nc1, Nc2, t1)
					}, {
						Nc2 = +Nc1,
						Nc1 = Nc1 + mi
					}, true)
				},
				// 余りが0なら返却
				Statement{
					new If("CPBreak", {
						Nc2.load()
					},{
						new Goto("CPEnd")
					}, true)
				},

				Statement{ new Const("CPCont,") },
				// インクリメント
				i2 = (i<<1) + i2,
				++i2,
				++i,
				--mi
			})
		},
		halt,
		Statement{ new Const("CPEnd,") }
	};

	// Nを減らしながら `checkPrine` 関数を呼ぶ
	StatementList program = {
		begin,

		Statement{
			new While("MainLoop", {
				N.load()
			}, {
				checkPrime.stat("CP"),
				Statement{ new Const("L1,") },
				--N
			})
		},
		halt,
		Statement{
			new Const("\n\n")
		},

		Data::stat_all(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
