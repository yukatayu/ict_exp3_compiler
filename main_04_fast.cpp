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
	Data n_tmp(INT, "ITMP", 0);
	Data mi(INT, "MI", 0);
	// Note: i^2 < 65536
	Data i2(INT, "I2", 0);

	Data i_shift(INT, "ISH", 0);
	Data i_ptr(INT, "IPTR", 0);

	// Temporary
	Data t(INT, "TEMP", 0);
	Data flag(INT, "TFLAG", 0);
	Data cmp_res(INT, "CMPRES", 0);

	// Nc1 >= Nc2
	StatementList cmpNc1Nc2 = {
		t = -Nc2,
		Statement{ new Const("CLE\n") },
		t + Nc1,
		cmp_res = GetE,
	};

	StatementList checkPrime = {
		i = +i_init,
		mi = -i_init,
		i2 = +i2_init,
		Statement{
			new While("CPLoop", {
				Nc1 = +N,
				Nc2 = +i2,
				cmpNc1Nc2.stat("cnn0", "__TOKEN__"),
				cmp_res.load()
			}, {
				Nc1 = +N,
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
						Nc1 = +i_shift,
						Nc2 = +i,
						cmpNc1Nc2.stat("cnn2", "__TOKEN__"),
						cmp_res.load()
					}, {
						Statement{
							new If("DivCmp", {
								Nc1 = +n_tmp,
								Nc2 = +i_shift,
								cmpNc1Nc2.stat("cnn3", "__TOKEN__"),
								cmp_res.load()
							}, {
								t = -i_shift,
								n_tmp = n_tmp + t,
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
				t = i << 1,
				++t,
				i2 = i2 + t,
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
		N.stat(),
		Nc1.stat(),
		Nc2.stat(),
		i_init.stat(),
		i2_init.stat(),
		i.stat(),
		mi.stat(),
		i2.stat(),
		t.stat(),
		i_shift.stat(),
		i_ptr.stat(),
		n_tmp.stat(),
		flag.stat(),
		cmp_res.stat(),
		end,
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
