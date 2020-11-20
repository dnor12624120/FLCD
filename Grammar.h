#pragma once

#include <map>
#include <string>
#include <vector>
#include <fstream>

class Terminal
{
	public:
		Terminal(const std::string& repr) :
			representation{ repr }
		{

		}
	private:
		std::string representation;
};

class NonTerminal
{
	public:
		NonTerminal(const std::string& repr, const std::vector<std::vector<std::string>>& prods) :
			representation{ repr },
			productions{ prods }
		{

		}
	private:
		std::string representation;
		std::vector<std::vector<std::string>> productions;
};

class Grammar
{
	public:
		Grammar(const std::string& filepath)
		{
			loadGrammar(filepath);
		}

		bool isTerminal(const std::string& input)
		{
			return terminals.find(input) != terminals.end();
		}

		bool isNonterminal(const std::string& input)
		{
			return nonterminals.find(input) != nonterminals.end();
		}
	private:
		void loadGrammar(const std::string& filepath)
		{
			std::ifstream inputFile(filepath);
			int numTerminals;
			int numNonterminals;

			inputFile >> numTerminals >> numNonterminals;

			for (int i = 0; i < numTerminals; i++)
			{
				std::string repr;
				inputFile >> repr;
				terminals[repr] = Terminal(repr);
			}

			for (int i = 0; i < numNonterminals; i++)
			{
				std::string repr;
				int numProductions;
				std::vector<std::vector<std::string>> prods;
				inputFile >> repr >> numProductions;
				for (int j = 0; j < numProductions; j++)
				{
					int prodLength;
					std::vector<std::string> prod;
					inputFile >> prodLength;
					for (int k = 0; k < prodLength; k++)
					{
						std::string prodComponent;
						inputFile >> prodComponent;
						prod.emplace_back(prodComponent);
					}
					prodLength;
				}
				nonterminals[repr] = NonTerminal(repr, prods);
			}

		}

		std::map<std::string, Terminal> terminals;
		std::map<std::string, NonTerminal> nonterminals;
};