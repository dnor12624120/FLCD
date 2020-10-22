#include <iostream>
#include <utility>

#include "HashTable.h"

int main()
{
	HashTable ht;
	ht.add(Token("a", TokenType::IDENTIFIER), 2);
	ht.add(Token("b", TokenType::IDENTIFIER), 3);
	ht.add(Token("c", TokenType::IDENTIFIER), 3);
	ht.add(Token("d", TokenType::IDENTIFIER), 3);
	ht.add(Token("e", TokenType::IDENTIFIER), 3);
	ht.add(Token("f", TokenType::IDENTIFIER), 3);
	ht.add(Token("g", TokenType::IDENTIFIER), 3);
	ht.add(Token("h", TokenType::IDENTIFIER), 3);
	ht.add(Token("i", TokenType::IDENTIFIER), 3);
	ht.add(Token("j", TokenType::IDENTIFIER), 113);
	ht.add(Token("k", TokenType::IDENTIFIER), 3);
	ht.add(Token("afzxcvxcvzxg", TokenType::IDENTIFIER), 3);
	ht.add(Token("a", TokenType::IDENTIFIER), 1111);
	ht.add(Token("e", TokenType::IDENTIFIER), 1111555);
	std::cout << ht.get(Token("j", TokenType::IDENTIFIER)) << '\n';
	std::cout << ht.toString() << '\n';
	return 0;
}