#include <string>
#include <vector>
#include <sstream>
#include <istream>

namespace EX3{
	namespace helper{
		std::vector<std::string> split(std::string str, char delim, bool skipEmpty){
			std::vector<std::string> res;
			std::string buf;
			std::stringstream ss{ str };
			while(std::getline(ss, buf, delim))
				if(!skipEmpty || !buf.empty())
					res.push_back(buf);
			return res;
		}

		std::string indent(std::string str, int width){
			std::string tab, res;
			while(width-->0) tab += " ";
			auto sv = split(str, '\n', false);
			for(const auto& s : sv){
				if(!s.empty())
					res += tab;
				res += s + "\n";
			}
			return res;
		}
	}
}

