#pragma once
#include <vector>
#include "Token.h"
#include "AST.h"

/*
 * 语法分析错误相关
 * ParserError 记录：错误类型 + 出错的Token
 */
enum class ParserErrorType {
	UnexpectedToken,
	UnterminatedProgram
};

struct ParserError
{
	ParserErrorType type;
	Token token;

	ParserError(ParserErrorType type, Token token)
		: type(type), token(token)
	{
	}
};

/*
 * 语法分析器
 */
class Parser
{
public:
	Parser(const vector<Token>& tokenList);
	//~Parser();

private:
	// 原始 Token
	const vector<Token>& _tokenList;
	size_t _cursor;

	// 语法分析结果
	unique_ptr<ASTNode> _root;
	vector<ParserError> _errorList;	// 错误列表

	// 读取 Token 的方法
	Token Advance();
	Token Peek();
	Token PeekNext();

	bool IsAtEnd();

	// 消耗特定类型的 Token，如果类型不符/内容不符则引发错误错误
	Token Consume(TokenType type);	

	void AddError(ParserErrorType type, Token token);

	// 递归分析下降程序
	unique_ptr<Node_VariableProperty>	Parse_VariableProperty();
	unique_ptr<Node_Type>				Parse_Type();
	unique_ptr<Node_LeftValue>			Parse_LeftValue();

	unique_ptr<Node_Program>				Parse_Program();
	vector<unique_ptr<Node_Declaration>>	Parse_DeclarationSequence();
	unique_ptr<Node_Declaration>			Parse_Declaration();
	unique_ptr<Node_FunctionDeclaration>	Parse_FunctionDeclaration();
	unique_ptr<Node_FunctionHeader>			Parse_FunctionHeaderDeclaration();
	unique_ptr<Node_Type>					Parse_FunctionTypeDeclaration();

	vector<unique_ptr<Node_Parameter>>	Parse_ParameterList();
	unique_ptr<Node_Parameter>			Parse_Parameter();

	unique_ptr<Node_StatementBlock>		Parse_StatementBlock();
	vector<unique_ptr<Node_Statement>>	Parse_StatementSequence();
	unique_ptr<Node_Statement>			Parse_Statement();

	unique_ptr<Node_ReturnStatement>	Parse_ReturnStatement();

	unique_ptr<Node_VariableDeclarationStatement>	Parse_VariableDeclarationStatement();
	unique_ptr<Node_Type>							Parse_VariableTypeDeclaration();

	unique_ptr<Node_IfStatement>		Parse_IfStatement();
	unique_ptr<Node_StatementBlock>		Parse_ElseStatement();

	unique_ptr<Node_LoopStatement>	Parse_LoopStatement();
	unique_ptr<Node_WhileStatement>	Parse_WhileStatement();

	unique_ptr<Node_ExpressionStatement>	Parse_ExpressionStatement();

	unique_ptr<Node_AssignmentStatement>	Parse_AssignmentStatement();

	unique_ptr<Node_Expression>	Parse_OptionalExpression();
	unique_ptr<Node_Expression>	Parse_Expression();
	unique_ptr<Node_Expression>	Parse_AddSubExpression();
	unique_ptr<Node_Expression>	Parse_Term();
	unique_ptr<Node_Expression>	Parse_Oprand();

	vector<unique_ptr<Node_Expression>> Parse_ArgumentList();

	// 出错时恢复
	void Synchronize();
	
public:
	// 一键式完成语法分析、获取错误列表
	const ASTNode* Parse();
	const vector<ParserError>& GetErrorList();
};