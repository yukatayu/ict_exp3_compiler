#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data AccBak(INT, "ACCBAK", 0);
	Data EBak(INT, "EBAK", 0);
	Data mask_init(INT, "MASKINIT", 15);
	Data out_flag(INT, "OutFlag", 0);

	Data IN_tmp(INT, "INTmp", 0);
	Data N(INT, "N", 0);

	Data ascii_0(INT, "ASCII0", 48);
	Data ascii_ent(INT, "ASCIIENTER", 10);
	Data dec(INT, "DECVAL", 10);
	Data decp1(INT, "DECVALP1", 11);

	Data CPRet(INT, "CPRET", 0);

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
	Data t2(INT, "TEMP2", 0);

	Data Q(INT, "QVAL", 0);
	Data R(INT, "RVAL", 0);
	Data X(INT, "XVAL", 0);

	// `N` が素数なら終了, さもなくば返却するサブルーチン
	StatementList checkPrime = {
		i = +i_init,
		mi = -i_init,
		i2 = +i2_init,
		CPRet = Zero,
		While("CPLoop", { MoreEq(N, i2, t1) }, {
			i_shift = +i,
			n_tmp = +N,
			// `i` が4以上の偶数ならcontinue
			If("CPSkip1", { i >> 2 }, {
				+i,
				Const(
					"CIR\n"
					"CME\n"
					"SZE\n"
					"BUN CPCont\n"
				)
			}),
			// 最上位ビットが立つまで値を左シフトする
			While("CPPre", { Negative(i_shift) }, {
				i_shift = (i_shift<<1),
			}, true),
			// 割り算の筆算と同様に値を引いていく
			While("CPMain", { MoreEq(i_shift, i, t1) }, {
				If("DivCmp", { MoreEq(n_tmp, i_shift, t1) }, {
					n_tmp = -i_shift +  n_tmp,
				}),
				i_shift = (i_shift >> 1)
			}),
			// 余りが0なら返却
			If("CPBreak", { +n_tmp },{
				Goto("CPEnd")
			}, true),

			Const("CPCont,"),
			// インクリメント
			i2 = (i<<1) + i2,
			++i2,
			++i,
			--mi
		}),
		CPRet = One,
		Const("CPEnd,")
	};

	StatementList checkChar = {
		If("CheckCharEnt", { -ascii_ent + IN_tmp }, {
			out_flag = One,
			While("PrimeSearchLoop", { +N }, {
				checkPrime.stat("CP"),
				If("BreakCPRet", { +CPRet }, {
					Break("PrimeSearchLoop")
				}),
				--N
			}),
		}, true),
		t1 = -ascii_0 + IN_tmp,
		//If("CheckCharCtrlD", {}, {}),
		If("CheckCharNum", { MoreEq(dec, t1, t2) }, {
			If("CheckCharNumPosi", { Negative(t1) }, {
				N = N << 1,
				N = (N<<2) + N + t1
				//N = +IN_tmp
			}, true)
		})
	};

	StatementList interruptMain = {
		// Save Acc, E
		AccBak = Const(),
		EBak = GetE,

		// Reset Flag
		out_flag = Zero,

		// Get Input
		"SKI"_asm,
		"BUN InterruptReturn"_asm,
		IN_tmp = Const("INP"),

		checkChar.stat(),

		// Output
		If("OutputFlag", { +out_flag }, {
			"SKO"_asm,
			"BUN InterruptReturn"_asm,

			// divide
			If("ShowN_NEq0", { +N }, {
				Goto("InterruptReturn")
			}, true),
			While("ShowMain", { +N }, {
				i = Zero,
				Q = +N,
				While("ShowSubDigit", { +Q }, {
					R = t2 = +Q,
					X = -dec + Q,
					Q = Zero,
					While("ShowSub", { MoreEq(X, R, t1) }, {
						R = +X,
						X = -dec + X,
						++Q
					}, true),
					++i,
				}),
				i_shift = One,
				While("DispIShift", { --i }, {
					i_shift = i_shift << 1,
					i_shift = (i_shift<<2) + i_shift,
				}),
				i = +t2,
				While("DispIShiftSub", { +i }, {
					N = -i_shift + N,
					--i
				}),
				// 余りに '0' を足したものを先頭に挿入
				t2 + ascii_0,
				"OUT"_asm,
			}),
		}),

		"InterruptReturn,"_asm,
		// Load Acc, E
		EBak >> 1,
		+AccBak,
		"ION"_asm,
		Return("INT_RET")
	};

	// メインループ,  `N` を減らしながら検査していく
	StatementList program = {
		begin_interrupt("INT_MAIN", "INT_RET"),
		begin,

		+mask_init,
		"IMK"_asm,
		"SIO"_asm,
		"ION"_asm,	// enable interrupt

		While("MainLoop", { One }, {
		}),
		//While("MainLoop", { +N }, {
		//	checkPrime.stat("CP"),
		//	Const("L1,"),
		//	--N
		//}),
		halt,
		interruptMain.stat("INT_MAIN"),

		Data::stat_all(),
		end,
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
