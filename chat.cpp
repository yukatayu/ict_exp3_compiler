#include <iostream>
#include <ex3.hpp>

EX3::Statement strCpy(EX3::Data from_ptr_init, EX3::Data to_ptr, EX3::Data tmp) {
	using namespace EX3;
	return For({ { tmp = +from_ptr_init }, { +*tmp }, { ++tmp } }, {
		*to_ptr = +*tmp,
		++to_ptr,
	});
}

int main(){
	using namespace EX3;
	using namespace helper;

	Data AccBak(INT, "ACCBAK", 0);
	Data EBak(INT, "EBAK", 0);

	// triggers
	Data in_trigger(INT, "INPTRIG", 0);
	Data out_trigger(INT, "OUTTRIG", 0);
	Data sep_trigger(INT, "SEPTRIG", 0);
	Data halt_trigger(INT, "HaltFlag", 0);
	Data redraw_trigger(INT, "REDRAWTRIG", 0);

	Data mask_sin (INT, "MASKSIN",  8);
	Data mask_sout(INT, "MASKSOUT", 4);
	//Data mask_pin (INT, "MASKPIN",  2);
	//Data mask_pout(INT, "MASKPOUT", 1);
	Data mask_pin (INT, "MASKPIN",  0);
	Data mask_pout(INT, "MASKPOUT", 0);	// TODO

	Data send_buf(INT, "SENDBUFFER", 0);
	for(int i=100; i --> 0;)
		Data(INT, "", 0);
	Data send_buf_ptr_init(PTR, "SENDBUFPTRINIT", send_buf);
	Data send_buf_ptr(PTR, "SENDBUFPTR", send_buf);

	Data recv_buf(INT, "RECVBUFFER", 0);
	for(int i=100; i --> 0;)
		Data(INT, "", 0);
	Data recv_buf_ptr_init(PTR, "RECVBUFPTRINIT", recv_buf);
	Data recv_buf_ptr(PTR, "RECVBUFPTR", recv_buf);

	Data disp_buf(INT, "DISPBUFFER", 0);
	for(int i=150; i --> 0;)
		Data(INT, "", 0);
	Data disp_buf_ptr_init(PTR, "DISPBUFPTRINIT", disp_buf);
	Data disp_buf_ptr(PTR, "DISPBUFPTR", disp_buf);

	Data ascii_ent(INT, "ASCIIENTER", 10);
	Data ascii_sp(INT, "ASCIISPACE", 32);
	Data ascii_ctrl_D(INT, "ASCIICTRLD", 4);

	// Temporary
	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);
	Data IN_tmp(INT, "INTMP", 0);

	// エスケープシーケンスを安全に扱うために、文字列を整数に変換して扱う
	// 文字列リテラルは末尾に '\0' が入るのでヌル文字の追加は不要
	// Ptr は MaybeUnused
