#include "Lexer.h"
#include <fstream>
#include <sstream>
using namespace std;

Lexer::Lexer()
{
	_cursor = 0;
	_sourceCode = "";
	_location.column = 1;
	_location.line = 1;
}

/************************************************
* Advance：移动游标，读取并返回下一个字符
************************************************/
char Lexer::Advance()
{
	if (IsAtEnd())
		return SOURCE_END_CHAR;
	
	char ch = _sourceCode[_cursor++];
	
	// 更新位置信息
	if (ch == '\n') {
		_location.line++;
		_location.column = 1;
	}
	else {
		_location.column++;
	}

	return ch;
}

/************************************************
* Peek：不移动游标，读取并返回下一个字符
************************************************/
char Lexer::Peek()
{
	if (IsAtEnd())
		return SOURCE_END_CHAR;

	return _sourceCode[_cursor];
}
char Lexer::PeekNext()
{
	if (_cursor + 1 >= _sourceCode.length())
		return SOURCE_END_CHAR;

	return _sourceCode[_cursor + 1];
}

/************************************************
* SkipWhitespace：移动游标，跳过所有无效符号（空格、换行、注释）
************************************************/
void Lexer::SkipWhitespace()
{
	while (true) {
		char ch = Peek();
		switch (ch) {
		case ' ':
		case '\r':
		case '\t':
		case '\n':
			Advance(); // 跳过普通空白符
			break;
		case '/':	// 处理注释
			if (PeekNext() == '/')	// 单行注释：一直跳到行尾
			{
				while (!IsAtEnd() && Advance() != '\n')
					;
			}
			else if (PeekNext() == '*')	// 多行注释：跳到 */ 为止
			{
				SkipBlockComment();
			}
			else
			{
				return;	// 单独的 '/' 可能是除号，交给 NextToken 处理
			}
			break;
		default:
			return; // 遇到任何非空白、非注释字符，退出
		}
	}
}

/************************************************
* SkipBlockComment：移动游标，跳过所有块级注释
************************************************/
void Lexer::SkipBlockComment()
{
	SourceLocation location = _location;

	// 读入 /*
	Advance();
	Advance();

	while (!IsAtEnd()) {
		if (Peek() == '*' && PeekNext() == '/')	// 读入 */
		{
			Advance();
			Advance();
			return;
		}
		Advance();
	}

	// 如果运行到这里，说明未读到 */ ，注释未正常闭合，添加错误
	AddError(LexerErrorType::UnterminatedComment, location);
}

/************************************************
* IsAtEnd：判断游标是否到达源代码末尾
************************************************/
bool Lexer::IsAtEnd()
{
	return _cursor >= _sourceCode.length();
}

/************************************************
* IsAlpha等等：判断字符是否为字母/数字/字母或数字
************************************************/
bool Lexer::IsAlpha(char ch)
{
	return ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'));
}
bool Lexer::IsDigit(char ch)
{
	return (ch >= '0' && ch <= '9');
}
bool Lexer::IsAlphaOrDigit(char ch)
{
	return IsAlpha(ch) || IsDigit(ch);
}

/************************************************
* AddToken：向Token列表添加一个Token
************************************************/
Token Lexer::AddToken(TokenType type, const string& content, const SourceLocation& location)
{
	Token t(type, content, location);
	_tokenList.push_back(t);
	return t;
}

/************************************************
* AddError：向错误列表添加一个错误
************************************************/
void Lexer::AddError(LexerErrorType type, const SourceLocation& location)
{
	_errorList.push_back(LexerError(type, location));
}

/************************************************
* LoadFile：读取源代码文件
************************************************/
void Lexer::LoadFile(const string& filename)
{
	// 读取文件
	ifstream fin;

	fin.open(filename, ios::in);

	if (!fin.is_open())
		throw runtime_error("Failed to open file \"" + filename + "\".");

	// 文件内容用字符串流中转，存储到string成员中
	std::stringstream buffer;
	buffer << fin.rdbuf();

	_sourceCode = buffer.str();

	fin.close();
}

