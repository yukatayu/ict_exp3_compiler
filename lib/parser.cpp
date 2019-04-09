#include <parser.hpp>

#include <optional>
#include <string>
#include <vector>
#include <memory>

namespace EX3{


enum class DataType{
	DataType_INT,
	DataType_CHAR,
	DataType_PTR
};

enum DataType_Tag_INT{
	INT
};
enum DataType_Tag_CHAR{
	CHAR
};
enum DataType_Tag_PTR{
	PTR
};

class Data{
private:
	DataType type_;

	int dec_;
	char chr_;
	std::string ref_;

	std::string name_;

public:
	std::string name(){
		return name_;
	}

	std::string make(){
		std::string res = name() + ", ";
		switch(type_){
			case DataType::DataType_INT:
				res += "DEC " + dec_;
				break;
			case DataType::DataType_CHAR:
				res += "CHR " + chr_;
				break;
			case DataType::DataType_PTR:
				res += "SYM " + ref_;
				break;
		}
		return res;
	}

	Data(DataType_Tag_PTR type, std::string name, int data)
		: name_ { name }
		, type_ { DataType::DataType_INT }
		, dec_ { data }
	{
	}
	Data(DataType_Tag_CHAR type, std::string name, char data)
		: name_ { name }
		, type_ { DataType::DataType_CHAR }
		, chr_ { data }
	{
	}
	Data(DataType_Tag_PTR type, std::string name, Data data)
		: name_ { name }
		, type_ { DataType::DataType_PTR }
		, ref_ { data.name() }
	{
	}
};

struct Statement_impl{
	virtual std::string make() = 0;
};

using Statement = std::shared_ptr<Statement_impl>;

struct StatementList {
	std::vector<Statement> statements_;
	StatementList(){}
	StatementList(std::initializer_list<Statement> statements)
		: statements_(statements)
	{
	}
};

struct While{
};


}
