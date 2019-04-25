#include <ex3.hpp>

#include <optional>
#include <string>
#include <vector>

namespace EX3{
	using helper::indent;

	// Statement_impl
	std::string Statement_impl::make(){
		std::string res = make_impl();
		if(!res.empty() && res.back() != '\n')
			res += "\n";
		return res;
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
		std::string res;
		std::string if1 = "XIF1" + name_;
		std::string if2 = "XIF2" + name_;
		//std::string if3 = "XIF3" + name_;
		res += "\n / > if " + name_ + "\n";
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
		res += " / < end of if " + name_ + "\n\n";
		return res;
	}

	std::string If_impl::make_then_else_impl() {
		std::string res;
		std::string if1 = "XIF1" + name_;
		std::string if2 = "XIF2" + name_;
		std::string if3 = "XIF3" + name_;
		res += "\n / > if " + name_ + "\n";
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
		res += " / < end of if " + name_ + "\n\n";
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
		std::string wh3 = "XWH3" + name_;
		return "BUN " + wh3;
	}
	// Continue
	Continue_impl::Continue_impl(std::string name)
		: name_(name)
	{ }

	std::string Continue_impl::make_impl(){
		std::string wh1 = "XWH1" + name_;
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
		std::string res;
		std::string wh1 = "XWH1" + name_;
		std::string wh2 = "XWH2" + name_;
		std::string wh3 = "XWH3" + name_;
		res += "\n / > while loop " + name_ + "\n";
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
		res += " / < end of while loop " + name_ + "\n\n";
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
