#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data data0(CHAR, "DATA", '3');
	Data data1(CHAR, "",     '0');
	Data data2(CHAR, "",     '3');
	Data data3(CHAR, "",     '9');
	Data data4(INT, "", 0);  // NULL

	Data data_ptr(PTR, "DATAPTR", data0);

	Data result0(CHAR, "RESULT", 'E', MaybeUnused);
	Data result1(CHAR, "", 'R');
	Data result2(CHAR, "", 'R');
	Data result3(CHAR, "", 'O');
	Data result4(CHAR, "", 'R');
	Data result5(INT, "", 0);  // NULL

	Data result_ptr_init(PTR, "RESULTPTRINIT", result0);
	Data result_ptr(PTR, "RESULTPTR", result0);
	Data result_ptr2(PTR, "RESULTPTR2", result0);
	Data result_ptr3(PTR, "RESULTPTR3", result0);

	Data result_num(INT, "RESULTNUM", 0);
	Data dec_minus(INT, "DECMINUS", -10);

	Data X(INT, "X", 0);
	Data Q(INT, "Q", 0);
	Data R(INT, "R", 0);

	Data check_char(INT, "CHKCHAR", 0);
	Data check_char_upper(CHAR, "CHKCHARU", '9');
	Data check_char_lower(CHAR, "CHKCHARL", '0');

	Data check_char_upper_F(INT, "CHKCHARUF", 70);  // 'F'
	Data check_char_lower_m_A(INT, "CHKCHARLMA", -65);  // -'A'
	Data check_char_lower_m_A_m10(INT, "CHKCHARLMAM10", -55);

	Data t1(INT, "TEMP1", 0);

	// `result_num` を10進数に変換する
	StatementList show{
		While("ShowMain", { +result_num }, {
			// `result_num` を10で割る
			Q = Zero,
			R = +result_num,
			X = result_num + dec_minus,
			While("ShowSub", { MoreEq(X, R, t1) }, {
				R = +X,
				X = X + dec_minus,
				++Q
			}, true),

			// 商を再代入
			result_num = +Q,

			// 余りに '0' を足したものを先頭に挿入
			*result_ptr_init = R + check_char_lower,

			// まだ `result_num` が残っているなら文字列をシフト
			If("ShowRes", { +result_num }, {
				result_ptr3 = result_ptr2 = +result_ptr,
				++result_ptr2,
				While("ShowResShift", {
					-result_ptr_init + result_ptr2
				}, {
					*result_ptr2 = +*result_ptr3,
					--result_ptr2,
					--result_ptr3
				})
			}),
			++result_ptr
		}),
		// 末尾にNULL文字を挿入
		*result_ptr = Zero,
		halt
	};

	// 文字が A-F の範囲にあるかどうかを検査
	StatementList checkAF{
		t1 = -check_char + check_char_upper_F,
		If("checkAF1__TOKEN__", { Negative(t1) }, {
			halt
		}),
		t1 = check_char + check_char_lower_m_A,
		If("checkAF2__TOKEN__", { Negative(t1) }, {
			halt
		}),
		check_char = check_char + check_char_lower_m_A_m10
	};

	// 文字が 0-9 の範囲にあるかどうかを検査
	// NULL文字を検出した場合、表示処理に飛ぶ
	// <1>
	StatementList check{
		If("check0", { +check_char }, {
			show.stat("SHOW")
		}, true),

		t1 = -check_char + check_char_upper,
		If("check1", { Negative(t1) }, {
			checkAF.stat("AF1", "__TOKEN__"),
			Goto("CheckEnd")
		}),

		t1 = -check_char_lower + check_char,
		If("check2", { Negative(t1) }, {
			checkAF.stat("AF2", "__TOKEN__")
		}),
		check_char = +t1,
		Const("CheckEnd,")
	};

	// プログラム全体
	StatementList program{
		begin,

		While("Main", { One }, {  // Infinity Loop
			// 1文字ずつ取り出してcheck関数に渡す
			check_char = +*data_ptr,
			check.stat("Check"),

			result_num = (result_num << 4) + check_char,
			++data_ptr
		}),
		halt,

		Data::stat_all(),
		end
	};

	// コンパイル
	std::string res = program.make();
	std::cout << res << std::endl;
}
