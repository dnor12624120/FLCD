#pragma once

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <string>
#include <stack>
#include <algorithm>
#include <numeric>
#include <sstream>

#include "HashTable.h"
#include "Grammar.h"

enum class ParserState
{
	STATE_NORMAL,
	STATE_BACK,
	STATE_FINAL,
	STATE_ERROR
};

class Parser
{
	public:
		Parser(const Grammar& grammar, std::vector<std::tuple<std::string, int, int>> pif):
			grammar{ grammar },
			currentPosition{ 0 },
			word{ pif },
			currentState{ ParserState::STATE_NORMAL }
		{

		}

		std::string prodConcat(const std::vector<std::string>& prod)
		{
			std::stringstream ss;
			const int v_size = prod.size();
			for (size_t i = 0; i < v_size; ++i)  
			{
				if (i != 0)
					ss << " ";
				ss << prod[i];
			}
			return ss.str();
		}

		std::vector<std::string> prodSplit(const std::string& prod)
		{
			std::string tmp;
			std::stringstream ss(prod);
			std::vector<std::string> words;

			while (getline(ss, tmp, ' '))
			{
				words.push_back(tmp);
			}
			return words;
		}

		std::vector<std::string> nextProduction(const std::string& nonterminal, const std::string& production)
		{
			auto productions = grammar.getNonterminals()[nonterminal].getProductions();
			for (int i = 0; i < productions.size(); i++)
			{
				if (prodConcat(productions[i]) == production && i < productions.size() - 1)
				{
					return productions[i + 1];
				}
			}
			return std::vector<std::string>();
		}

		bool isWorkStackTopTerminal(const std::string& workTop)
		{
			int index = workTop.find(" -> ");
			auto nonterminals = grammar.getNonterminals();
			return index == -1;
		}

		static std::string getNonterminalFromWorkStack(const std::string& workTop)
		{
			std::string delimiter = " -> ";
			return workTop.substr(0, workTop.find(delimiter));
		}

		static std::string getProductionFromWorkStack(const std::string& workTop)
		{
			std::string delimiter = " -> ";
			return workTop.substr(workTop.find(delimiter) + delimiter.length(), workTop.size() - 1);
		}

		std::vector<std::string> parse()	//long function could be divided into multiple function
		{
			input.push(grammar.getStarting());
			while (currentState != ParserState::STATE_FINAL && currentState != ParserState::STATE_ERROR)
			{
				if (currentState == ParserState::STATE_NORMAL)
				{
					if (input.empty() && currentPosition == word.size())
					{
						currentState = ParserState::STATE_FINAL;
					}
					else if (input.empty() || currentPosition >= word.size())
					{
						currentState = ParserState::STATE_BACK;
					}
					else
					{
						if (grammar.isNonterminal(input.top()))
						{
							std::string nonterminal = input.top();
							auto firstProduction = grammar.getNonterminals()[nonterminal].getProductions()[0];
							work.push(nonterminal + " -> " + prodConcat(firstProduction));
							input.pop();
							for (int i = firstProduction.size() - 1; i >= 0; i--)
							{
								input.push(firstProduction[i]);
							}
							currentProductionSize = firstProduction.size();
						}
						else
						{
							if (input.top() == std::get<0>(word[currentPosition]))
							{
								work.push(std::get<0>(word[currentPosition]));
								currentPosition++;
								input.pop();
								currentProductionSize--;
							}
							else
							{
								currentState = ParserState::STATE_BACK;
								if (!firstWrong)
								{
									errorPosition = word[currentPosition];
									firstWrong = true;
								}
							}
						}
					}
				}
				else
				{
					if (currentState == ParserState::STATE_BACK)
					{
						if (work.empty())
						{
							currentState = ParserState::STATE_ERROR;
						}
						else if (isWorkStackTopTerminal(work.top()))
						{
							currentPosition--;
							std::string terminal = work.top();
							work.pop();
							input.push(terminal);
							currentProductionSize++;
						}
						else
						{
							std::string nonterminal = Parser::getNonterminalFromWorkStack(work.top());
							std::string production = Parser::getProductionFromWorkStack(work.top());
							auto next = nextProduction(nonterminal, production);
							if (!next.empty())
							{
								currentState = ParserState::STATE_NORMAL;
								work.pop();
								work.push(nonterminal + " -> " + prodConcat(next));
								while (currentProductionSize)
								{
									input.pop();
									currentProductionSize--;
								}
								for (int i = next.size() - 1; i >= 0; i--)
								{
									input.push(next[i]);
								}
								currentProductionSize = next.size();
							}
							else
							{
								if (currentPosition == 0 && work.top() == grammar.getStarting())
								{
									currentState == ParserState::STATE_ERROR;
								}
								else
								{
									work.pop();
									input.push(nonterminal);
								}
							}
						}
					}
				}
			}
			if (currentState == ParserState::STATE_ERROR)
			{
				std::cout << "Error parsing " + std::get<0>(errorPosition) + " at line " + std::to_string(std::get<1>(errorPosition)) + ", column " + std::to_string(std::get<2>(errorPosition));
				return std::vector<std::string>();
			}
			std::vector<std::string> output;
			while (!work.empty())
			{
				output.emplace_back(work.top());
				work.pop();
			}
			return output;
		}

	private:
		Grammar grammar;
		std::vector<std::tuple<std::string, int, int>> word;
		HashTable symbolTable;
		int currentPosition;
		int currentProductionSize;
		bool firstWrong = false;
		ParserState currentState;
		std::string currentToken;
		std::tuple<std::string, int, int> errorPosition;
		std::stack<std::string> input;
		std::stack<std::string> work;
};
