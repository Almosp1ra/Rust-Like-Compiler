#include <iostream>
#include <iomanip>
#include "Lexer.h"
#include "Parser.h"
#include "SemanticAnalyzer.h"
#include "CompilerTools.h"
#include "IntermediateCodeGenerator.h"

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

	/* 语义检查 */

	// 分析执行
	SemanticAnalyzer semanticAnalyzer;

	semanticAnalyzer.SemanticAnalyze(ASTRoot);

	const auto& symbolTable = semanticAnalyzer.GetSymbolTable();
	const auto& semanticErrorList = semanticAnalyzer.GetErrorList();

	// 结果打印
	CompilerTools::PrintSemanticError(semanticErrorList);

	for (size_t i = 0; i < semanticErrorList.size(); i++)
	{
		if (semanticErrorList[i].level == ErrorLevel::Error)	// 出错直接返回
		{
			return 0;
		}
	}

	CompilerTools::PrintSymbolTable(symbolTable);

	/* 中间代码生成 */

	IntermediateCodeGenerator generator;

	generator.Generate(ASTRoot);

	const auto& quadruples = generator.GetQuadruples();

	cout << endl;
	cout << "--- Intermediate Code ---" << endl;

	for (size_t i = 0; i < quadruples.size(); i++)
	{
		cout
			<< "("
			<< quadruples[i].op << ", "
			<< quadruples[i].arg1 << ", "
			<< quadruples[i].arg2 << ", "
			<< quadruples[i].result
			<< ")"
			<< endl;
	}


	return 0;
}