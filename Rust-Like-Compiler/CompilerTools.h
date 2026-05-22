#pragma once
#include <iostream>
#include <iomanip>
#include "Lexer.h"
#include "Parser.h"
using namespace std;

/* 
* 编译程序运行工具
* 包括打印 Usage、打印各阶段编译结果等
*/

class CompilerTools
{
public:
	// 使用方法打印
	static void PrintUsage(const char* programName);
	
	// 词法分析结果打印
	static void PrintTokenList(const vector<Token>& tokenList);
	static void PrintLexerError(const vector<LexerError>& errorList);

	// 词法分析结果打印
	static void PrintParserError(const vector<ParserError>& errorList);
	static void PrintAST(const ASTNode* root);
	static void PrintIndent(int depth);
	static void PrintNode(const ASTNode* node, int depth);
};