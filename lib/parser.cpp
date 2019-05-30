#include <ex3.hpp>

#include <optional>
#include <string>
#include <vector>

namespace EX3{
	using helper::indent;

	long long int Statement_impl::global_cnt_ = 0;

	// Statement_impl
	std::string Statement_impl::make(){
		std::string res = make_impl();
		if(!res.empty() && res.back() != '\n')
			res += "\n";
		return res;
	}

	std::string Statement_impl::nextAutoLabel() {
		return "_XAUTO" + std::to_string(++global_cnt_);
	}

	// Const
	Const_impl::Const_impl(std::string stat)
		: stat_ (stat)
	{ }
	
	std::string Const_impl::make_impl(){
		return stat_;
	}

	// Goto
	Goto_impl::Goto_impl(std::string label){
		target_name_ = label;
	}
	std::string Goto_impl::make_impl() {
		return "BUN " + target_name_ + "\n";
	}

	// Call
	Call_impl::Call_impl(std::string label){
		target_name_ = label;
	}
	std::string Call_impl::make_impl() {
		return "BSA " + target_name_ + "\n";
	}

	// ACC = (target1 >= target2)
	MoreEq_impl::MoreEq_impl(Data target1, Data target2, Data tmp)
		: target1_(target1)
		, target2_(target2)
		, tmp_(tmp)
	{ }
	std::string MoreEq_impl::make_impl() {
		StatementList cmp12 = {
			// tmp_ <- (target2_ == 0)
			+target2_,
			Const(
				"CLE\n"
				"CME\n"
				"SZA\n"
				"CME\n"
			),
			tmp_ = helper::GetE,
			// acc <- overflow + tmp_
			-target2_ + target1_,
			helper::GetE + tmp_,
		};
		return cmp12.make();
	}

	// Negative
	Negative_impl::Negative_impl(Data d){
		target_name_ = d.name();
	}
	std::string Negative_impl::make_impl() {
		return
			"LDA " + target_name_ + "\n"
			"CIL\n"
			"CLA\n"
			"CIL\n"
		;
	}

	// If
	If_impl::If_impl(std::string name, StatementList stats_cond, StatementList stats, bool invert)
		: name_(name)
		, stats_cond_(stats_cond)
		, stats_(stats)
		, invert_(invert)
		, else_exists_(false)
	{ }

	If_impl::If_impl(std::string name, StatementList stats_cond, StatementList stats, StatementList stats_else, bool invert)
		: name_(name)
		, stats_cond_(stats_cond)
		, stats_(stats)
		, invert_(invert)
		, stats_else_(stats_else)
		, else_exists_(true)
	{ }

	std::string If_impl::make_then_impl() {
		std::string name_label = name_;
		if(name_.empty())
			name_label = Statement_impl::nextAutoLabel();
		std::string res;
		std::string if1 = "_XIF1" + name_label;
		std::string if2 = "_XIF2" + name_label;
		//std::string if3 = "XIF3" + name_label;
		res += "\n";
		if(!name_.empty())
			res += " / > if " + name_ + "\n";
		res += " / if (\n";
		res += indent(stats_cond_.make());
		res += "SZA\n";
		res += "BUN " + (invert_ ? if2 : if1) + "\n";
		res += "BUN " + (invert_ ? if1 : if2) + "\n";
		res += " / ) {\n";
		res += if1 + "," + "\n";
		res += indent(stats_.make());
		res += " / }\n";
		res += if2 + "," + "\n";
		if(!name_.empty())
			res += " / < end of if " + name_ + "\n";
		res += "\n";
		return res;
	}

	std::string If_impl::make_then_else_impl() {
		std::string name_label = name_;
		if(name_.empty())
			name_label = Statement_impl::nextAutoLabel();
		std::string res;
		std::string if1 = "_XIF1" + name_label;
		std::string if2 = "_XIF2" + name_label;
		std::string if3 = "_XIF3" + name_label;
		res += "\n";
		if(!name_.empty())
			res += " / > if " + name_ + "\n";
		res += " / if (\n";
		res += indent(stats_cond_.make());
		res += "SZA\n";
		res += "BUN " + (invert_ ? if2 : if1) + "\n";
		res += "BUN " + (invert_ ? if1 : if2) + "\n";
		res += " / ) {\n";
		res += if1 + "," + "\n";
		res += indent(stats_.make());
		res += "BUN " + if3 + "\n";
		res += " / } else {  // " + name_ + "\n";
		res += if2 + "," + "\n";
		res += indent(stats_else_.make());
		res += " / }\n";
		res += if3 + "," + "\n";
		if(!name_.empty())
			res += " / < end of if " + name_ + "\n";
		res += "\n";
		return res;
	}

