#pragma once

#include <map>
#include <iostream>
#include <regex>
#include <exception>

#include "Token.h"
#include "HashTable.h"
#include "FiniteAutomaton.h"


class Scanner
{
	using ch = std::string::const_iterator;

	public:
		Scanner(const std::map<std::string, int>& tokens) :
			currentLine{ 0 },
			currentColumn{ 0 },
			tokens{ tokens },
			integerFA{ "integer.aut" },
			identifierFA{ "identifier.aut" }
		{

		}

		std::vector<std::pair<int, std::tuple<int, int, int>>> getPIF() { return pif; }
		HashTable getST() { return st; }

		void scan(const std::string& program)
		{
			auto tks = splitByWhitespace(program);
			std::vector<InternalToken> splitTokens;
			for (auto& t : tks)
			{
				auto str = splitBySeparatorOperator(t);
				int n = 0;
				for (auto& s : str)
				{
					int cc = t.cc + n;
					splitTokens.emplace_back(InternalToken(s, cc, t.cl));
					n += s.length();
				}
			}
			classifyTokens(splitTokens);
			disambiguateTokens(splitTokens);
			insertTokens(splitTokens);
		}

		std::string pifToString() const
		{
			std::string content;
			std::for_each(pif.begin(), pif.end(), [&](const auto& p) 
				{
					content += "Token type: " + std::to_string(p.first) +
						" Code: " + std::to_string(std::get<0>(p.second)) + "\n" +
						" Line: " + std::to_string(std::get<1>(p.second)) + "\n" +
						" Column: " + std::to_string(std::get<2>(p.second)) + "\n";
				});
			return "PIF:\n" + content;
		}

		std::string stToString() const
		{
			return "ST:\n" + st.toString();
		}

	private:
		void disambiguateTokens(std::vector<InternalToken>& tks)
		{
			for (int i = 0; i < tks.size(); i++)
			{
				// if an operator is a sign, modify the subsequent token to include the sign
				if (i > 0 && i < tks.size() - 1 &&                   // verify that we're not out of bounds
					(tks[i].repr == "+" || tks[i].repr == "-") &&    // check if current token could be a sign
					tks[i - 1].type == InternalTokenType::OPERATOR)  // and that the previous token is also an operator
				{
					tks[i + 1].repr = tks[i].repr + tks[i + 1].repr;
					tks.erase(tks.begin() + i, tks.begin() + i + 1);
					i--;
				}
				// if the preceding token is not an identifier or a constant, the following one is,
				// and the current one could be a sign, then it is a sign
				else if (i == 0 && i < tks.size() - 1)
				{
					if (tks[i].repr == "+" || tks[i].repr == "-")
					{
						if (tks[i + 1].type == InternalTokenType::IDENTIFIER || tks[i + 1].type == InternalTokenType::CONSTANT)
						{
							tks[i + 1].repr = tks[i].repr + tks[i + 1].repr;
							tks.erase(tks.begin() + i, tks.begin() + i + 1);
							i--;
						}
					}
				}
				else if (i > 0 && i < tks.size() - 1)
				{
					if (tks[i - 1].type != InternalTokenType::IDENTIFIER || tks[i - 1].type != InternalTokenType::CONSTANT)
					{
						if ((tks[i].repr == "+" || tks[i].repr == "-"))
						{
							if (tks[i + 1].type == InternalTokenType::IDENTIFIER || tks[i + 1].type == InternalTokenType::CONSTANT)
							{
								tks[i + 1].repr = tks[i].repr + tks[i + 1].repr;
								tks.erase(tks.begin() + i, tks.begin() + i + 1);
								i--;
							}
						}
					}
				}
				// if theres 2 constants separated by a comma, then its a float
				if (tks[i].repr == "." && i > 0 && i < tks.size() - 1)
				{
					if (tks[i - 1].type == InternalTokenType::CONSTANT)
					{
						if (tks[i + 1].type == InternalTokenType::CONSTANT)
						{
							tks[i + 1].repr = tks[i - 1].repr + tks[i].repr + tks[i + 1].repr;
							tks.erase(tks.begin() + i - 1, tks.begin() + i + 1);
							i--;
						}
					}
				}

			}
		}

