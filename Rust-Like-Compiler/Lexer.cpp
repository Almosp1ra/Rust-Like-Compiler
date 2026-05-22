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

	string content = "";

	// 读入 /*
	content += Advance();
	content += Advance();

	while (!IsAtEnd()) {
		if (Peek() == '*' && PeekNext() == '/')	// 读入 */
		{
			content += Advance();
			content += Advance();
			return;
		}
		content += Advance();
	}

	// 如果运行到这里，说明未读到 */ ，注释未正常闭合，添加错误
	AddError(LexerErrorType::UnterminatedComment, location, content);
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
void Lexer::AddError(LexerErrorType type, const SourceLocation& location, const string content)
{
	_errorList.push_back(LexerError(type, location, content));
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
	string content = string(1, ch);

	if (ch == '_' || IsAlpha(ch))	// 标识符
	{
		while (!IsAtEnd() && (Peek() == '_' || IsAlphaOrDigit(Peek())))
		{
			content += Advance();
		}

		auto type = StringToTokenType(content);

		if (type != TokenType::NullType)	// 关键字是标识符的一种特例
		{
			return Token(type, content, location);
		}
		return Token(TokenType::Identifier, content, location);
	}
	else if (IsDigit(ch))	// 数值
	{
		while (!IsAtEnd() && IsDigit(Peek()))
		{
			content += Advance();
		}
		return Token(TokenType::Value, content, location);
	}
	else if (ch == '=')	// = 和 == 算符
	{
		if (!IsAtEnd() && Peek() == '=')
		{
			content += Advance();
			return Token(TokenType::Operator_Eq, content, location);
		}
		return Token(TokenType::Operator_Assign, content, location);
	}
	else if (ch == '!')	// != 算符
	{
		if (!IsAtEnd() && Peek() == '=')
		{
			content += Advance();
			return Token(TokenType::Operator_Ne, content, location);
		}
	}
	else if (ch == '<')	// < | <= 算符
	{
		if (!IsAtEnd() && Peek() == '=')
		{
			content += Advance();
			return Token(TokenType::Operator_Le, content, location);
		}
		return Token(TokenType::Operator_Lt, content, location);
	}
	else if (ch == '>')	// > | >= 算符
	{
		if (!IsAtEnd() && Peek() == '=')
		{
			content += Advance();
			return Token(TokenType::Operator_Ge, content, location);
		}
		return Token(TokenType::Operator_Gt, content, location);
	}
	else if (ch == '-')	// -算符、->特殊符
	{
		if (!IsAtEnd() && Peek() == '>')
		{
			content += Advance();
			return Token(TokenType::Arrow, content, location);
		}
		return Token(TokenType::Operator_Sub, content, location);
	}
	else if (ch == '+')	// + 算符
	{
		return Token(TokenType::Operator_Add, content, location);
	}
	else if (ch == '*')	// * 算符
	{
		return Token(TokenType::Operator_Mult, content, location);
	}
	else if (ch == '/')	// / 算符
	{
		return Token(TokenType::Operator_Div, content, location);
	}
	else if (ch == '&')	// & 算符
	{
		return Token(TokenType::Operator_And, content, location);
	}
	else if (ch == '(')	// 界符
	{
		return Token(TokenType::Delimiter_ParenL, content, location);
	}
	else if (ch == ')')
	{
		return Token(TokenType::Delimiter_ParenR, content, location);
	}
	else if (ch == '[')
	{
		return Token(TokenType::Delimiter_BraceL, content, location);
	}
	else if (ch == ']')
	{
		return Token(TokenType::Delimiter_BraceR, content, location);
	}
	else if (ch == '{')
	{
		return Token(TokenType::Delimiter_BracketL, content, location);
	}
	else if (ch == '}')
	{
		return Token(TokenType::Delimiter_BracketR, content, location);
	}
	else if (ch == ';')	// 分隔符
	{
		return Token(TokenType::Separator_Semicolon, content, location);
	}
	else if (ch == ':')
	{
		return Token(TokenType::Separator_Colon, content, location);
	}
	else if (ch == ',')
	{
		return Token(TokenType::Separator_Comma, content, location);
	}
	else if (ch == '.')	// .和..特殊符
	{
		if (!IsAtEnd() && Peek() == '.')
		{
			content += Advance();
			return Token(TokenType::DotDot, content, location);
		}
		return Token(TokenType::Dot, content, location);
	}
	//else if (ch == '#')	// 终止符
	//{
	//	return Token(TokenType::Terminator, "#", location);
	//}

	// 如果运行到这里，说明是未定义的Token，添加错误
	AddError(LexerErrorType::UndefinedToken, location, content);
	return Token(TokenType::NullType, content, location);
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
		if(t.type != TokenType::NullType)
			_tokenList.push_back(t);
	}

	_tokenList.push_back(Token(TokenType::Terminator, "", SourceLocation()));

	return _tokenList;
}

/************************************************
* GetErrorList：返回错误列表（需要在执行Lex之后才会有效）
************************************************/
const vector<LexerError>& Lexer::GetErrorList()
{
	return _errorList;
}
