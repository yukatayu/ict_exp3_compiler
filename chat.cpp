#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data AccBak(INT, "ACCBAK", 0);
	Data EBak(INT, "EBAK", 0);
	Data show_i(INT, "ShowIParam", 0);

	Data out_trigger(INT, "DECLTRIG", 0);
	Data sep_trigger(INT, "SEPTRIG", 0);
	Data halt_trigger(INT, "HaltFlag", 0);

	Data mask_sin (INT, "MASKSIN",  8);
	Data mask_sout(INT, "MASKSOUT", 4);
	Data mask_pin (INT, "MASKPIN",  2);
	Data mask_pout(INT, "MASKPOUT", 1);

	Data send_buf(INT, "SENDBUFFER", 0);
	for(int i=500; i --> 0;)
		Data(INT, "", 0);
	Data send_buf_ptr_init(PTR, "SENDBUFPTRINIT", send_buf);
	Data send_buf_ptr(PTR, "SENDBUFPTR", send_buf);

	Data recv_buf(INT, "RECVBUFFER", 0);
	for(int i=500; i --> 0;)
		Data(INT, "", 0);
	Data recv_buf_ptr_init(PTR, "RECVBUFPTRINIT", recv_buf);
	Data recv_buf_ptr(PTR, "RECVBUFPTR", recv_buf);

	Data ascii_ent(INT, "ASCIIENTER", 10);
	Data ascii_ctrl_D(INT, "ASCIICTRLD", 4);

	// Temporary
	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);
	Data IN_tmp(INT, "INTMP", 0);

	Data TestText(CHAR, "TextText", '\033');
	Data(CHAR, "", '[');
	Data(CHAR, "", '9');
	Data(CHAR, "", '1');
	Data(CHAR, "", 'm');
	Data(CHAR, "", 'M');
	Data(CHAR, "", 'S');
	Data(CHAR, "", 'M');
	Data(INT, "", 0);
	Data TestTextPtrInit(PTR, "TextTextPtrInit", TestText);
	Data TestTextPtr(PTR, "TextTextPtr", TestText);

	// Input a char
	StatementList checkChar = {
		// Enter -> Start output
		If("CheckCharEnt", { -ascii_ent + IN_tmp }, {
			out_trigger = One,
		}, true),
		// ^D -> halt
		If("CheckCharCtrlD", { -ascii_ctrl_D + IN_tmp }, {
			halt_trigger = One,
		}, true),
	};

	// Process Trigger
	StatementList prepareOutput = {
		If("PrimeDeclTrigger", { +out_trigger }, {
			out_trigger = Zero,
			TestTextPtr = +TestTextPtrInit,
		})
	};

	// Get Next Digit
	StatementList getDigitOne = {
		t2 = +*TestTextPtr,
		If("StepTestText", { +t2 }, {
			++TestTextPtr
		}),
		+t2
	};

	// Interrupt Routine
	StatementList interruptMain = {
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
	StatementList program = {
		begin_interrupt("INT_MAIN", "INT_RET"),
		begin,

		+mask_sin + mask_sout + mask_pin + mask_pout,
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
