#include <iostream>
#include <iomanip>
#include "Lexer.h"
#include "Parser.h"
#include "CompilerTools.h"
using namespace std;

int main(int argv, char** argc)
{
	/* 命令行参数解析 */

	if (argv != 2)
	{
		CompilerTools::PrintUsage(argc[0]);
		return 0;
	}

	string filename = argc[1];

	/* 词法分析 */

	// 初始化、读入源代码文件
	Lexer lexer;

	try
	{
		lexer.LoadFile(filename);
	}
	catch (exception& e)
	{
		cout << e.what() << endl;
		return -1;
	}

	// 分析执行
	const auto& tokenList = lexer.Lex();
	const auto& lexerErrorList = lexer.GetErrorList();

	// 结果打印
	CompilerTools::PrintLexerError(lexerErrorList);
	CompilerTools::PrintTokenList(tokenList);

	if (lexerErrorList.size())
	{
		return 0;
	}

	/* 语法分析 */

	// 分析执行
	Parser parser(tokenList);

	auto ASTRoot = parser.Parse();
	const auto& parserErrorList = parser.GetErrorList();

	// 结果打印
	CompilerTools::PrintParserError(parserErrorList);

	if (parserErrorList.size())	// 出错直接返回
	{
		return 0;
	}

	CompilerTools::PrintAST(ASTRoot);

	return 0;
}