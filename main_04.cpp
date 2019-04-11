#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data N(INT, "N", 9);
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
	Data t2(INT, "CMPTMP2", 0);
	Data flag(INT, "TFLAG", 0);
	Data cmp_res(INT, "CMPRES", 0);

	StatementList checkPrime = {
		i = +i_init,
		mi = -i_init,
		i2 = +i2_init,
		Statement{
			new While("CPLoop", {
				new MoreEq(N, i2, t1, t2)
			}, {
				Nc1 = +N,
				Nc2 = +N,
				Nc1 = Nc1 + mi,
				Statement{
					new While("CPLoop2", {
						new MoreEq(Nc1, Nc2, t1, t2)
					}, {
						Nc2 = +Nc1,
						Nc1 = Nc1 + mi
					}, true)
				},
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
		N.stat(),
		Nc1.stat(),
		Nc2.stat(),
		i_init.stat(),
		i2_init.stat(),
		i.stat(),
		mi.stat(),
		i2.stat(),
		t1.stat(),
		t2.stat(),
		flag.stat(),
		cmp_res.stat(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
