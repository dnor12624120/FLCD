#pragma once

#include <string>

enum class TokenType
{
	INVALID,
	IDENTIFIER,
	FLOAT_CONST,
	STRING_CONST,
	CHAR_CONST,
	BOOL_CONST,
	INT_CONST
};

class Token
{
	public:
		Token() :
			repr{ "" },
			type{ TokenType::INVALID }
		{

		}

		Token(const std::string& repr, TokenType type) :
			repr{ repr },
			type{ type }
		{

		}

		const std::string& toString() const { return repr; }
		TokenType getType() const { return type; }
	private:
		std::string repr;
		TokenType type;
};