#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "Grammar.h"

class Node
{
	public:
		std::string val;
		Node* child;
		Node* next;
		Node(const std::string& P)
		{
			val = P;
			child = nullptr;
			next = nullptr;
		}
};

class Output 
{
	public:
		Output(const Grammar& grammar, const std::vector<std::string>& _productions):
			grammar{ grammar }
		{
			for (int i = _productions.size() - 1; i >= 0; i--)
			{
				if (_productions[i].find(" -> ", 0) < _productions[i].size())
				{
					productions.emplace_back(_productions[i]);
				}
			}
		}

		static std::vector<std::string> prodSplit(const std::string& prod)
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

		static std::string getLefthand(const std::string& prod)
		{
			std::string delimiter = " -> ";
			return prod.substr(0, prod.find(delimiter));
		}

		static std::vector<std::string> getRighthand(const std::string& prod)
		{
			std::string delimiter = " -> ";
			return prodSplit(prod.substr(prod.find(delimiter) + delimiter.length(), prod.size() - 1));
		}

		bool isNonterminal(const std::string& sym)
		{
			return grammar.getNonterminals().count(sym);
		}

		Node* getRoot() { return root; }

		void buildTree(int level, Node* current)
		{
			if (level < productions.size())
			{
				auto rhs = getRighthand(productions[level]);
				if (level == 0)
				{
					root = new Node(getLefthand(productions[0]));
					current = root;
					current->child = new Node(rhs[level]);
				}
				else
				{
					current->child = new Node(getRighthand(productions[level])[0]);
				}
				current = current->child;
				for (int i = 1; i < rhs.size(); i++)
				{
					current->next = new Node(rhs[i]);
					if (isNonterminal(rhs[i]))
					{
						buildTree(level + 1, current->next);
					}
					current = current->next;
				}
			}
		}

		void print(Node* current)
		{
			if (current)
			{
				std::cout << "Current node: " << current->val << '\n';
				if (current->child)
				{
					std::cout << "Child: " << current->child->val << '\n';
				}
				Node* sibling = current->child;
				if (sibling->next)
				{
					std::cout << "Siblings: ";
					while (sibling->next)
					{
						std::cout << sibling->next->val << ' ';
						sibling = sibling->next;
					}
					std::cout << '\n';
					sibling = current->child;
					while (sibling->next)
					{
						if (isNonterminal(sibling->next->val))
						{
							print(sibling->next);
						}
						sibling = sibling->next;
					}
				}
				
			}
		}
	private:
		Grammar grammar;
		std::vector<std::string> productions;
		Node* root;
};