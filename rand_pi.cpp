#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data AccBak(INT, "ACCBAK", 0);
	Data EBak(INT, "EBAK", 0);

	Data out_trigger(INT, "OUTTRIG", 0);
	Data calc_trigger(INT, "CALCTRIG", 0);
	Data halt_trigger(INT, "HaltFlag", 0);
	Data period_trigger(INT, "PeriodFlag", 1);

	Data mask_sin(INT, "MASKSIN", 8);
	Data mask_sout(INT, "MASKSOUT", 4);

	Data IN_tmp(INT, "INTmp", MaybeUnused);

	Data ascii_0(CHAR, "ASCII0", '0');
	Data ascii_period(CHAR, "ASCIIPeriod", '.');

	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);
	Data i(INT, "TempI", 0);
	Data t_queue_1(INT, "TEMPQUEUE1", 0);

	Data debug(INT, "DebugNum", 0x6FFF);

	// Display Buffer
	Data queue_data(INT, "QueData", 0);
	for(int i = 60; i --> 0;) Data(INT, "", 0);
	Data queue_data_end_anchor(INT, "QueDataEndA", 0);
	Data queue_remain(INT, "QueRemain", 0);
	Data queue_ptr(PTR, "QuePtr", queue_data);
	Data queue_read_ptr(PTR, "QueReadPtr", queue_data);
	Data queue_ptr_init(PTR, "QuePtrInit", queue_data);
	Data queue_ptr_end(PTR, "QuePtrEnd", queue_data_end_anchor);

	Data seed(INT, "RandSeed", 1);
	//Data N(INT, "LoopN", 500000);
	Data N(INT, "LoopN", 30000);
	Data sum(INT, "CntSum", 0);
	Data in(INT, "CntIn", 0);
	Data x(INT, "PosX", 0);
	Data y(INT, "PosY", 0);
	Data keta(INT, "Keta", 6);
	Data Q(INT, "QVal", 0);

	Data random(INT, "Rand", 0);
	Data rand16(INT, "Rand16", 0);
	Data tmp_random(INT, "TmpRandom", 0);
	Data randMod(INT, "RandMod", 0x7FFF);

	Subroutine xorshift = {
		If({ +random }, {
			random = +seed,
		}, true),
		tmp_random = random << 13,
		random = random ^ tmp_random,
		tmp_random = random >> 17,
		random = random ^ tmp_random,
		tmp_random = random << 5,
		random = random ^ tmp_random,
		rand16 = random & randMod,
	};

	// Queue Util
	auto que_push = [&](Data d){
		// enqueue
		return Subroutine{
			*queue_ptr = +d,
			++queue_ptr,
			If({-queue_ptr + queue_ptr_end}, {
				queue_ptr = +queue_ptr_init,
			}, true),
			++queue_remain,
		}.stat();
	};

	auto print = que_push;

	Subroutine que_pop = {
		t_queue_1 = +*queue_read_ptr,
		++queue_read_ptr,
		If({-queue_read_ptr + queue_ptr_end}, {
			queue_read_ptr = +queue_ptr_init,
		}, true),
		--queue_remain,
		+t_queue_1
	};

	Subroutine checkChar = {
		// Any key -> Start output
		calc_trigger = One,
		+mask_sout,
		"IMK"_asm,
	};

	// Process Trigger
	Subroutine prepareOutput = {
		If({ +calc_trigger }, {
			calc_trigger = Zero,
			For({{ i = Zero }, { i ^ N}, { ++i }}, {
				++sum,
				xorshift(),
				x = rand16 * rand16,
				xorshift(),
				y = rand16 * rand16 + x,
				"CLE"_asm,
				//If("InCircle", { -y + border, GetNegative }, {
				If("InCircle", { y >> 30 }, {
					++in,
				}, true),
			}),
			in = in << 2,
			While({+keta}, {
				Q = +ascii_0,
				While({ -sum + in, GetNegative }, {
					++Q,
					in = -sum + in,
				}, true),
				print(Q),
				in = in << 1,
				in = (in << 2) + in,
				If({ +period_trigger }, {
					print(ascii_period),
					period_trigger = Zero,
				}),
				--keta,
			}),
			out_trigger = One,
		})
	};

	// Interrupt Routine
	Subroutine interruptMain = {
		// Save Acc, E
		AccBak = Const(),
		EBak = GetE,

		// Get Input
		If("InputAvailable", {
			// If input is available ...
			One,
			"SKI"_asm,
			"CLA"_asm
		}, {
			// Input a digit
			IN_tmp = Const("INP"),
			checkChar(),
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
			If({ +out_trigger }, {
				If({ +queue_remain }, {
					que_pop(),
					"OUT"_asm,
				}, Else, {
					halt,  // shutdown
				}),
			}),
		}),

		// Return
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

	// Main Program
	Subroutine program = {
		begin_interrupt("INT_MAIN", "INT_RET"),
		begin,

		+mask_sin,
		"IMK"_asm,
		"SIO"_asm,
		"ION"_asm,	// enable interrupt

		While("MainLoop", { +halt_trigger }, {
			++seed,
		}, true),
		halt,

		interruptMain.stat("INT_MAIN"),

		Data::stat_all(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
