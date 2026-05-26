#pragma once
#include <string>
#include <unordered_set>
#include <map>
#include "SourceLocation.h"
using namespace std;

/* 返回的终止符 */
const char SOURCE_END_CHAR = '\0';

/* token 类别枚举 */
enum class TokenType {
	NullType,

	// Keyword,			// i32 | let | if | else | while | return | mut | fn | for | in | loop | break | continue
	Keyword_I32,
	Keyword_Let,
	Keyword_If,
	Keyword_Else,
	Keyword_While,
	Keyword_Return,
	Keyword_Mut,
	Keyword_Fn,
	Keyword_For,
	Keyword_In,
	Keyword_Loop,
	Keyword_Break,
	Keyword_Continue,

	Identifier,			// (字母|_)(字母|数字|_)
	Value,				// 数字(数字)*

	// Operator,			// + | - | * | / | == | > | >= | < | <= | != | & | *
	Operator_Assign,	// =
	Operator_Add,
	Operator_Sub,
	Operator_Mult,
	Operator_Div,
	Operator_Eq,
	Operator_Gt,
	Operator_Ge,
	Operator_Lt,
	Operator_Le,
	Operator_Ne,
	Operator_And,

	// Delimiter,			// ( | ) | { | } | [ | ]
	Delimiter_ParenL,
	Delimiter_ParenR,
	Delimiter_BraceL,
	Delimiter_BraceR,
	Delimiter_BracketL,
	Delimiter_BracketR,

	// Separator,			// ; | : | ,
	Separator_Semicolon,
	Separator_Colon,
	Separator_Comma,

	// Special,			// -> | . | ..
	Arrow,
	Dot,
	DotDot,

	// CommentMarker,		// /*  */ | //
	Comment_Line,
	Comment_Block,

	Terminator			// #
};

/* TokenType 与 String 转换 */
// 列表
const map<string, TokenType> STR_TO_TOKENTYPE = {
	pair<string, TokenType>("i32", TokenType::Keyword_I32),
	pair<string, TokenType>("let", TokenType::Keyword_Let),
	pair<string, TokenType>("if", TokenType::Keyword_If),
	pair<string, TokenType>("else", TokenType::Keyword_Else),
	pair<string, TokenType>("while", TokenType::Keyword_While),
	pair<string, TokenType>("return", TokenType::Keyword_Return),
	pair<string, TokenType>("mut", TokenType::Keyword_Mut),
	pair<string, TokenType>("fn", TokenType::Keyword_Fn),
	pair<string, TokenType>("for", TokenType::Keyword_For),
	pair<string, TokenType>("in", TokenType::Keyword_In),
	pair<string, TokenType>("loop", TokenType::Keyword_Loop),
	pair<string, TokenType>("break", TokenType::Keyword_Break),
	pair<string, TokenType>("continue", TokenType::Keyword_Continue),

	pair<string, TokenType>("=", TokenType::Operator_Assign),
	pair<string, TokenType>("+", TokenType::Operator_Add),
	pair<string, TokenType>("-", TokenType::Operator_Sub),
	pair<string, TokenType>("*", TokenType::Operator_Mult),
	pair<string, TokenType>("/", TokenType::Operator_Div),
	pair<string, TokenType>("==", TokenType::Operator_Eq),
	pair<string, TokenType>(">", TokenType::Operator_Gt),
	pair<string, TokenType>(">=", TokenType::Operator_Ge),
	pair<string, TokenType>("<", TokenType::Operator_Lt),
	pair<string, TokenType>("<=", TokenType::Operator_Le),
	pair<string, TokenType>("!=", TokenType::Operator_Ne),
	pair<string, TokenType>("&", TokenType::Operator_And),

	pair<string, TokenType>("(", TokenType::Delimiter_ParenL),
	pair<string, TokenType>(")", TokenType::Delimiter_ParenR),
	pair<string, TokenType>("[", TokenType::Delimiter_BraceL),
	pair<string, TokenType>("]", TokenType::Delimiter_BraceR),
	pair<string, TokenType>("{", TokenType::Delimiter_BracketL),
	pair<string, TokenType>("}", TokenType::Delimiter_BracketR),

	pair<string, TokenType>(";", TokenType::Separator_Semicolon),
	pair<string, TokenType>(":", TokenType::Separator_Colon),
	pair<string, TokenType>(",", TokenType::Separator_Comma),

	pair<string, TokenType>("->", TokenType::Arrow),
	pair<string, TokenType>(".", TokenType::Dot),
	pair<string, TokenType>("..", TokenType::DotDot)
};
TokenType StringToTokenType(string content);

/*
 * Token
 */
struct Token
{
	TokenType type;
	string content;
	SourceLocation location;

	Token(TokenType type, const string& content, const SourceLocation& location);

	// 快速判断是否属于某一个 Token
	bool EqualTo(TokenType type) const;
};