#include <ex3.hpp>

#include <optional>
#include <string>
#include <vector>

namespace EX3{

	// Statement_impl
	std::string Statement_impl::make(){
		std::string res = make_impl();
		if(!res.empty() && res.back() != '\n')
			res += "\n";
		return res;
	}

	// Const
	Const::Const(std::string stat)
		: stat_ (stat)
	{ }
	
	std::string Const::make_impl(){
		return stat_;
	}

	// Goto
	Goto::Goto(std::string label){
		target_name_ = label;
	}
	std::string Goto::make_impl() {
		return "BUN " + target_name_ + "\n";
	}

	// Negative
	Negative::Negative(Data d){
		target_name_ = d.name();
	}
	std::string Negative::make_impl() {
		return
			"LDA " + target_name_ + "\n"
			"CIL\n"
			"CLA\n"
			"CIL\n"
		;
	}

	// If
	void If::init(std::string name, StatementList stats_cond, StatementList stats, bool invert){
		name_ = name;
		stats_cond_ = stats_cond;
		stats_ = stats;
		invert_ = invert;
	}

	If::If(std::string name, StatementList stats_cond, StatementList stats, bool invert){
		init(name, stats_cond, stats, invert);
	}
	If::If(std::string name, std::initializer_list<Statement> stats_cond, StatementList stats, bool invert){
		init(name, stats_cond, stats, invert);
	}
	If::If(std::string name, StatementList stats_cond, std::initializer_list<Statement> stats, bool invert){
		init(name, stats_cond, stats, invert);
	}
	If::If(std::string name, std::initializer_list<Statement> stats_cond, std::initializer_list<Statement> stats, bool invert){
		init(name, stats_cond, stats, invert);
	}

	//TODO: implement Else
	std::string If::make_impl() {
		std::string res;
		std::string if1 = "XIF1" + name_;
		std::string if2 = "XIF2" + name_;
		//std::string if3 = "XIF3" + name_;
		res += " / > if " + name_ + "\n";
		res += " / if (\n";
		res += stats_cond_.make();
		res += "SZA\n";
		res += "BUN " + (invert_ ? if2 : if1) + "\n";
		res += "BUN " + (invert_ ? if1 : if2) + "\n";
		res += " / ) {\n";
		res += if1 + "," + "\n";
		res += stats_.make();
		res += " / }\n";
		res += if2 + "," + "\n";
		res += " / < end of if " + name_ + "\n";
		return res;
	}

	// While
	void While::init(std::string name, StatementList stats_cond, StatementList stats, bool invert){
		name_ = name;
		stats_cond_ = stats_cond;
		stats_ = stats;
		invert_ = invert;
	}

	While::While(std::string name, StatementList stats_cond, StatementList stats, bool invert){
		init(name, stats_cond, stats, invert);
	}
	While::While(std::string name, std::initializer_list<Statement> stats_cond, StatementList stats, bool invert){
		init(name, stats_cond, stats, invert);
	}
	While::While(std::string name, StatementList stats_cond, std::initializer_list<Statement> stats, bool invert){
		init(name, stats_cond, stats, invert);
	}
	While::While(std::string name, std::initializer_list<Statement> stats_cond, std::initializer_list<Statement> stats, bool invert){
		init(name, stats_cond, stats, invert);
	}

	std::string While::make_impl() {
		std::string res;
		std::string wh1 = "XWH1" + name_;
		std::string wh2 = "XWH2" + name_;
		std::string wh3 = "XWH3" + name_;
		res += " / > while loop " + name_ + "\n";
		res += wh1 + "," + "\n";
		res += " / while (\n";
		res += stats_cond_.make();
		res += "SZA\n";
		res += "BUN " + (invert_ ? wh3 : wh2) + "\n";
		res += "BUN " + (invert_ ? wh2 : wh3) + "\n";
		res += " / ) {\n";
		res += wh2 + "," + "\n";
		res += stats_.make();
		res += "BUN " + wh1 + "\n";
		res += " / }\n";
		res += wh3 + "," + "\n";
		res += " / < end of while loop " + name_ + "\n";
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

	Statement StatementList::stat(std::string label){
		std::string res;
		if(!label.empty())
			res += " / > beginning of " + label + "\n"
				+ label + ",\n";
		res += make();
		if(!label.empty())
			res += " / < end of " + label + "\n";
		return Statement{
			new Const(res)
		};
	}

}
