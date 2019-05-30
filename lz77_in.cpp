#include <iostream>
#include <ex3.hpp>

int main(){
	using namespace EX3;
	using namespace helper;

	Data AccBak(INT, "ACCBAK", 0);
	Data EBak(INT, "EBAK", 0);
	Data show_i(INT, "ShowIParam", 0);

	Data calc_trigger(INT, "CALCTRIG", 0);
	Data out_trigger(INT, "OUTTRIG", 0);
	Data halt_trigger(INT, "HaltFlag", 0);

	Data mask_sin(INT, "MASKSIN", 8);
	Data mask_sout(INT, "MASKSOUT", 4);

	Data _global_no(INT, "GlobalNo", 0);
	Data _map_max_len(INT, "MapMaxLen", 10);

	Data IN_tmp(INT, "INTmp", 0);

	Data ascii_ent(INT, "ASCIIENTER", 10);

	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);

	Data t_queue_1(INT, "TEMPQUEUE1", 0);
	Data t_print_1(INT, "TEMPPRINT1", 0);
	Data t_print_2(INT, "TEMPPRINT2", 0);
	Data t_print_3(INT, "TEMPPRINT3", 0);
	Data t_print_4(INT, "TEMPPRINT4", 0);
	Data A(INT, "AVAL", 0);

	// String
	Data raw_str_data(INT, "RawStrData", 0);
	for(int i = 40; i --> 0;) Data(INT, "", 0);
	Data raw_str_ptr(PTR, "RawStrPtr", raw_str_data);
	Data raw_str_ptr_init(PTR, "RawStrPtrInit", raw_str_data);

	// Display Buffer
	Data queue_data(INT, "QueData", 0);
	for(int i = 60; i --> 0;) Data(INT, "", 0);
	Data queue_data_end_anchor(INT, "QueDataEndA", 0);
	Data queue_remain(INT, "QueRemain", 0);
	Data queue_ptr(PTR, "QuePtr", queue_data);
	Data queue_read_ptr(PTR, "QueReadPtr", queue_data);
	Data queue_ptr_init(PTR, "QuePtrInit", queue_data);
	Data queue_ptr_end(PTR, "QuePtrEnd", queue_data_end_anchor);

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

	/*auto printStr = [&](Data from_ptr_init, Data tmp){
		return For({ { tmp = +from_ptr_init }, { +*tmp }, { ++tmp } }, {
			print(*tmp)
		});
	};*/
	
	Data make_string_len(INT, "MakeStringLen", 0);
	Data alloc_len(INT, "AllocLen", 0);
	Data _alloc_new(INT, "AllocNew", 0);

	Subroutine makeString = {
		"MakeString, HEX 0"_asm,
		alloc_len = ++make_string_len,
		Call("Alloc"),
		Return("MakeString")
	};
	Subroutine makeMap = {
		"MakeMap, HEX 0"_asm,
		alloc_len = (_map_max_len << 1),
		++alloc_len,
		Call("Alloc"),
		Return("MakeMap")
	};
	Data _node_size(INT, "NodeSize", 4);
	Data _node_ptr(INT, "NodePtr", 0);
	Data _tmp_make_node(INT, "TempMakeNode", 0);
	Data make_node_string(INT, "MakeNodeStr", 0);
	Data make_node_len(INT, "MakeNodeStrLen", 0);
	Subroutine makeNode = {
		alloc_len = +_node_size,
		Call("Alloc"),
		_tmp_make_node = _node_ptr = +_alloc_new,
		*_tmp_make_node = +_global_no,
		++_tmp_make_node,
		*_tmp_make_node = +make_node_string,
		++_tmp_make_node,
		Call("MakeMap"),
		*_tmp_make_node = +_alloc_new,
		++_tmp_make_node,
		*_tmp_make_node = +make_node_len,
		++_global_no,
	};

	Data map_found(INT, "MapFound", 0);
	Data map_result(INT, "MapResult", 0);
	Data map_result_entry(INT, "MapResEntry", 0);
	Data find_map_map(INT, "FindMapMap", 0);
	Data find_map_char(INT, "FindMapChar", 0);
	Data tmp_find_map(INT, "TempFindMap", 0);
	Data tmp_find_map_ptr(INT, "TempFindMapPtr", 0);
	Subroutine findMap = {
		tmp_find_map_ptr = +find_map_map,
		map_found = Zero,
		++tmp_find_map_ptr,
		For("FindMapLoop", {{tmp_find_map = Zero}, { -_map_max_len + tmp_find_map}, { ++tmp_find_map }}, {
			If({-*tmp_find_map_ptr + find_map_char}, {
				map_result = +tmp_find_map,
				++tmp_find_map_ptr,
				map_result_entry = +*tmp_find_map_ptr,
				map_found = One,
				Break("FindMapLoop")
			}, true),
			++tmp_find_map_ptr,
			++tmp_find_map_ptr,
		}),
	};

	Data children_new(INT, "NewChildren", 0);
	Data insert_res(INT, "InsertRes", 0);
	Data tmp_insert(INT, "TempInsert", 0);
	Data tmp_insert_map(INT, "TempInsertMap", 0);
	Data tmp_insert_str(INT, "TempInsertStr", 0);
	Data tmp_insert_len(INT, "TempInsertLen", 0);
	Data insert_node(INT, "InsertNode", 0);
	Data insert_char(INT, "InsertChar", 0);
	Subroutine insert = {
		tmp_insert = +insert_node,
		++tmp_insert,
		tmp_insert_str = +tmp_insert,
		++tmp_insert,
		tmp_insert_map = +tmp_insert,
		++tmp_insert,
		tmp_insert_len = +tmp_insert,
		find_map_map = +tmp_insert_map,
		find_map_char = +insert_char,
		findMap(),
		insert_res = -map_found,
		++insert_res,
		If({ +map_found }, {
		}, true),
	};

	// Stack
	// Input a char
	Subroutine checkChar = {
		/*
		// Enter -> Start output
		If("CheckCharEnt", { -ascii_ent + IN_tmp }, {
			out_trigger = One,
			+mask_sout,
			"IMK"_asm,
		}, true),
		// [0-9] -> accumulation
		t1 = -ascii_0 + IN_tmp,
		If("CheckCharNum", { MoreEq(dec, t1, t2) }, {
			If("CheckCharNumPosi", { Negative(t1) }, {
				N = N << 1,
				N = (N<<2) + N + t1
			}, true)
		})
		*/
	};

	// Process Trigger
	Subroutine prepareOutput = {
		If({ +out_trigger }, {
			/*
			// Decrement to Prime Number
			out_trigger = Zero,
			While("PrimeSearchLoop", { +N }, {
				checkPrime.stat("CP"),
				If("BreakCPRet", { +CPRet }, {
					Break("PrimeSearchLoop")
				}),
				--N
			}),
			N_bak = +N,

			// Prepare for Output (counting 10^i)
			show_i = Zero,
			Q = +N,
			While("ShowSubDigit", { +Q }, {
				R = +Q,
				X = -dec + Q,
				Q = Zero,
				While("ShowSubSearch", { MoreEq(X, R, t1) }, {
					R = +X,
					X = -dec + X,
					++Q
				}, true),
				++show_i,
			}),
			*/
		})
	};

	// Get Next Digit
	Subroutine getDigitOne = {
	/*	i_shift = One,
		i = +show_i,
		Q = +N,
		If("ReadyState", { +i }, {
			If("AnythingToShow", { +sep_trigger }, {
				sep_trigger = Zero,
				If("DecrNeeded", { MoreEq(N_bak, min_primep1, t1) }, {
					// Kick `--N` Trigger
					N = --N_bak,
					out_trigger = One
				}, Else, {
					+mask_sin,
					"IMK"_asm,
				}),
				+ascii_ent,
				Goto("GetDigitOutT_End")
			}),
			Zero,
			Goto("GetDigitOutT_End")
		}, true),

		While("DispIShift", { --i }, {
			// i_shift * 10
			i_shift = i_shift << 1,
			i_shift = (i_shift<<2) + i_shift,
		}),

		// (N, t2) <- div(N, i_shift)
		X = -i_shift + N,
		Q = Zero,
		R = +N,
		While("ShowSub", { MoreEq(X, R, t1) }, {
			R = +X,
			X = -i_shift + X,
			++Q
		}, true),
		N = +R,
		t2 = +Q,

		--show_i,

		// Kick "\n" Trigger
		If("ReachedEnd", { +show_i }, {
			sep_trigger = One,
		}, true),

		t2 + ascii_0,
		"GetDigitOutT_End,"_asm
		*/
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

	// なるべく最後に宣言
	Data _alloc_ptr(PTR, "AllocPtr", "AllocBuf");
	Data _alloc_buf(INT, "AllocBuf", 0);
	for(int i=500; i --> 0;) Data(INT, "", 0);

	Subroutine alloc = {
		"Alloc, HEX 0"_asm,
		_alloc_new = +_alloc_ptr,
		_alloc_ptr = _alloc_ptr + alloc_len,
		Return("Alloc")
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
		}, true),
		halt,

		alloc.stat(),
		makeString.stat(),
		makeMap.stat(),

		interruptMain.stat("INT_MAIN"),

		Data::stat_all(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