/************************************************
* NextToken：移动游标，读取并返回下一个Token
************************************************/
Token Lexer::NextToken()
{
	SkipWhitespace();

	if (IsAtEnd())
	{
		return Token(TokenType::NullType, "", _location);
	}

	// 读取一个字符，判断分支
	SourceLocation location = _location;
	char ch = Advance();

	if (ch == '_' || IsAlpha(ch))	// 标识符
	{
		string content = "";
		content += ch;
		while (!IsAtEnd() && (Peek() == '_' || IsAlphaOrDigit(Peek())))
		{
			content += Advance();
		}
		if (KEYWORDS.find(content) != KEYWORDS.end())	// 关键字是标识符的一种特例
		{
			return Token(TokenType::Keyword, content, location);
		}
		return Token(TokenType::Identifier, content, location);
	}
	else if (IsDigit(ch))	// 数值
	{
		string content = "";
		content += ch;
		while (!IsAtEnd() && IsDigit(Peek()))
		{
			content += Advance();
		}
		return Token(TokenType::Value, content, location);
	}
	else if (ch == '=')	// 赋值号和 == 算符
	{
		if (!IsAtEnd() && Peek() == '=')
		{
			Advance();
			return Token(TokenType::Operator, "==", location);
		}
		return Token(TokenType::AssignmentOperator, string(1, ch), location);
	}
	else if (ch == '!')	// != 算符
	{
		if (!IsAtEnd() && Peek() == '=')
		{
			Advance();
			return Token(TokenType::Operator, "!=", location);
		}
	}
	else if (ch == '<' || ch == '>')	// < | > | <= | >= 算符
	{
		string content = string(1, ch);
		if (!IsAtEnd() && Peek() == '=')
		{
			content += Advance();
		}
		return Token(TokenType::Operator, content, location);
	}
	else if (ch == '-')	// -算符、->特殊符
	{
		if (!IsAtEnd() && Peek() == '>')
		{
			Advance();
			return Token(TokenType::Special, "->", location);
		}
		return Token(TokenType::Operator, string(1, ch), location);
	}
	else if (ch == '+' || ch == '*' || ch == '/' || ch == '&')	// + | * | / | & 算符
	{
		return Token(TokenType::Operator, string(1, ch), location);
	}
	else if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}')	// 界符
	{
		return Token(TokenType::Delimiter, string(1, ch), location);
	}
	else if (ch == ';' || ch == ':' || ch == ',')	// 分隔符
	{
		return Token(TokenType::Separator, string(1, ch), location);
	}
	else if (ch == '.')	// .和..特殊符
	{
		if (!IsAtEnd() && Peek() == '=')
		{
			Advance();
			return Token(TokenType::Special, "..", location);
		}
		return Token(TokenType::Special, string(1, ch), location);
	}
	//else if (ch == '#')	// 终止符
	//{
	//	return Token(TokenType::Terminator, "#", location);
	//}

	// 如果运行到这里，说明是未定义的Token，添加错误
	AddError(LexerErrorType::UndefinedToken, location);
	return Token(TokenType::NullType, string(1, ch), location);
}

/************************************************
* Lex：进行完整的词法分析，返回 Token 列表
************************************************/
const vector<Token>& Lexer::Lex()
{
	_tokenList.clear();
	_errorList.clear();
	_cursor = 0;
	_location.column = 1, _location.line = 1;

	while (!IsAtEnd()) {
		Token t = NextToken();
		_tokenList.push_back(t);
	}

	_tokenList.push_back(Token(TokenType::Terminator, "#", SourceLocation()));

	return _tokenList;
}

/************************************************
* GetErrorList：返回错误列表（需要在执行Lex之后才会有效）
************************************************/
const vector<LexerError>& Lexer::GetErrorList()
{
	return _errorList;
}
