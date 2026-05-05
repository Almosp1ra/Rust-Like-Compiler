#pragma once
#include <string>
#include <unordered_set>
using namespace std;

/* 返回的终止符 */
const char SOURCE_END_CHAR = '\0';

/* 关键词列表 */
const unordered_set<string> KEYWORDS = {
	"i32", "let", "if", "else", "while", "return", "mut", "fn",
	"for", "in", "loop", "break", "continue"
};

/* token 类别枚举 */
enum class TokenType {
	NullType,
	Keyword,			// i32 | let | if | else | while | return | mut | fn | for | in | loop | break | continue
	Identifier,			// (字母|_)(字母|数字|_)
	Value,				// 数字(数字)*
	AssignmentOperator,	// =
	Operator,			// + | - | * | / | == | > | >= | < | <= | != | & | *
	Delimiter,			// ( | ) | { | } | [ | ]
	Separator,			// ; | : | ,
	Special,			// -> | . | ..
	CommentMarker,		// /*  */ | //
	Terminator			// #
};

/* 
 * 结构体，指示源代码位置
 */
struct SourceLocation
{
	size_t column;
	size_t line;

	SourceLocation(size_t column = 0, size_t line = 0);
};

/*
 * Token
 */
struct Token
{
	TokenType type;
	string content;
	SourceLocation location;

	Token(TokenType type, const string& content, const SourceLocation& location);

	// 快速判断是否属于某一类 / 某一类的具体某个 Token
	bool EqualTo(TokenType type) const;
	bool EqualTo(TokenType type, const string& content) const;
};