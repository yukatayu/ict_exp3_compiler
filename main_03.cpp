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

	Data data_ptr_init(PTR, "DATAPTRINIT", data0);
	Data data_ptr(PTR, "DATAPTR", data0);

	Data result0(CHAR, "RESULT", 'E');
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
	Data dec(INT, "DECNUM", 10);
	Data dec_minus(INT, "DECMINUS", -10);

	Data X(INT, "X", 0);
	Data Q(INT, "Q", 0);
	Data R(INT, "R", 0);

	Data check_char(INT, "CHKCHAR", 0);
	Data check_char_upper(CHAR, "CHKCHARU", '9');
	Data check_char_lower(CHAR, "CHKCHARL", '0');

	Data check_char_upper_F(INT, "CHKCHARUF", 70);
	Data check_char_lower_m_A(INT, "CHKCHARLMA", -65);
	Data check_char_lower_m_A_m10(INT, "CHKCHARLMAM10", -55);

	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);
	Data tc(INT, "TEMPCMP", 0);
	Data flag(INT, "TFLAG", 0);
	Data cmp_res(INT, "CMPRES", 0);

	StatementList show{
		Statement{
			new While("ShowMain", {
				result_num.load()
			}, {
				Q = Zero,
				R = +result_num,
				X = +result_num,
				X = X + dec_minus,
				Statement{
					new While("ShowSub", {
						new MoreEq(X, R, t1, t2)
					}, {
						R = +X,
						X = X + dec_minus,
						++Q
					}, true)
				},

				result_num = +Q,

				*result_ptr_init = R + check_char_lower,

				Statement{
					new If("ShowRes", {
						result_num.load()
					}, {
						result_ptr2 = +result_ptr,
						++result_ptr2,
						result_ptr3 = +result_ptr,
						Statement{
							new While("ShowResShift", {
								t1 = -result_ptr_init,
								t1 + result_ptr2
							}, {
								*result_ptr2 = +*result_ptr3,
								--result_ptr2,
								--result_ptr3
							})
						}
					})
				},
				++result_ptr
			})
		},
		*result_ptr = Zero,
		halt
	};

	StatementList checkAF{
		t1 = -check_char,
		t1 = t1 + check_char_upper_F,
		Statement{
			new If("checkAF1__TOKEN__", {
				new Negative(t1)
			}, {
				halt
			})
		},
		t1 = check_char + check_char_lower_m_A,
		Statement{
			new If("checkAF2__TOKEN__", {
				new Negative(t1)
			}, {
				halt
			})
		},
		check_char = check_char + check_char_lower_m_A_m10
	};


	StatementList check{
		Statement{
			new If("check0", {
				check_char.load()
			}, {
				show.stat("SHOW")
			}, true)
		},

		t1 = -check_char,
		t1 = t1 + check_char_upper,
		Statement{
			new If("check1", {
				new Negative(t1)
			}, {
				checkAF.stat("AF1", "__TOKEN__")
			})
		},

		t1 = -check_char_lower,
		t1 = t1 + check_char,
		Statement{
			new If("check2", {
					new Negative(t1)
			}, {
				checkAF.stat("AF2", "__TOKEN__")
			})
		},
		check_char = +t1
	};


	StatementList program{
		begin,
		Statement{
			new While("Main", {
				One,  // infinity loop
			}, {
				check_char = +*data_ptr,
				check.stat("Check"),

				result_num = (result_num << 1),
				result_num = (result_num << 1),
				result_num = (result_num << 1),
				result_num = (result_num << 1),
				result_num = result_num + check_char,
				++data_ptr
			})
		},
		halt,

		X.stat(),
		Q.stat(),
		R.stat(),

		data0.stat(),
		data1.stat(),
		data2.stat(),
		data3.stat(),
		data4.stat(),
		data_ptr_init.stat(),
		data_ptr.stat(),

		result0.stat(),
		result1.stat(),
		result2.stat(),
		result3.stat(),
		result4.stat(),
		result5.stat(),

		result_ptr_init.stat(),
		result_ptr.stat(),
		result_ptr2.stat(),
		result_ptr3.stat(),

		result_num.stat(),
		dec.stat(),
		dec_minus.stat(),

		check_char.stat(),
		check_char_upper.stat(),
		check_char_lower.stat(),

		check_char_upper_F.stat(),
		check_char_lower_m_A.stat(),
		check_char_lower_m_A_m10.stat(),

		t1.stat(),
		t2.stat(),
		tc.stat(),
		flag.stat(),
		cmp_res.stat(),

		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