		void insertTokens(std::vector<InternalToken>& tks)
		{
			for (auto& t : tks)
			{
				if (t.type == InternalTokenType::IDENTIFIER)
				{
					st.add(t);
					pif.emplace_back(std::pair<int, std::tuple<int, int, int>>(0, std::make_tuple(st.get(t), t.cl, t.cc)));
				}
				else if (t.type == InternalTokenType::CONSTANT)
				{
					st.add(t);
					pif.emplace_back(std::pair<int, std::tuple<int, int, int>>(1, std::make_tuple(st.get(t), t.cl, t.cc)));
				}
				else
				{
					pif.emplace_back(std::pair<int, std::tuple<int, int, int>>(-1, std::make_tuple(tokens[t.repr], t.cl, t.cc)));
				}
			}
		}

		void classifyTokens(std::vector<InternalToken>& tks)
		{
			for (auto& t : tks)
			{
				t.type = classifyToken(t);
				if (t.type == InternalTokenType::INVALID)
				{
					throw std::runtime_error("Invalid token at line " + std::to_string(t.cl + 1) + " column " + std::to_string(t.cc + 1));
				}
			}
		}

		std::vector<InternalToken> splitByWhitespace(const std::string& str)
		{
			std::vector<InternalToken> tokens;
			auto c = str.begin();
			while (c != str.end())
			{
				c = skipComment(c);
				while (c != str.end() && isWhitespace(c))
				{
					advance(c);
				}
				c = skipComment(c);
				if (c != str.end() && !isWhitespace(c))
				{
					std::string token;
					int cl = currentLine;
					int cc = currentColumn;
					while (c != str.end() && !isWhitespace(c))
					{
						token.push_back(*c);
						advance(c);
					}
					tokens.emplace_back(InternalToken{ token, cc, cl });
				}
			}
			return tokens;
		}

		InternalTokenType classifyToken(const InternalToken& token)
		{
			if (token.repr.length() > 32)
			{
				return InternalTokenType::INVALID;
			}
			if (isTokenKeyword(token.repr))
			{
				return InternalTokenType::KEYWORD;
			}
			if (isTokenSeparator(token.repr))
			{
				return InternalTokenType::SEPARATOR;
			}
			if (isTokenOperator(token.repr))
			{
				return InternalTokenType::OPERATOR;
			}
			if (isTokenIdentifier(token.repr))
			{
				return InternalTokenType::IDENTIFIER;
			}
			if (isTokenConstant(token.repr))
			{
				return InternalTokenType::CONSTANT;
			}
			return InternalTokenType::INVALID;
		}

		std::vector<std::string> splitBySeparatorOperator(InternalToken& token)
		{
			std::vector<std::string> tokens;
			auto c = token.repr.begin();
			int cc = token.cc;
			int last = cc;
			while (c != token.repr.end())
			{
				std::string prev;
				while (c != token.repr.end() && !isSeparator(c) && isOperator(c, token.repr.end()) == "")
				{
					prev.push_back(*c);
					advance(c);
				}
				if (!prev.empty())
				{
					tokens.emplace_back(prev);
				}
				if (c != token.repr.end() && isSeparator(c))
				{
					std::string sep;
					sep.push_back(*c);
					tokens.emplace_back(std::string(sep));
					advance(c);
				}
				if (c != token.repr.end())
				{
					std::string op = isOperator(c, token.repr.end());
					if (!op.empty())
					{
						tokens.emplace_back(op);
						for (int i = 0; i < op.length(); i++)
						{
							advance(c);
						}
					}
				}
			}
			return tokens;
		}

		void advance(ch& c)
		{
			if (*c == '\n')
			{
				currentLine++;
				currentColumn = 0;
			}
			else
			{
				currentColumn++;
			}
			c++;
		}

		bool isSeparator(ch c)
		{
			return *c == '{' || *c == '}' || *c == '(' || *c == ')' || *c == '[' || *c == ']' || *c == ';' || *c == ',';
		}

