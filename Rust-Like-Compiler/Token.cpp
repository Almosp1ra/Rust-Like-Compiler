#include "Token.h"

SourceLocation::SourceLocation(size_t column, size_t line)
	: column(column), line(line)
{
}

Token::Token(TokenType type, const string& content, const SourceLocation& location)
	: type(type), content(content), location(location)
{
}

/************************************************
* EqualTo(type)         : 属于某一类，返回 true
************************************************/
bool Token::EqualTo(TokenType type) const
{
	return this->type == type;
}

/************************************************
* StringToTokenType: TokenType 与 String 转换
************************************************/
TokenType StringToTokenType(string content)
{
	auto it = STR_TO_TOKENTYPE.find(content);

	if (it == STR_TO_TOKENTYPE.end())
	{
		return TokenType::NullType;
	}

	return it->second;
}