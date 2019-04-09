#include <ex3.hpp>
#include <iostream>

namespace EX3{

	std::string Data::name(){
		return name_;
	}

	std::string Data::make_impl(){
		std::string res = name() + ", ";
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

	Data::Data(DataType_Tag_INT type, std::string name, int data)
		: name_ { name }
		, type_ { DataType::DataType_INT }
		, dec_ { data }
	{
	}
	Data::Data(DataType_Tag_CHAR type, std::string name, char data)
		: name_ { name }
		, type_ { DataType::DataType_CHAR }
		, chr_ { data }
	{
	}
	Data::Data(DataType_Tag_PTR type, std::string name, Data data)
		: name_ { name }
		, type_ { DataType::DataType_PTR }
		, ref_ { data.name() }
	{
	}

	Statement Data::stat(){
		return Statement{new Data(*this), [](...){}};
	}

	Data Data::operator*(){
		Data tmp(*this);
		tmp.set_ref();
		return tmp;
	}

	void Data::set_ref(){
		name_ += " I";
	}

	Statement Data::operator+(Data d){
		return Statement{
			new Const{
				"LDA " + name_ + "\n"
				"ADD " + d.name() + "\n"
				"INC\n"
			}
		};
	}

	Statement Data::operator+(){
		return Statement{
			new Const{
				"LDA " + name_ + "\n"
			}
		};
	}

	Statement Data::operator++(){
		return Statement{
			new Const{
				"LDA " + name_ + "\n"
				"INC\n"
				"STA " + name_ + "\n"
			}
		};
	}

	Statement Data::operator-(){
		return Statement{
			new Const{
				"LDA " + name_ + "\n"
				"CMA\n"
				"INC\n"
			}
		};
	}

	Statement Data::operator=(Statement st){
		return Statement{
			new Const{
				st->make() +
				"STA " + name_ + "\n"
			}
		};
	}

}

