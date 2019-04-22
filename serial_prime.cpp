#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data AccBak(INT, "ACCBAK", 0);
	Data EBak(INT, "EBAK", 0);
	Data mask_init(INT, "MASKINIT", 15);
	Data show_i(INT, "ShowIParam", 0);

	Data out_trigger(INT, "OutFlag", 0);
	Data halt_trigger(INT, "HaltFlag", 0);

	Data IN_tmp(INT, "INTmp", 0);
	Data N(INT, "N", 0);
	Data N_bak(INT, "NBAK", 0);

	Data ascii_0(INT, "ASCII0", 48);
	Data ascii_ent(INT, "ASCIIENTER", 10);
	Data ascii_ctrl_D(INT, "ASCIICTRLD", 4);
	Data dec(INT, "DECVAL", 10);

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

	Data decr_trigger(INT, "DECLTRIG", 0);
	Data sep_trigger(INT, "SEPTRIG", 0);
	Data min_primep1(INT, "MINIMUMPRIMEP1", 3);

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
				"CIR"_asm,
				"CME"_asm,
				"SZE"_asm,
				"BUN CPCont"_asm
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

			"CPCont,"_asm,
			// インクリメント
			i2 = (i<<1) + i2,
			++i2,
			++i,
			--mi
		}),
		CPRet = One,
		"CPEnd,"_asm
	};

	StatementList checkChar = {
		If("CheckCharEnt", { -ascii_ent + IN_tmp }, {
			decr_trigger = One,
		}, true),
		t1 = -ascii_0 + IN_tmp,
		If("CheckCharCtrlD", { -ascii_ctrl_D + IN_tmp }, {
			halt_trigger = One,
		}, true),
		If("CheckCharNum", { MoreEq(dec, t1, t2) }, {
			If("CheckCharNumPosi", { Negative(t1) }, {
				N = N << 1,
				N = (N<<2) + N + t1
			}, true)
		})
	};

	StatementList prepareOutput = {
		// Decrement to Prime Number
		If("PrimeDeclTrigger", { +decr_trigger }, {
			decr_trigger = Zero,
			While("PrimeSearchLoop", { +N }, {
				checkPrime.stat("CP"),
				If("BreakCPRet", { +CPRet }, {
					Break("PrimeSearchLoop")
				}),
				--N
			}),
			N_bak = +N,
			out_trigger = One,
		}),

		// Prepare for Output (counting 10^i)
		If("GetDigitOutT", { +out_trigger }, {
			out_trigger = Zero,
			show_i = Zero,
			Q = +N,
			While("ShowSubDigit", { +Q }, {
				R = +Q,
				X = -dec + Q,
				Q = Zero,
				While("ShowSubSearch", { MoreEq(X, R, t1) }, {
					R = +X,
					X = -dec + X,
					++Q
				}, true),
				++show_i,
			}),
		})
	};

	// Zero が返ったら、何も表示しない。
	StatementList getDigitOne = {
		i_shift = One,
		i = +show_i,
		Q = +N,
		If("ReadyState", { +i }, {
			If("AnythingToShow", { +sep_trigger }, {
				sep_trigger = Zero,
				N = --N_bak,
				decr_trigger = One
				+ascii_ent,
				Goto("GetDigitOutT_End")
			}),
			Zero,
			Goto("GetDigitOutT_End")
		}, true),

		While("DispIShift", { --i }, {
			// i_shift * 10
			i_shift = i_shift << 1,
			i_shift = (i_shift<<2) + i_shift,
		}),

		// (N, t2) <- div(N, i_shift)
		X = -i_shift + N,
		Q = Zero,
		R = +N,
		While("ShowSub", { MoreEq(X, R, t1) }, {
			R = +X,
			X = -i_shift + X,
			++Q
		}, true),
		N = +R,
		t2 = +Q,

		--show_i,

		If("ReachedEnd", { +show_i }, {
			If("DecrNeeded", { MoreEq(N_bak, min_primep1, t1) }, {
				sep_trigger = One,
			}),
		}, true),

		t2 + ascii_0,
		"GetDigitOutT_End,"_asm
	};

	StatementList interruptMain = {
		// Save Acc, E
		AccBak = Const(),
		EBak = GetE,

		// Reset Flag
		//out_trigger = Zero,

		// Get Input
		If("InputAvailable", {
			// If input is available ...
			One,
			"SKI"_asm,
			"CLA"_asm
		}, {
			// Input a digit
			IN_tmp = Const("INP"),
			checkChar.stat(),
		}),

		// Process Triggers
		prepareOutput.stat("Prepare"),

		// Output
		If("OutputAvailable", {
			// If output is available ...
			One,
			"SKO"_asm,
			"CLA"_asm
		}, {
			// Output a digit (If any data remains)
			If("ShowMain", { getDigitOne.stat("GetDigitOne") }, {
				"OUT"_asm
			}),
		}),

		"InterruptReturn,"_asm,
		If("HaltTrigger", { +halt_trigger },{
			EBak >> 1,
			+AccBak,
			Return("INT_RET")
		}),
		// Load Acc, E
		EBak >> 1,
		+AccBak,
		"ION"_asm,
		Return("INT_RET")
	};

	StatementList program = {
		begin_interrupt("INT_MAIN", "INT_RET"),
		begin,

		+mask_init,
		"IMK"_asm,
		"SIO"_asm,
		"ION"_asm,	// enable interrupt

		While("MainLoop", { +halt_trigger }, {
		}, true),
		halt,

		interruptMain.stat("INT_MAIN"),

		Data::stat_all(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
