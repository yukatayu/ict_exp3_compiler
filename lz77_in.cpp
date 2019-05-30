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
	Data ascii_0(CHAR, "ASCII0", '0');

	Data t1(INT, "TEMP1", 0);
	Data t2(INT, "TEMP2", 0);
	Data t_debug1(CHAR, "TEMPDEBUG1", 'T');
	Data t_debug2(CHAR, "TEMPDEBUG2", 'e');
	Data t_debug3(CHAR, "TEMPDEBUG3", 's');
	Data t_debug4(CHAR, "TEMPDEBUG4", 't');
	Data t_debug5(CHAR, "TEMPDEBUG5", '!');

	Data t_queue_1(INT, "TEMPQUEUE1", 0);
	Data t_print_1(INT, "TEMPPRINT1", 0);
	Data t_print_2(INT, "TEMPPRINT2", 0);
	Data t_print_3(INT, "TEMPPRINT3", 0);
	Data t_print_4(INT, "TEMPPRINT4", 0);

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

	Subroutine _call_makeString = {
		"MakeString, HEX 0"_asm,
		alloc_len = ++make_string_len,
		Call("Alloc"),
		Return("MakeString")
	};
	Subroutine _call_makeMap = {
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
	" // [Debug] New Node!! Created at: "_asm, "CLE"_asm, "_B_,"_asm,
		*_tmp_make_node = +_global_no,
	" // [Debug] ID: "_asm, +*_tmp_make_node, "_B_,"_asm,
		++_tmp_make_node,
		*_tmp_make_node = +make_node_string,
	" // [Debug] string: "_asm, +*_tmp_make_node, "_B_,"_asm,
		++_tmp_make_node,
		Call("MakeMap"),
		*_tmp_make_node = +_alloc_new,
	" // [Debug] map: "_asm, +*_tmp_make_node, "_B_,"_asm,
		++_tmp_make_node,
		*_tmp_make_node = +make_node_len,
	" // [Debug] len: "_asm, +*_tmp_make_node, "_B_,"_asm,
		++_global_no,
	};

	Data map_found(INT, "MapFound", 0);
	Data map_result(INT, "MapResult", 0);
	Data map_result_entity(INT, "MapResEntry", 0);
	Data find_map_map(INT, "FindMapMap", 0);
	Data find_map_char(INT, "FindMapChar", 0);
	Data tmp_find_map(INT, "TempFindMap", 0);
	Data tmp_find_map_ptr(INT, "TempFindMapPtr", 0);
	Subroutine findMap = {
		tmp_find_map_ptr = +find_map_map,
		map_found = Zero,
		++tmp_find_map_ptr,
		For("FindMapLoop", {{tmp_find_map = Zero}, { -_map_max_len + tmp_find_map}, { ++tmp_find_map }}, {
		" // [Debug] Searching: "_asm, +tmp_find_map_ptr, "_B_,"_asm,
			If({-*tmp_find_map_ptr + find_map_char}, {
				map_result = +tmp_find_map,
				++tmp_find_map_ptr,
				map_result_entity = +*tmp_find_map_ptr,
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
	Data tmp_insert_new_str(INT, "TempInsertNewStr", 0);
	Data tmp_insert_ptr_src(INT, "TempInsertPtrSrc", 0);
	Data tmp_insert_ptr_dst(INT, "TempInsertPtrDst", 0);
	Data tmp_insert_len(INT, "TempInsertLen", 0);
	Data insert_node(INT, "InsertNode", 0);
	Data insert_char(INT, "InsertChar", 0);
	Subroutine insert = {
		tmp_insert = +insert_node,
	//" // tmp_insert = +insert_node,"_asm, +tmp_insert,
	//"_B_,"_asm,
	// Debug1 " // node no = AC,"_asm, +*tmp_insert, "_B_,"_asm,
		++tmp_insert,
		tmp_insert_str = +*tmp_insert,
	//" // tmp_insert_str = +*tmp_insert,"_asm, +tmp_insert_str,
	//"_B_,"_asm,
		++tmp_insert,
		tmp_insert_map = +*tmp_insert,
	//" // tmp_insert_map = +*tmp_insert,"_asm, +tmp_insert_map,
	//"_B_,"_asm,
		++tmp_insert,
		tmp_insert_len = +*tmp_insert,
	//" // tmp_insert_len = +*tmp_insert,"_asm, +tmp_insert_len,
	//"_B_,"_asm,
		find_map_map = +tmp_insert_map,
		find_map_char = +insert_char,
		findMap(),
		insert_res = -map_found,
		++insert_res,
		If({ +map_found }, {
				" // [Debug] Inserting"_asm, "CLE"_asm, "_B_,"_asm,
				//tmp_insert = +*find_map_map << 1,
				tmp_insert = +*tmp_insert_map,
				++*tmp_insert_map,
				++tmp_insert_map,
				tmp_insert = tmp_insert << 1,
				tmp_insert_map = tmp_insert_map + tmp_insert,
				//++*find_map_map,
				//++find_map_map,
				//find_map_map = find_map_map + tmp_insert,
				//make_string_len = ++tmp_insert_len,
				make_string_len = ++tmp_insert_len,
				Call("MakeString"),
				tmp_insert_new_str = +_alloc_new,
				For({
						{ tmp_insert_ptr_src = +tmp_insert_str, ++tmp_insert_ptr_dst = +tmp_insert_new_str },
						{ +*tmp_insert_ptr_src },
						{ ++tmp_insert_ptr_src, ++tmp_insert_ptr_dst }
				}, {
					*tmp_insert_ptr_dst = +*tmp_insert_ptr_src,
					" // [Debug] New Str: AC"_asm, "CLE"_asm, "_B_,"_asm,
				}),
				*tmp_insert_ptr_dst = +insert_char,
				make_node_string = +tmp_insert_new_str,
				make_node_len = +make_string_len,
				makeNode(),
				children_new = +_node_ptr,
				*tmp_insert_map = +insert_char,
				++tmp_insert_map,
				*tmp_insert_map = +children_new,
		}, true),
	};

	// Stack
	// Input a char
	Data main_root(INT, "MainRoot", 0);
	Data main_p(INT, "MainPtr", 0);
	Subroutine checkChar = {
		// Enter -> Start output
		If("CheckCharEnt", { -ascii_ent + IN_tmp }, {
			calc_trigger = One,
			+mask_sout,
			"IMK"_asm,
		}, Else, {
			//*raw_str_ptr = +IN_tmp,
			//++raw_str_ptr,
			//*raw_str_ptr = Zero,

			insert_node = +main_p,
			insert_char = +IN_tmp,
			insert(),
			//"_B_,"_asm,
			Zero,
			If({+insert_res}, {
				" // [Debug] Inserted"_asm, "CLE"_asm, "_B_,"_asm,
				" // [Debug] Now Head is "_asm, +main_p, "_B_,"_asm,
				t1 = +*main_p + ascii_0,
				print(t1),
				print(IN_tmp),
				main_p = +main_root,
			}, Else, {
				" // [Debug] Found"_asm, "CLE"_asm, "_B_,"_asm,
				" // [Debug] Now Head is "_asm, +main_p, "_B_,"_asm,
				main_p = +map_result_entity,
				" // [Debug] Now Head is "_asm, +map_result_entity, "_B_,"_asm,
			}),
		}, true),
	};

	// Process Trigger
	Subroutine prepareOutput = {
		If({ +calc_trigger }, {
			calc_trigger = Zero,

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
					// allow input
					// +mask_sin,
					// "IMK"_asm,
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

	// なるべく最後に宣言
	Data _alloc_ptr(PTR, "AllocPtr", "AllocBuf");
	Data _alloc_buf(INT, "AllocBuf", 0);
	for(int i=500; i --> 0;) Data(INT, "", 0);

	Subroutine _call_alloc = {
		"Alloc, HEX 0"_asm,
		//"_B_,"_asm,
	//" // [Debug] Allocated: bytes:  "_asm, +alloc_len, "_B_,"_asm,
	//" // [Debug] Allocated: to   :  "_asm, +_alloc_ptr, "_B_,"_asm,
		_alloc_new = +_alloc_ptr,
		_alloc_ptr = _alloc_ptr + alloc_len,
		Return("Alloc")
	};

	// Main Program
	Subroutine program = {
		begin_interrupt("INT_MAIN", "INT_RET"),
		begin,

		make_node_len = Zero,
		make_string_len = Zero,
		Call("MakeString"),
		make_node_string = +_alloc_new,
		makeNode(),
		main_p = main_root = +_node_ptr,

		+mask_sin,
		"IMK"_asm,
		"SIO"_asm,
		"ION"_asm,	// enable interrupt

		While("MainLoop", { +halt_trigger }, {
		}, true),
		halt,

		_call_alloc.stat(),
		_call_makeString.stat(),
		_call_makeMap.stat(),

		interruptMain.stat("INT_MAIN"),

		Data::stat_all(),
		end
	};

	std::string res = program.make();
	std::cout << res << std::endl;
}
