#pragma once

#include <string>

enum class InternalTokenType
{
	INVALID,
	IDENTIFIER,
	CONSTANT,
	KEYWORD,
	SEPARATOR,
	OPERATOR
};

struct InternalToken
{
	std::string repr;
	int cc;
	int cl;
	InternalTokenType type;

	InternalToken() = default;

	InternalToken(const std::string& repr, int cc, int cl) :
		repr{ repr }, cc{ cc }, cl{ cl }, type{ InternalTokenType::INVALID }
	{ }

	InternalToken(const std::string& repr, int cc, int cl, InternalTokenType type) :
		repr{ repr }, cc{ cc }, cl{ cl }, type{ type }
	{ }

	std::string toString()
	{
		return "Representation: " + repr + "\nLine: " + std::to_string(cl) + "\nColumn: " + std::to_string(cc);
	}

	inline bool operator<(const InternalToken& other) const { return (int)type < (int)other.type; }
};