#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data N(INT, "N", 65535);

	Data i_init(INT, "IINIT", 2);
	Data i2_init(INT, "I2INIT", 4);

	// Note: Always i < 256
	Data i(INT, "I", 0);
	Data n_tmp(INT, "ITMP", 0);
	Data mi(INT, "MI", 0);
	// Note: i^2 < 65536
	Data i2(INT, "I2", 0);

	Data i_shift(INT, "ISH", 0);

	// Temporary
	Data t1(INT, "TEMP1", 0);

	// `N` が素数なら終了, さもなくば返却するサブルーチン
	StatementList checkPrime = {
		i = +i_init,
		mi = -i_init,
		i2 = +i2_init,
		Statement{
			new While("CPLoop", {
				new MoreEq(N, i2, t1)
			}, {
				i_shift = +i,
				n_tmp = +N,
				// `i` が4以上の偶数ならcontinue
				Statement {
					new If("CPSkip1", {
							i >> 2
					}, {
						i.load(),
						Statement{ new Const{
							"CIR\n"
							"CME\n"
							"SZE\n"
							"BUN CPCont\n"
						} }
					})
				},
				// 最上位ビットが立つまで値を左シフトする
				Statement{
					new While("CPPre", {
						new Negative(i_shift)
					}, {
						i_shift = (i_shift<<1),
					}, true)
				},
				// 割り算の筆算と同様に値を引いていく
				Statement{
					new While("CPMain", {
						new MoreEq(i_shift, i, t1)
					}, {
						Statement{
							new If("DivCmp", {
								new MoreEq(n_tmp, i_shift, t1)
							}, {
								n_tmp = -i_shift +  n_tmp,
							})
						},
						i_shift = (i_shift >> 1),
					})
				},
				// 余りが0なら返却
				Statement{
					new If("CPBreak", {
						n_tmp.load()
					},{
						new Goto("L1")
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

	// メインループ,  `N` を減らしながら検査していく
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
		end,
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
