#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data AccBak(INT, "ACCBAK", 0);
	Data EBak(INT, "EBAK", 0);
	Data show_i(INT, "ShowIParam", 0);

	Data out_trigger(INT, "DECLTRIG", 0);
	Data halt_trigger(INT, "HaltFlag", 0);

	Data mask_sin(INT, "MASKSIN", 8);
	Data mask_sout(INT, "MASKSOUT", 4);

	Data IN_tmp(INT, "INTmp", 0);
	Data N(INT, "N", 0);
	Data N_num(INT, "NNUM", 0);  // Nの桁数

	Data ascii_0(INT, "ASCII0", 48);
	Data ascii_A(INT, "ASCIIA", 65);
	Data ascii_ent(INT, "ASCIIENTER", 10);
	Data ascii_ctrl_D(INT, "ASCIICTRLD", 4);
	Data ascii_paren_begin(INT, "ASCIIPARBEG", 40);
	Data ascii_paren_end(INT, "ASCIIPAREND", 41);
	Data ascii_plus(INT, "ASCIIPLUS", 43);
	Data ascii_minus(INT, "ASCIIMINUS", 45);
	Data ascii_ast(INT, "ASCIIAST", 42);
	Data ascii_slush(INT, "ASCIISLU", 47);
	Data dec(INT, "DECVAL", 10);

	// Temporary
	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);
	Data t_ctoi_1(INT, "TEMPCTOI1", 0);
	Data t_ctoi_2(INT, "TEMPCTOI2", 0);
	Data t_priority_1(INT, "TEMPPRI1", 0);
	Data t_priority_2(INT, "TEMPPRI2", 0);
	//Data Q(INT, "QVAL", 0);
	//Data R(INT, "RVAL", 0);
	//Data X(INT, "XVAL", 0);

	// Stack
	// TODO: store value type
	const int STACK_MAX = 20; // TODO: increase
	Data stack_data(INT, "STACKDATA", 0);
	for(int i = STACK_MAX; i --> 0;) Data(INT, "", 0);
	Data stack_index(INT, "STACKINDEX", 0);
	Data stack_ptr(PTR, "STACKPTR", stack_data);
	Data stack_ptr_init(PTR, "STACKPTRINIT", stack_data);

	Data stack_tmp(INT, "STACKTMPVAL", 0);
	Data RPN_i(INT, "RPNITR", 0);
	Data RPN_token(INT, "RPNITRTOKEN", 0);
	Data RPN_type(INT, "RPNITRTYPE", 0);

	// Buffer
	Data buffer_data(INT, "BUFDATA", 0);
	for(int i = STACK_MAX; i --> 0;) Data(INT, "", 0);
	Data buffer_index(INT, "BUFINDEX", 0);
	Data buffer_ptr(PTR, "BUFPTR", buffer_data);
	Data buffer_ptr_init(PTR, "BUFPTRINIT", buffer_data);

	// Heap
	/*Data heap_data(INT, "HEAPDATA", 0);
	for(int i = STACK_MAX*5; i --> 0;) Data(INT, "", 0);
	Data heap_index(INT, "HEAPINDEX", 0);
	Data heap_ptr(PTR, "HEAPPTR", heap_data);
	Data heap_ptr_init(PTR, "HEAPPTRINIT", heap_data);
	*/

	// Token List
	// Note: [Type] Num: 1, Op: 0
	Data token_data(INT, "TOKENDATA", 0);
	for(int i = STACK_MAX; i --> 0;) Data(INT, "", 0);
	Data token_type_data(INT, "TOKENTYPEDATA", 0);
	for(int i = STACK_MAX; i --> 0;) Data(INT, "", 0);
	Data token_index(INT, "TOKENINDEX", 0);
	Data token_ptr(PTR, "TOKENPTR", token_data);
	Data token_ptr_init(PTR, "TOKENPTRINIT", token_data);
	Data token_type_ptr(PTR, "TOKENTYPEPTR", token_type_data);
	Data token_type_ptr_init(PTR, "TOKENTYPEPTRINIT", token_type_data);

	Data token_num_tmp(INT, "TOKENNUMTMP", 0);

	// String
	Data raw_str_data(INT, "RAWSTRDATA", 0);
	for(int i = (STACK_MAX/2+1) * 5 + 5; i --> 0;) Data(INT, "", 0);
	Data raw_str_ptr(PTR, "RAWSTRPTR", raw_str_data);
	Data raw_str_ptr_init(PTR, "RAWSTRPTRINIT", raw_str_data);

	Data before_num(INT, "BEFORETYPE", 0);  // init: (not number)

	StatementList resetTokenStack = {
		stack_index
		= token_index
		= buffer_index
		= Zero,

		stack_ptr = +stack_ptr_init,
		token_ptr = +token_ptr_init,
		buffer_ptr = +buffer_ptr_init,
		token_type_ptr = +token_type_ptr_init,

		token_num_tmp = before_num = Zero,
	};

	StatementList resetInputString = {
		raw_str_ptr = +raw_str_ptr_init,
		*raw_str_ptr = +ascii_paren_begin,
		++raw_str_ptr,
		*raw_str_ptr = Zero,
	};

	StatementList finishInputString = {
		*raw_str_ptr = +ascii_paren_end,
		++raw_str_ptr,
		*raw_str_ptr = Zero,
	};

	auto cToi = [&](Data c){
		return StatementList {
			t_ctoi_2 = One,  // error
			t_ctoi_1 = -ascii_0 + c,
			If({ Negative(t_ctoi_1) }, {	
				If({
					-dec + t_ctoi_1,
					"CIL"_asm,
					"CLA"_asm,
					"CIL"_asm,
				}, {
					t_ctoi_2 = Zero
				})
			}, true),
			// E <- t2
			"CLE"_asm,
			+t_ctoi_2,
			"SZA"_asm,
			"CME"_asm,
			+t_ctoi_1
		}.stat();
	};

	StatementList tokenize_and_reset_str = {
		finishInputString(),
		resetTokenStack(),
		For("TokenizeStr", {{raw_str_ptr = +raw_str_ptr_init}, {t1 = +*raw_str_ptr}, { ++raw_str_ptr }}, {
			t2 = cToi(t1),
			If("TokenIsNum", { GetE }, {
				// token_num_tmp *= 10
				token_num_tmp = (token_num_tmp << 1),
				token_num_tmp = (token_num_tmp << 2) + token_num_tmp,
				token_num_tmp = token_num_tmp + t2,
				before_num = One,
			}, Else, {
				If({ +before_num }, {
					*token_ptr = +token_num_tmp,
					++token_ptr,
					*token_type_ptr = One,
					++token_type_ptr,
					++token_index,
					// reset number status
					before_num = token_num_tmp = Zero,
				}),
				*token_ptr = +t1,
				++token_ptr,
				*token_type_ptr = Zero,
				++token_type_ptr,
				++token_index,
			}, true)
		}),
		resetInputString()
	};

	auto push = [&](Data i){
		return StatementList{
			// TODO: border check
			*stack_ptr = +i,
			++stack_ptr,
			++stack_index,
		}.stat();
	};

	// returns char*
	StatementList pop = {
		--stack_ptr,
		--stack_index,
		+*stack_ptr
	};

	StatementList priority = {
		t_priority_1 = Const(),
		t_priority_2 = +ascii_0,  // 十分大きければ何でも良い

		If({ -ascii_plus + t_priority_1 }, {
			t_priority_2 = One,
		}, true),
		If({ -ascii_minus + t_priority_1 }, {
			t_priority_2 = One,
		}, true),
		If({ -ascii_ast + t_priority_1 }, {
			t_priority_2 = Zero,
		}, true),
		If({ -ascii_slush + t_priority_1 }, {
			t_priority_2 = Zero,
		}, true),
		+t_priority_2
	};

	StatementList peek = {
		stack_tmp = One,
		stack_tmp = -stack_tmp + stack_ptr,
		//stack_tmp =
		+*stack_tmp,
	};

	StatementList error = {
		// TODO: implement
	};

	StatementList RPN = {
		For({{ RPN_i = Zero }, {
			-token_index + RPN_i,
			"CIL"_asm,
			"CLA"_asm,
			"CIL"_asm,
		}, { ++RPN_i }}, {
			RPN_token = token_ptr_init + RPN_i,
			RPN_type = token_type_ptr_init + RPN_i,
			If("RPNTokenNum", { +*RPN_type }, {  // 数字だったらbufferへ
				*buffer_ptr = +*RPN_token,
				++buffer_ptr,
				++buffer_index,
			}, Else, {  // 演算子や ( ) の場合
				If({ -*RPN_token + ascii_paren_begin }, {  // `(`
					push(*RPN_token),
				}, Else, {
					If({ -*RPN_token + ascii_paren_end }, {  // `)`
						// スタックを巻き戻す処理を入れる
						While({
							t1 = One,
							If({+stack_index}, {t1 = Zero}, Else, {t2 = pop()} , true),
							If({-ascii_paren_begin + t2}, {t1 = Zero} , true),
							+t1
						}, {
							*buffer_ptr = +t2,
							++buffer_ptr,
							++buffer_index,
						}),
						// TODO: 括弧の対応エラーを受けるならここ
					}, Else, {
						If({ +stack_index }, {  // stack is empty
							push(*RPN_token),
						}, Else, {  // need to compare operators
							While("CompareOps", { +stack_index }, {
								+*RPN_token,
								t1 = priority(),
								peek(),
								t2 = priority(),
								If({
									-t1 + t2,
									"CIL"_asm,
									"CLA"_asm,
									"CIL"_asm,
								}, {
									*buffer_ptr = pop(),
									++buffer_ptr,
									++buffer_index,
								}, Else, {
									push(*RPN_token),
									Break("CompareOps"),
								})
							})
						}, true),
					}, true),
				}, true),
			})

		//,+*RPN_token,
		//"_B_,"_asm,
		//+*RPN_token,

		}),
		// この時点で一番外側の ( ) によってバッファは全てフラッシュされているはず
		// (さもなくばエラー)
	};

	// Input a char
	StatementList checkChar = {
		// Enter -> Start output
		If("CheckCharEnt", { -ascii_ent + IN_tmp }, {
			out_trigger = One,
			+mask_sout,
			"IMK"_asm,
		}, Else, {
			// ^D -> halt
			If("CheckCharCtrlD", { -ascii_ctrl_D + IN_tmp }, {
				halt_trigger = One,
			}, Else, {
				// [0-9+-*/()] -> str
				*raw_str_ptr = +IN_tmp,
				++raw_str_ptr,
				*raw_str_ptr = Zero,
			}, true),
		}, true),
		t1 = -ascii_0 + IN_tmp,
	};

	// Process Trigger
	StatementList digitOutput = {
		If({ +out_trigger }, {
			tokenize_and_reset_str(),
			RPN(),
			halt
		})
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
		digitOutput.stat("Output"),

		// Return
		If("HaltTrigger", { +halt_trigger },{
			EBak >> 1,
			+AccBak,
			Return("INT_RET")
		}),

		// Load Acc, E
		EBak >> 2,
		+AccBak,
		"ION"_asm,
		Return("INT_RET")
	};

	// Main Program
	StatementList program = {
		begin_interrupt("INT_MAIN", "INT_RET"),
		begin,

		resetInputString(),
		+mask_sin,
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
