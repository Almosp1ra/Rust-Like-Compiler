#pragma once
#include <vector>
#include <string>
#include "Token.h"
#include "SourceLocation.h"
using namespace std;

/*
 * 词法分析错误相关
 * LexerError 记录：错误类型 + 源码发生错误的位置
 */
enum class LexerErrorType {
	UndefinedToken,
	UnterminatedComment
};

struct LexerError
{
	LexerErrorType type;
	SourceLocation location;
	string content;

	LexerError(LexerErrorType type, SourceLocation location, string content)
		: type(type), location(location), content(content)
	{
	}
};

/*
 * 词法分析器
 */
class Lexer
{
public:
	Lexer();
	//~Lexer();

private:
	// 源码读取相关记录
	string _sourceCode;
	size_t _cursor;
	SourceLocation _location;

	vector<Token> _tokenList;		// 已读取的Token列表
	vector<LexerError> _errorList;	// 错误列表

	// 读取源码的方法
	char Advance();				// 查看字符并向后移动一个位置
	char Peek();				// 查看当前位置的字符
	char PeekNext();			// 查看当前位置 + 1 的字符
	void SkipWhitespace();		// 跳过无关字符（空格、换行、注释等）
	void SkipBlockComment();	// 跳过注释块

	bool IsAtEnd();		// 判断cursor是否到达末尾			

	// 判断字符是否为字母/数字/字母或数字
	bool IsAlpha(char ch);
	bool IsDigit(char ch);
	bool IsAlphaOrDigit(char ch);

	// 向列表中加入 Token 或 错误
	Token AddToken(TokenType type, const string& content, const SourceLocation& location);
	void AddError(LexerErrorType type, const SourceLocation& location, const string content);

public:
	void  LoadFile(const string& filename);	// 读取文件
	Token NextToken();						// 向后读取一个 Token

	// 一键式完成词法分析、获取错误列表
	const vector<Token>& Lex();
	const vector<LexerError>& GetErrorList();
};