		std::string isOperator(ch c, ch end)
		{
			if (c + 1 != end && *c == '+' && *(c + 1) == '=')
			{
				return "+=";
			}
			if (c + 1 != end && *c == '-' && *(c + 1) == '=')
			{
				return "-=";
			}
			if (c + 1 != end && *c == '*' && *(c + 1) == '=')
			{
				return "*=";
			}
			if (c + 1 != end && *c == '/' && *(c + 1) == '=')
			{
				return "/=";
			}
			if (c + 1 != end && *c == '%' && *(c + 1) == '=')
			{
				return "%=";
			}
			if (c + 1 != end && *c == '<' && *(c + 1) == '=')
			{
				return "<=";
			}
			if (c + 1 != end && *c == '>' && *(c + 1) == '=')
			{
				return ">=";
			}
			if (c + 1 != end && *c == '!' && *(c + 1) == '=')
			{
				return "!=";
			}
			if (c + 1 != end && *c == '=' && *(c + 1) == '=')
			{
				return "==";
			}
			if (c + 1 != end && *c == '&' && *(c + 1) == '&')
			{
				return "&&";
			}
			if (c + 1 != end && *c == '|' && *(c + 1) == '|')
			{
				return "||";
			}
			if (*c == '=')
			{
				return "=";
			}
			if (*c == '+')
			{
				return "+";
			}
			if (*c == '-')
			{
				return "-";
			}
			if (*c == '*')
			{
				return "*";
			}
			if (*c == '/')
			{
				return "/";
			}
			if (*c == '%')
			{
				return "%";
			}
			if (*c == '.')
			{
				return ".";
			}
			if (*c == '!')
			{
				return "!";
			}
			if (*c == '<')
			{
				return "<";
			}
			if (*c == '>')
			{
				return ">";
			}
			if (*c == '?')
			{
				return "?";
			}
			if (*c == ':')
			{
				return ":";
			}
			return "";
		}


		bool isWhitespace(ch c)
		{
			return *c == ' ' || *c == '\n' || *c == '\t';
		}

		bool isTokenKeyword(const std::string& tok)
		{
			return tok == "sizeof" ||
				tok == "const" ||
				tok == "float" ||
				tok == "char" ||
				tok == "string" ||
				tok == "bool" ||
				tok == "int" ||
				tok == "struct" ||
				tok == "if" ||
				tok == "else" ||
				tok == "while" ||
				tok == "for" ||
				tok == "true" ||
				tok == "false" ||
				tok == "readFloat" ||
				tok == "readInt" ||
				tok == "readChar" ||
				tok == "readBool" ||
				tok == "readString" ||
				tok == "printFloat" ||
				tok == "printInt" ||
				tok == "printChar" ||
				tok == "printBool" ||
				tok == "printString";
		}

		bool isTokenSeparator(const std::string& tok)
		{
			return tok == "[" ||
				tok == "]" ||
				tok == "(" ||
				tok == ")" ||
				tok == "{" ||
				tok == "}" ||
				tok == ";" ||
				tok == ",";
		}

		bool isTokenOperator(const std::string& tok)
		{
			return tok == "+" ||
				tok == "-" ||
				tok == "*" ||
				tok == "/" ||
				tok == "%" ||
				tok == "+=" ||
				tok == "-=" ||
				tok == "*=" ||
				tok == "/=" ||
				tok == "%=" ||
				tok == "<" ||
				tok == ">" ||
				tok == "<=" ||
				tok == ">=" ||
				tok == "==" ||
				tok == "!=" ||
				tok == "!" ||
				tok == "&&" ||
				tok == "==" ||
				tok == "?" ||
				tok == ":" ||
				tok == "." || 
				tok == "=";
		}

		bool isTokenIdentifier(const std::string& tok)
		{
			return identifierFA.accepted(tok);
		}

		bool isTokenConstant(const std::string& tok)
		{
			return integerFA.accepted(tok) || 
				std::regex_match(tok, std::regex("^[+-]?([0-9]*[.])?[0-9]+$|^\'.\'$|^\".*\"$", std::regex_constants::ECMAScript));

		}

		ch skipComment(ch c)
		{
			//check for single line comment
			if (*c == '/' && *(c + 1) == '/')
			{
				while (*c != '\n')
				{
					advance(c);
				}
			}
			return c;
		}

		FiniteAutomaton identifierFA;
		FiniteAutomaton integerFA;
		std::vector<std::pair<int, std::tuple<int, int, int>>> pif;
		HashTable st;
		std::map<std::string, int> tokens;
		int currentLine;
		int currentColumn;
};