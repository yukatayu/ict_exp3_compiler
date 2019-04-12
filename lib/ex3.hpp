#pragma once
#include <string>
#include <memory>
#include <vector>

namespace EX3{

	enum class DataType{
		DataType_INT,
		DataType_CHAR,
		DataType_PTR
	};

	enum DataUsageCheck{
		EnableCheck,
		MaybeUnused
	};

	enum DataType_Tag_INT{
		INT = 10
	};
	enum DataType_Tag_CHAR{
		CHAR = 11
	};
	enum DataType_Tag_PTR{
		PTR = 12
	};

	class Statement_impl{
	public:
		virtual std::string make_impl() = 0;
		std::string make();
	};

	using Statement = std::shared_ptr<Statement_impl>;
	using Box = Statement;

	class Data : public Statement_impl{
	private:
		DataType type_;

		int dec_;
		char chr_;
		bool used_;
		DataUsageCheck check_;
		std::string ref_;

		std::string name_;
		void set_ref();

		static std::vector<Statement> stats_all_;

	public:
		void use();
		std::string name();
		std::string make_impl() override;

		Data(const Data&);
		Data(Data&&);
		~Data();
		Data(DataType_Tag_INT type, std::string name, int data, DataUsageCheck check = EnableCheck);
		Data(DataType_Tag_CHAR type, std::string name, char data, DataUsageCheck check = EnableCheck);
		Data(DataType_Tag_PTR type, std::string name, Data data, DataUsageCheck check = EnableCheck);
		Statement stat();
		Statement load();
		Data operator*();
		Statement operator+(Data d);
		Statement operator+();
		Statement operator++();	// 前置
		Statement operator--();	// 前置
		Statement operator~();
		Statement operator-();
		Statement operator<<(int i);
		Statement operator>>(int i);
		Statement operator=(Statement);
		static Statement stat_all();
	};

	Statement operator+(Statement st, Data d);


	class StatementList {
	private:
		std::vector<Statement> statements_;
	public:
		StatementList() = default;
		StatementList(std::initializer_list<Statement> statements);
		StatementList(std::initializer_list<Statement_impl*> statements);
		StatementList(std::vector<Statement> statements);
		std::string make();
		Statement stat(std::string label = "", std::string placeholder = "");
	};

	class Const : public Statement_impl {
	private:
		std::string stat_;
	public:
		Const(std::string stat);
		std::string make_impl() override;
	};

	class Goto : public Statement_impl {
	private:
		std::string target_name_;
	public:
		Goto(std::string label);
		std::string make_impl() override;
	};

	// ACC = (target1 >= target2)
	class MoreEq : public Statement_impl {
	private:
		Data tmp_;
		Data target1_;
		Data target2_;
	public:
		MoreEq(Data target1, Data target2, Data tmp);
		std::string make_impl() override;
	};

	class Negative : public Statement_impl {
	private:
		std::string target_name_;
	public:
		Negative(Data d);
		std::string make_impl() override;
	};

	class While : public Statement_impl {
	private:
		std::string name_;
		StatementList stats_cond_;
		StatementList stats_;
		bool invert_;
		void init(std::string name, StatementList stats_cond, StatementList stats, bool invert);
	public:
		While(std::string name, StatementList stats_cond, StatementList stats, bool invert = false);
		While(std::string name, std::initializer_list<Statement> stats_cond, StatementList stats, bool invert = false);
		While(std::string name, StatementList stats_cond, std::initializer_list<Statement> stats, bool invert = false);
		While(std::string name, std::initializer_list<Statement> stats_cond, std::initializer_list<Statement> stats, bool invert = false);
		std::string make_impl() override;
	};

	class If : public Statement_impl {
	private:
		std::string name_;
		StatementList stats_cond_;
		StatementList stats_;
		bool invert_;
		void init(std::string name, StatementList stats_cond, StatementList stats, bool invert);
	public:
		If(std::string name, StatementList stats_cond, StatementList stats, bool invert = false);
		If(std::string name, std::initializer_list<Statement> stats_cond, StatementList stats, bool invert = false);
		If(std::string name, StatementList stats_cond, std::initializer_list<Statement> stats, bool invert = false);
		If(std::string name, std::initializer_list<Statement> stats_cond, std::initializer_list<Statement> stats, bool invert = false);
		std::string make_impl() override;
	};

	namespace helper{
		std::vector<std::string> split(std::string str, char delim, bool skipEmpty = false);
		std::string indent(std::string str, int width = 4);

		static Statement begin { new Const("ORG 10 / Entry Point") };
		static Statement halt  { new Const("_M_, HLT") };
		static Statement end   { new Const("END") };
		static Statement Zero{
			new Const(
				"CLA\n"
			)
		};

		static Statement One{
			new Const(
				"CLA\n"
				"INC\n"
			)
		};

		static Statement GetE{
			new Const(
				"CLA\n"
				"CIL\n"
			)
		};
	}

}
