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
	Data t(INT, "CMPTMP", 0);
	Data t1(INT, "T1", 0);
	Data t2(INT, "T2", 0);
	Data t3(INT, "T3", 0);
	Data t4(INT, "T4", 0);
	Data flag(INT, "TFLAG", 0);
	Data cmp_res(INT, "CMPRES", 0);

	StatementList cmpNc1Nc2 = {
		cmp_res = One,
		Statement{ new If("Cmp__TOKEN__", {
			Nc2.load(),
		}, {
			t = -Nc2,
			Statement{ new Const("CLE\n") },
			t + Nc1,
			cmp_res = GetE,
		})}
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
				Nc1 = Nc1 + mi,
				Statement{
					new While("CPLoop2", {
						cmpNc1Nc2.stat("cnn1", "__TOKEN__"),
						cmp_res.load(),
						//Statement{ new Negative(Nc1) },
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
		t.stat(),
		t1.stat(),
		t2.stat(),
		t3.stat(),
		t4.stat(),
		flag.stat(),
		cmp_res.stat(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
