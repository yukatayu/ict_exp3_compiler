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
	Data send_trigger(INT, "SENDTRIG", 0);
	Data halt_trigger(INT, "HaltFlag", 0);
	Data redraw_trigger(INT, "REDRAWTRIG", 0);
	Data open_trigger(INT, "OPENTRIG", 0);

	Data debug_num(INT, "DEBUGNUM", 0);	// TODO: debug

	Data mask_sin (INT, "MASKSIN",  8);
	Data mask_sout(INT, "MASKSOUT", 4);
	Data mask_pin (INT, "MASKPIN",  2);
	Data mask_pout(INT, "MASKPOUT", 1);
	Data mask_s(INT, "MASKS", 0);
	Data mask_p(INT, "MASKP", 0);

	Data send_buf(INT, "SENDBUFFER", 0);
	for(int i=99; i --> 0;)
		Data(INT, "", 0);
	Data send_buf_end_anchor(INT, "SENDBUFENDANC", 0);
	Data send_buf_ptr_init(PTR, "SENDBUFPTRINIT", send_buf);
	Data send_buf_ptr(PTR, "SENDBUFPTR", send_buf);
	//Data send_buf_ptr_para(PTR, "SENDBUFPTR2", send_buf);
	Data send_buf_end_anchor_ptr(PTR, "SENDBUFENDANCP", send_buf_end_anchor);

	Data recv_buf(INT, "RECVBUFFER", 0);
	for(int i=99; i --> 0;)
		Data(INT, "", 0);
	Data recv_buf_end_anchor(INT, "RECVBUFENDANC", 0);
	Data recv_buf_ptr_init(PTR, "RECVBUFPTRINIT", recv_buf);
	Data recv_buf_ptr(PTR, "RECVBUFPTR", recv_buf);
	Data recv_buf_end_anchor_ptr(PTR, "RECVBUFENDANCP", recv_buf_end_anchor);

	Data disp_buf(INT, "DISPBUFFER", 0);
	for(int i=250; i --> 0;)
		Data(INT, "", 0);
	Data disp_buf_ptr_init(PTR, "DISPBUFPTRINIT", disp_buf);
	Data disp_buf_ptr(PTR, "DISPBUFPTR", disp_buf);

	Data ascii_ent(INT, "ASCIIENTER", 10);
	Data ascii_0(INT, "ASCIIZERO", 48);
	Data ascii_sp(INT, "ASCIISPACE", 32);
	Data ascii_bs(INT, "ASCIIBKSPACE", 8);
	Data ascii_del(INT, "ASCIIDELETE", 127);
	Data ascii_ctrl_D(INT, "ASCIICTRLD", 4);

	// Temporary
	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);
	Data A(INT, "TEMPA", 0);
	Data B(INT, "TEMPB", 0);
	Data Q(INT, "TEMPQ", 0);
	Data R(INT, "TEMPR", 0);
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

	helper_ConstString_Safe(disp_count_postfix, "DispCountPostfix", "/100");
	helper_ConstString_Safe(prompt_text, "PromptText", " > ");
	helper_ConstString_Safe(new_message_text, "NewMsgText", "New Message > ");

	helper_ConstString_Safe(red_text, "RedText", "\033[91m");  // Red (Forward)
	helper_ConstString_Safe(red_back, "RedBack", "\033[41m");  // Red Back
	helper_ConstString_Safe(green_text, "GreenText", "\033[92m");  // Green
	helper_ConstString_Safe(reset_text, "ResetText", "\033[0m");  // Reset
	helper_ConstString_Safe(clear_line, "ClearLine", "\033[1G\033[0K");  // ClearLine
	// vv debug vv
	//helper_ConstString_Safe(red_text, "RedText", "<Red>");
	//helper_ConstString_Safe(red_back, "RedBack", "<RedBack>");
	//helper_ConstString_Safe(green_text, "GreenText", "<Green>");
	//helper_ConstString_Safe(reset_text, "ResetText", "<Reset>");
	//helper_ConstString_Safe(clear_line, "ClearLine", "\r\n");

	// Helper
	Data keta_10(INT, "Keta10Arr", 100);
		Data(INT, "", 10);
		Data(INT, "", 1);
		Data(INT, "", 0);
	Data keta_10_ptr_init(PTR, "Keta10PtrInit", keta_10);

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
		If("OpenArrivedMsg", {+open_trigger}, {
			open_trigger = Zero,
			If({+status_unread}, {
				status_unread = Zero,
				strCpy(new_message_text_ptr_init, disp_buf_ptr, t1),
				strCpy(recv_buf_ptr_init, disp_buf_ptr, t1),
				recv_buf_ptr = +recv_buf_ptr_init,
				*recv_buf_ptr = Zero,
				*disp_buf_ptr = +ascii_ent, ++disp_buf_ptr,
			})
		}),

		*disp_buf_ptr = +ascii_sp, ++disp_buf_ptr,
		// 未読があったら赤色
		//	status_color = Cond("Debug_Cond", { +status_unread }, { +red_back_ptr_init }, { +reset_text_ptr_init }, true),
		If({ +status_unread }, {
			status_color = +red_back_ptr_init
		}, Else, {
			status_color = +reset_text_ptr_init
		}),
		strCpy(status_color, disp_buf_ptr, t1),
		*disp_buf_ptr = +ascii_sp, ++disp_buf_ptr,
		*disp_buf_ptr = +ascii_sp, ++disp_buf_ptr,
		strCpy(reset_text_ptr_init, disp_buf_ptr, t1),

		// 現在の文字数が0か上限だったら赤色、さもなくば緑色
		t2 = +green_text_ptr_init,
		If({-send_buf_ptr + send_buf_end_anchor_ptr}, {
			t2 = +red_text_ptr_init,
		}, true),
		If({-send_buf_ptr + send_buf_ptr_init}, {
			t2 = +red_text_ptr_init,
		}, true),
		strCpy(t2, disp_buf_ptr, t1),
		*disp_buf_ptr = +ascii_sp, ++disp_buf_ptr,

		// 現在の文字数を表示
		A = -send_buf_ptr_init + send_buf_ptr,
		For("ShowCharNumInput", { { t1 = +keta_10_ptr_init }, { +*t1 }, { ++t1 } }, {
			B = -*t1,
			Q = Zero,
			--Q,
			While({ MoreEq(A, B, t2) }, {
				R = +A,
				++Q,
				A = A + B
			}, true),
			*disp_buf_ptr = Q + ascii_0,
			++disp_buf_ptr,
			A = +R,
		}),
		// 現在の入力内容の表示
		strCpy(disp_count_postfix_ptr_init, disp_buf_ptr, t1),
		strCpy(reset_text_ptr_init, disp_buf_ptr, t1),
		strCpy(prompt_text_ptr_init, disp_buf_ptr, t1),
		strCpy(send_buf_ptr_init, disp_buf_ptr, t1),

		//"renderTextReturn,"_asm,

		// NULL文字を追加し、ポインタ位置をリセット
		*disp_buf_ptr = Zero,
		disp_buf_ptr = +disp_buf_ptr_init,
	};

	// Input a char
	StatementList inputChar = {
		// ^D -> halt
		If("CheckCharCtrlD", { -ascii_ctrl_D + IN_tmp }, {
			halt_trigger = One,
		}, true),
		// Enter -> Start output
		// TODO: とりあえずリセットして未読フラグを反転している (デバッグ)
		If("CheckCharEnt", { -ascii_ent + IN_tmp }, {
			//status_unread = -status_unread,
			//++status_unread,
			open_trigger = One,

			//If({ -ascii_bs + IN_tmp }, {
			send_trigger = One,
			send_buf_ptr = +send_buf_ptr_init,
			// パラレルを出力モードに
			mask_p = +mask_pout,
			+mask_s + mask_p,
			"IMK"_asm
			//}),
			// Debug TODO: Remove (unneeded clearing)
			//send_buf_ptr = +send_buf_ptr_init,
			//*send_buf_ptr = Zero
		}, Else, {
			t1 = Zero,
			If({ -ascii_bs + IN_tmp }, {
				t1 = One
			}, true),
			If({ -ascii_del + IN_tmp }, {
				t1 = One
			}, true),
			If("CheckCharBS", { +t1 }, {
				If({-send_buf_ptr + send_buf_ptr_init}, {
					--send_buf_ptr,
					*send_buf_ptr = Zero,
				})
			}, Else, {
				// 入力した時点で出力モードにする
				If({-send_buf_ptr + send_buf_end_anchor_ptr}, {
					*send_buf_ptr = +IN_tmp,
					++send_buf_ptr,
					*send_buf_ptr = Zero,
				})
			}),
		}, true),
		out_trigger = One,
		mask_s = +mask_sout,
		+mask_s + mask_p,
		"IMK"_asm,
	};

	StatementList inputCharParallel = {
		If("CheckCharBSPara", { -ascii_del + IN_tmp }, {
			// do nothing: Debug
		}, Else, {
			// バッファに蓄積
			If({-recv_buf_ptr + recv_buf_end_anchor_ptr}, {
				*recv_buf_ptr = +IN_tmp,
				++recv_buf_ptr,
				*recv_buf_ptr = Zero,
				status_unread = One,
			})
		}, true)
	};

	// Process Trigger
	StatementList prepareOutput = {
		If("OutTrigger", { +out_trigger }, {
			out_trigger = Zero,
			renderText.stat("RenderText")
			//status_output = One,
		}),
		If("InTrigger", { +in_trigger }, {
			in_trigger = Zero,
			//status_output = Zero,
		}),
	};

	// Get Next Output Char
	StatementList getOutput = {
		t1 = +*disp_buf_ptr,
		If("StepDispText", { +t1 }, {
			++disp_buf_ptr
		}, Else, {
			// 出力するものがなければ入力許可
			in_trigger = One,
			mask_s = +mask_sin,
			+mask_s + mask_p,
			"IMK"_asm,
		}),
		+t1
	};

	// Get Next Output (Parallel)
	StatementList getOutputParallel = {
		t1 = +*send_buf_ptr,
		If("StepOutputText", { +t1 }, {
			++send_buf_ptr
		})
		+t1
	};

	// Interrupt Routine
	StatementList interruptMain = {
		// Save Acc, E
		AccBak = Const(),
		EBak = GetE,

		//
		// シリアルポート処理
		//
		"SIO"_asm,

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

		//
		// パラレルポート処理
		//
		"PIO"_asm,
		If("InputAvailablePara", {
			// If input is available ...
			One,
			"SKI"_asm,
			"CLA"_asm
		}, {
			// Input a digit
			IN_tmp = Const("INP"),
			inputCharParallel.stat()
		}),
		// Output
		If("OutputAvailablePara", {
			// If output is available ...
			One,
			"SKO"_asm,
			"CLA"_asm
		}, {
			If({+send_trigger}, {
				// Output a char (If any data remains)
				If({ getOutputParallel.stat("GetOutputPara") }, {
					"OUT"_asm,
					++debug_num  // TODO: remove
				}, Else, {
					// パラレル出力フラグをOFFにして、出力バッファをクリア
					send_trigger = Zero,
					send_buf_ptr = +send_buf_ptr_init,
					*send_buf_ptr = Zero,
					// パラレルを入力モードに
					mask_p = +mask_pin,
					+mask_s + mask_p,
					"IMK"_asm
				})
			})
		}),

		"SIO"_asm,  // TODO: debug
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

		// 起動時に表示するため
		out_trigger = One,

		//+mask_sin + mask_pin + mask_pout,
		mask_s = +mask_sout,
		mask_p = +mask_pin,
		+mask_s + mask_p,
		"IMK"_asm,
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
