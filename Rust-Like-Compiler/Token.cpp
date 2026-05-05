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
* EqualTo(type, content): 属于某一类且内容相同，返回 true
************************************************/
bool Token::EqualTo(TokenType type) const
{
	return this->type == type;
}
bool Token::EqualTo(TokenType type, const string& content) const
{
	return (this->type == type && this->content == content);
}