	std::string If_impl::make_impl() {
		return (else_exists_ ? make_then_else_impl() : make_then_impl());
	}

	// Break
	Break_impl::Break_impl(std::string name)
		: name_(name)
	{ }

	std::string Break_impl::make_impl(){
		std::string wh3 = "_XWH3" + name_;
		return "BUN " + wh3;
	}
	// Continue
	Continue_impl::Continue_impl(std::string name)
		: name_(name)
	{ }

	std::string Continue_impl::make_impl(){
		std::string wh1 = "_XWH1" + name_;
		return "BUN " + wh1;
	}

	// Return
	Return_impl::Return_impl(std::string target)
		: target_(target)
	{ }

	std::string Return_impl::make_impl(){
		return "BUN " + target_ + " I";
	}

	// While
	While_impl::While_impl(std::string name, StatementList stats_cond, StatementList stats, bool invert)
		: name_(name)
		, stats_cond_(stats_cond)
		, stats_(stats)
		, invert_(invert)
	{ }

	std::string While_impl::make_impl() {
		std::string name_label = name_;
		if(name_.empty())
			name_label = Statement_impl::nextAutoLabel();
		std::string res;
		std::string wh1 = "_XWH1" + name_label;
		std::string wh2 = "_XWH2" + name_label;
		std::string wh3 = "_XWH3" + name_label;
		res += "\n";
		if(!name_.empty())
			res += " / > while loop " + name_ + "\n";
		res += wh1 + "," + "\n";
		res += " / while (\n";
		res += indent(stats_cond_.make());
		res += "SZA\n";
		res += "BUN " + (invert_ ? wh3 : wh2) + "\n";
		res += "BUN " + (invert_ ? wh2 : wh3) + "\n";
		res += " / ) {\n";
		res += wh2 + "," + "\n";
		res += indent(stats_.make());
		res += "BUN " + wh1 + "\n";
		res += " / }\n";
		res += wh3 + "," + "\n";
		if(!name_.empty())
			res += " / < end of while loop " + name_ + "\n";
		res += "\n";
		return res;
	}

	// For
	For_impl::For_impl(std::string name, std::tuple<StatementList, StatementList, StatementList> stats_init_cond_after, StatementList stats, bool invert)
		: name_(name)
		, stats_(stats)
		, invert_(invert)
	{
		std::tie(stats_init_, stats_cond_, stats_after_) = stats_init_cond_after;
	}

	std::string For_impl::make_impl() {
		std::string name_label = name_;
		if(name_.empty())
			name_label = Statement_impl::nextAutoLabel();
		std::string res;
		std::string wh1 = "_XFOR1" + name_label;
		std::string wh2 = "_XFOR2" + name_label;
		std::string wh3 = "_XFOR3" + name_label;
		res += "\n";
		if(!name_.empty())
			res += " / > for loop " + name_ + "\n";
		res += stats_init_.make();
		res += wh1 + "," + "\n";
		res += " / for (\n";
		res += indent(stats_cond_.make());
		res += "SZA\n";
		res += "BUN " + (invert_ ? wh3 : wh2) + "\n";
		res += "BUN " + (invert_ ? wh2 : wh3) + "\n";
		res += " / ) {\n";
		res += wh2 + "," + "\n";
		res += indent(stats_.make());
		res += stats_after_.make();
		res += "BUN " + wh1 + "\n";
		res += " / }\n";
		res += wh3 + "," + "\n";
		if(!name_.empty())
			res += " / < end of for loop " + name_ + "\n";
		res += "\n";
		return res;
	}

	StatementList::StatementList(std::vector<Statement> statements)
		: statements_(statements)
	{ }

	StatementList::StatementList(std::initializer_list<Statement> statements)
		: statements_(statements)
	{ }

	StatementList::StatementList(std::initializer_list<Statement_impl*> statements)
	{
		for(auto s : statements)
			statements_.emplace_back(s);
	}

	std::string StatementList::make(){
		std::string res;
		for(const auto& s : statements_)
			res += s->make();
		return res;
	}

	Statement StatementList::operator()(){
		return stat();
	}

	Statement StatementList::stat(std::string label, std::string placeholder){
		std::string res;
		if(!label.empty())
			res += "\n / > beginning of " + label + "\n"
				+ label + ",\n";
		std::string mk = make();
		if(!placeholder.empty()){
			size_t pos = 0;
			while((pos = mk.find(placeholder, pos)) != std::string::npos){
				mk.replace(pos, placeholder.length(), label);
				pos += placeholder.length();
			}
		}
		res += indent(mk);
		if(!label.empty())
			res += " / < end of " + label + "\n\n";
		return Const(res);
	}

	Statement operator ""_asm(char const * str, std::size_t size){
		return Const(std::string{str});
	}

}
