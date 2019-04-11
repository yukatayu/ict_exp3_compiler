#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data N(INT, "N", 65535);
	Data Nc2(INT, "NC2", 0);

	Data i_init(INT, "IINIT", 2);
	Data i2_init(INT, "I2INIT", 4);

	// Note: Always i < 256
	Data i(INT, "I", 0);
	Data n_tmp(INT, "ITMP", 0);
	Data mi(INT, "MI", 0);
	// Note: i^2 < 65536
	Data i2(INT, "I2", 0);

	Data i_shift(INT, "ISH", 0);
	Data i_ptr(INT, "IPTR", 0);

	// Temporary
	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);

	StatementList checkPrime = {
		i = +i_init,
		mi = -i_init,
		i2 = +i2_init,
		Statement{
			new While("CPLoop", {
				new MoreEq(N, i2, t1, t2)
			}, {
				Nc2 = +N,

				i_ptr = Zero,
				++i_ptr,
				i_shift = +i,
				n_tmp = +N,

				Statement{
					new While("CPPre", {
						new Negative(i_shift)
					}, {
						i_shift = (i_shift<<1),
						i_ptr = (i_ptr<<1)
					}, true)
				},

				Statement{
					new While("CPMain", {
						new MoreEq(i_shift, i, t1, t2)
					}, {
						Statement{
							new If("DivCmp", {
								new MoreEq(n_tmp, i_shift, t1, t2)
							}, {
								t1 = -i_shift,
								n_tmp = n_tmp + t1,
								//i_res = i_res + i_shift,
							})
						},
						i_shift = (i_shift >> 1),
						i_ptr = (i_ptr >> 1)
					})
				},

				Nc2 = +n_tmp,
				Statement{
					new If("CPBreak", {
						Nc2.load()
					},{
						new Goto("L1")
					}, true)
				},
				t1 = i << 1,
				++t1,
				i2 = i2 + t1,
				++i,
				--mi
			})
		},
		halt
	};

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