#define helper_ConstString_Safe(ProgName, AsmName, Str) \
	Data ProgName(INT, AsmName, static_cast<unsigned char>(Str[0])); \
	for(int i=1; i<(sizeof(Str)/sizeof(*Str)); ++i) \
		Data(INT, "", static_cast<unsigned char>(Str[i])); \
	Data(INT, "", 0); \
	Data ProgName ## _ptr_init (PTR, AsmName "PtrInit", ProgName);
	// Data ProgName ## _ptr (PTR, AsmName "Ptr", ProgName, MaybeUnused);

	helper_ConstString_Safe(prompt_text, "PromptText", " > ");

	//helper_ConstString_Safe(red_text, "RedText", "\033[91m");  // Red (Forward)
	//helper_ConstString_Safe(red_back, "RedBack", "\033[41m");  // Red Back
	//helper_ConstString_Safe(green_text, "GreenText", "\033[92m");  // Green
	//helper_ConstString_Safe(reset_text, "ResetText", "\033[0m");  // Reset
	//helper_ConstString_Safe(clear_line, "ClearLine", "\033[1G\033[0K");  // ClearLine
	// vv debug vv
	helper_ConstString_Safe(red_text, "RedText", "<Red>");
	helper_ConstString_Safe(red_back, "RedBack", "<RedBack>");
	helper_ConstString_Safe(green_text, "GreenText", "<Green>");
	helper_ConstString_Safe(reset_text, "ResetText", "<Reset>");
	helper_ConstString_Safe(clear_line, "ClearLine", "\r\n");

	// Status
	Data status_output(INT, "ISSTATOUT", 0);
	Data status_color(PTR, "STATUSCOLOR", reset_text);
	Data status_unread(INT, "STATUREAD", 0);
	Data input_cnt(INT, "INPUTCHARNUM", 0);

	// テキストのレンダリング
	StatementList renderText = {
		disp_buf_ptr = +disp_buf_ptr_init,
		/*For({ { clear_line_ptr = +clear_line_ptr_init }, { +*clear_line_ptr }, { ++clear_line_ptr } }, {
			*disp_buf_ptr = +*clear_line_ptr,
			++disp_buf_ptr,
		}),*/

		strCpy(clear_line_ptr_init, disp_buf_ptr, t1),
		// 未読があったら赤色
			// 何故かバグる //TODO
		//	status_color = Cond("Debug_Cond", { +status_unread }, { +red_back_ptr_init }, { +reset_text_ptr_init }, true),
		If({ +status_unread }, {
			status_color = +red_back_ptr_init
		}, Else, {
			status_color = +reset_text_ptr_init
		}),
		//"LDA STATUSCOLOR"_asm,
		//"_B_,LDA STATUSCOLOR"_asm,
		strCpy(status_color, disp_buf_ptr, t1),
		*disp_buf_ptr = +ascii_sp, ++disp_buf_ptr,
		*disp_buf_ptr = +ascii_sp, ++disp_buf_ptr,
		strCpy(reset_text_ptr_init, disp_buf_ptr, t1),
		strCpy(green_text_ptr_init, disp_buf_ptr, t1),
		*disp_buf_ptr = +ascii_sp, ++disp_buf_ptr,
		// TODO: 現在の文字数
		strCpy(reset_text_ptr_init, disp_buf_ptr, t1),
		strCpy(prompt_text_ptr_init, disp_buf_ptr, t1),
		strCpy(send_buf_ptr_init, disp_buf_ptr, t1),

		// NULL文字を追加し、ポインタ位置をリセット
		*disp_buf_ptr = Zero,
		disp_buf_ptr = +disp_buf_ptr_init,
	};

	// Input a char
	StatementList inputChar = {
		// Enter -> Start output
		/*If("CheckCharEnt", { -ascii_ent + IN_tmp }, {
			out_trigger = One,
		}, true),*/
		// ^D -> halt
		If("CheckCharCtrlD", { -ascii_ctrl_D + IN_tmp }, {
			halt_trigger = One,
		}, Else, {
			*send_buf_ptr = +IN_tmp,
			++send_buf_ptr,
		}, true),
		out_trigger = One,
	};

	// Process Trigger
	StatementList prepareOutput = {
		If("OutTrigger", { +out_trigger }, {
			out_trigger = Zero,
			renderText.stat("RenderText")
			+mask_sout + mask_pin + mask_pout,
			"IMK"_asm,
			status_output = One,
		}),
		If("InTrigger", { +in_trigger }, {
			in_trigger = Zero,
			+mask_sin + mask_pin + mask_pout,
			"IMK"_asm,
			status_output = Zero,
		}),
	};

	// Get Next Output Char
	StatementList getOutput = {
		t1 = +*disp_buf_ptr,
		If("StepDispText", { +t1 }, {
			++disp_buf_ptr
		}, Else, {
			If({ +status_output }, {  // TODO: unneeded?
				in_trigger = One
			})
		}),
		+t1
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
			inputChar.stat(),
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
			If("ShowMain", { getOutput.stat("GetOutput") }, {
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

		+mask_sin + mask_pin + mask_pout,
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
