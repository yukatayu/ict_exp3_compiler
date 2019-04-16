#include <ex3.hpp>
#include <iostream>
#include <exception>

namespace EX3{

	std::vector<Statement> Data::stats_all_;

	std::string Data::name(){
		return name_;
	}

	std::string Data::make_impl(){
		std::string res;
		if(name().empty())
			res += "    ";
		else
			res += name() + ", ";

		switch(type_){
			case DataType::DataType_INT:
				res += "DEC " + std::to_string(dec_);
				break;
			case DataType::DataType_CHAR:
				res += "CHR " + std::string{chr_};
				break;
			case DataType::DataType_PTR:
				res += "SYM " + ref_;
				break;
		}
		return res + "\n";
	}

	Data::Data(DataType_Tag_INT type, std::string name, int data, DataUsageCheck check)
		: name_ { name }
		, type_ { DataType::DataType_INT }
		, dec_ { data }
		, used_ { false }
		, check_ { check }
	{
		stats_all_.push_back(stat());
	}
	Data::Data(DataType_Tag_CHAR type, std::string name, char data, DataUsageCheck check)
		: name_ { name }
		, type_ { DataType::DataType_CHAR }
		, chr_ { data }
		, used_ { false }
		, check_ { check }
	{
		stats_all_.push_back(stat());
	}
	Data::Data(DataType_Tag_PTR type, std::string name, Data data, DataUsageCheck check)
		: name_ { name }
		, type_ { DataType::DataType_PTR }
		, ref_ { data.name() }
		, used_ { false }
		, check_ { check }
	{
		stats_all_.push_back(stat());
	}

	Data::Data(const Data& d){
		type_ = d.type_;
		dec_ = d.dec_;
		chr_ = d.chr_;
		ref_ = d.ref_;
		name_ = d.name_;
		use();
		const_cast<Data&>(d).use();
	}

	Data::Data(Data&& d){
		type_ = d.type_;
		dec_ = d.dec_;
		chr_ = d.chr_;
		ref_ = d.ref_;
		name_ = d.name_;
		use();
		d.use();
	}

	Data::~Data(){
		if(!name_.empty() && !used_ && check_ == DataUsageCheck::EnableCheck){
			// standard error output
			std::cerr << "\e[93m[warning]\e[0mData " << name_ << " has never used" << std::endl;
		}
	}

	Statement Data::stat(){
		// this code below is undesirable due to copy contructor which set `used_` true
		// return Statement{new Data(*this), [](...){}};
		//
		return Const(make());
	}

	Statement Data::stat_all(){
		return Const(
			" / Data Segment\n" + 
			StatementList(stats_all_).stat()->make()
		);
	}

	void Data::use(){
		used_ = true;
	}

	Data Data::operator*(){
		use();
		Data tmp(*this);
		tmp.set_ref();
		return tmp;
	}

	void Data::set_ref(){
		used_ = true;
		name_ += " I";
	}

	Statement Data::operator+(Data d){
		use();
		return Const(
			"LDA " + name_ + "\n"
			"ADD " + d.name() + "\n"
		);
	}

	Statement operator+(Statement st, Data d){
		d.use();
		return Const(
			st->make() +
			"ADD " + d.name() + "\n"
		);
	}

	Statement Data::operator+(){
		use();
		return Const(
			"LDA " + name_ + "\n"
		);
	}

	Statement Data::operator++(){
		use();
		return Const(
			"LDA " + name_ + "\n"
			"INC\n"
			"STA " + name_ + "\n"
		);
	}

	Statement Data::operator--(){
		use();
		return Const(
			"LDA " + name_ + "\n"
			"CMA\n"
			"INC\n"
			"CMA\n"
			"STA " + name_ + "\n"
		);
	}

	Statement Data::operator<<(int i){
		use();
		std::vector<Statement> res;
		res.emplace_back(
			Const(
				"LDA " + name_ + "\n"
			)
		);
		while(i --> 0)
			res.emplace_back(
				Const(
					"CLE\n"
					"CIL\n"
				)
			);

		return Const( StatementList(res).make() );
	}

	Statement Data::operator>>(int i){
		use();
		std::vector<Statement> res;
		res.emplace_back(
			Const(
				"LDA " + name_ + "\n"
			)
		);
		while(i --> 0)
			res.emplace_back(
				Const(
					"CLE\n"
					"CIR\n"
				)
			);

		return Const( StatementList(res).make() );
	}

	Statement Data::operator~(){
		use();
		return Const(
			"LDA " + name_ + "\n"
			"CMA\n"
		);
	}

	Statement Data::operator-(){
		use();
		return Const(
			"LDA " + name_ + "\n"
			"CMA\n"
			"INC\n"
		);
	}

	Statement Data::operator=(Statement st){
		// used_ = true;  // it is not "used" !
		return Const(
			st->make() +
			"STA " + name_ + "\n"
		);
	}

}

