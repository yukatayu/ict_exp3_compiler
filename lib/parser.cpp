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

	void While::init(std::string name, StatementList stats_cond, StatementList stats, bool invert){
		name_ = name;
		stats_cond_ = stats_cond;
		stats_ = stats;
		invert_ = invert;
	}

	// While
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
		std::string wh1 = "WH1" + name_;
		std::string wh2 = "WH2" + name_;
		std::string wh3 = "WH3" + name_;
		res += " / > while loop " + name_ + "\n";
		res += wh1 + "," + "\n";
		res += " / while (\n";
		res += stats_cond_.make();
		res += "SZA\n";
		res += (invert_ ? wh3 : wh2) + "\n";
		res += (invert_ ? wh2 : wh3) + "\n";
		res += " / ) {\n";
		res += wh2 + "," + "\n";
		res += stats_.make();
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

}
