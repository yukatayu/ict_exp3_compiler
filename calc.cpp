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
	//Data show_i(INT, "ShowIParam", 0);

	Data out_trigger(INT, "DECLTRIG", 0);
	Data halt_trigger(INT, "HaltFlag", 0);

	Data mask_sin(INT, "MASKSIN", 8);
	Data mask_sout(INT, "MASKSOUT", 4);

	Data IN_tmp(INT, "INTmp", 0);
	//Data N(INT, "N", 0);
	//Data N_num(INT, "NNUM", 0);  // Nの桁数

	Data ascii_0(INT, "ASCII0", 48);
	Data ascii_A(INT, "ASCIIA", 65);
	Data ascii_ent(INT, "ASCIIENTER", 10);
	Data ascii_sp(INT, "ASCIISPACE", 32);
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
	Data t_calc_index(INT, "TEMPCALCINDEX", 0);
	Data t_calc_1(INT, "TEMPCALC1", 0);
	Data t_calc_2(INT, "TEMPCALC2", 0);
	Data t_calc_3(INT, "TEMPCALC3", 0);
	Data t_result(INT, "TEMPRESULT", 0);
	//Data Q(INT, "QVAL", 0);
	//Data R(INT, "RVAL", 0);
	//Data X(INT, "XVAL", 0);

	// Display Buffer
	const int QUEUE_MAX = 50;
	Data queue_data(INT, "QUEDATA", 0);
	for(int i = QUEUE_MAX; i --> 0;) Data(INT, "", 0);
	Data queue_data_end_anchor(INT, "QUEDATENDA", 0);
	Data queue_index(INT, "QUEINDEX", 0);
	Data queue_ptr(PTR, "QUEPTR", queue_data);
	Data queue_ptr_init(PTR, "QUEPTRINIT", queue_data);
	Data queue_ptr_end(PTR, "QUEPTRINIT", queue_data_end_anchor);

	// Stack
	const int STACK_MAX = 30;
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
	Data buffer_type_data(INT, "BUFTYPEDATA", 0);
	for(int i = STACK_MAX; i --> 0;) Data(INT, "", 0);
	Data buffer_index(INT, "BUFINDEX", 0);
	Data buffer_ptr(PTR, "BUFPTR", buffer_data);
	Data buffer_ptr_init(PTR, "BUFPTRINIT", buffer_data);
	Data buffer_type_ptr(PTR, "BUFTYPEPTR", buffer_type_data);
	Data buffer_type_ptr_init(PTR, "BUFTYPEPTRINIT", buffer_type_data);

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

	Data t_debug(INT, "TEMPDEBUG", 0);  // TODO: remove

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

	// TODO
	//helper_ConstString_Safe(disp_count_postfix, "DispCountPostfix", "/100");
	//helper_ConstString_Safe(prompt_text, "PromptText", " > ");

	// Initialization
	StatementList resetTokenStack = {
		stack_index
		= token_index
		= buffer_index
		= Zero,

		stack_ptr = +stack_ptr_init,
		token_ptr = +token_ptr_init,
		buffer_ptr = +buffer_ptr_init,
		token_type_ptr = +token_type_ptr_init,
		buffer_type_ptr = +buffer_type_ptr_init,

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

	// String Utilities
	// TODO: to HEX
	auto cToi = [&](Data c){
		return StatementList {
			t_ctoi_2 = One,  // error
			t_ctoi_1 = -ascii_0 + c,
			If({ Negative(t_ctoi_1) }, {
				If({
					-dec + t_ctoi_1,
					GetNegative
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

	// TODO: remove
	auto cToi_dec = [&](Data c){
		return StatementList {
			t_ctoi_2 = One,  // error
			t_ctoi_1 = -ascii_0 + c,
			If({ Negative(t_ctoi_1) }, {
				If({
					-dec + t_ctoi_1,
					GetNegative
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

	StatementList error = {
		// TODO: implement
	};

	auto print = [&](Data ptr){
		return StatementList{
		}.stat();
	};

	// Stack Util
	auto push = [&](Data i){
		return StatementList{
			// TODO: border check
			*stack_ptr = +i,
			++stack_ptr,
			++stack_index,
		}.stat();
	};

	StatementList pop = {
		--stack_ptr,
		--stack_index,
		+*stack_ptr
	};

	StatementList peek = {
		stack_tmp = One,
		stack_tmp = -stack_tmp + stack_ptr,
		+*stack_tmp,
	};

	// Operator Precedence
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

	// Step1: Tokenize
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

	// Step2: RPN
	StatementList RPN = {
		For({{ RPN_i = Zero }, { -token_index + RPN_i, GetNegative }, { ++RPN_i }}, {
			RPN_token = token_ptr_init + RPN_i,
			RPN_type = token_type_ptr_init + RPN_i,
			If("RPNTokenNum", { +*RPN_type }, {  // 数字だったらbufferへ
				*buffer_ptr = +*RPN_token,
				++buffer_ptr,
				*buffer_type_ptr = One,
				++buffer_type_ptr,
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
							*buffer_type_ptr = Zero,
							++buffer_type_ptr,
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
									-t2 + t1,
									GetNegative
								}, {
									*buffer_ptr = pop(),
									++buffer_ptr,
									*buffer_type_ptr = Zero,
									++buffer_type_ptr,
									++buffer_index,
								}, Else, {
									push(*RPN_token),
									Break("CompareOps"),
								}, true)
							})
						}, true),
					}, true),
				}, true),
			})
		}),
		// この時点で一番外側の ( ) によってバッファは全てフラッシュされているはず
		// (TODO: さもなくばエラー)
	};

	// Step3: Calculate
	StatementList calc = {
		// reset stack
		stack_index = Zero,
		stack_ptr = +stack_ptr_init,
		For("CalcMain", {{ t_calc_index = Zero }, {
			-buffer_index + t_calc_index, GetNegative
		}, { ++t_calc_index }}, {
			RPN_token = buffer_ptr_init + t_calc_index,
			RPN_type = buffer_type_ptr_init + t_calc_index,
			If({ +*RPN_type }, {  // number
				push(*RPN_token)
			}, Else, {  // Ops
				t_calc_2 = pop(),  // reverse order
				t_calc_1 = pop(),
				If({ -ascii_plus + *RPN_token }, {
					t_calc_1 = t_calc_1 + t_calc_2
				}, true),
				If({ -ascii_minus + *RPN_token }, {
					t_calc_1 = -t_calc_2 + t_calc_1
				}, true),
				If({ -ascii_ast + *RPN_token }, {
					t2 = +t_calc_1,
					t_calc_1 = Zero,
					For({{t1=Zero}, {-t_calc_2 + t1, GetNegative}, {++t1}}, {
						t_calc_1 = t2 + t_calc_1
					}),
				}, true),
				If({ -ascii_slush + *RPN_token }, {
					// TODO: implement
				}, true),
				push(t_calc_1),
			}),
		}),
		pop()  // TODO: エラー処理
	};

	// Input Subroutine
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
				// ignore space
				If({ -ascii_sp + IN_tmp }, {
					// [0-9+-*/()] -> str
					*raw_str_ptr = +IN_tmp,
					++raw_str_ptr,
					*raw_str_ptr = Zero,
				})
			}, true),
		}, true),
		t1 = -ascii_0 + IN_tmp,
	};

	// Process Trigger
	StatementList digitOutput = {
		If({ +out_trigger }, {
			out_trigger = Zero,
			tokenize_and_reset_str(),
			RPN(),
			t_result = calc(),
			"_B_,"_asm,
			t_debug = +t_result,
			//halt
			// TODO: v  allow input v
			+mask_sin,
			"IMK"_asm,
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
