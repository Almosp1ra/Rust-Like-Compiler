#include "Parser.h"
#include <iostream>

Parser::Parser(const vector<Token>& tokenList): _tokenList(tokenList)
{
	_cursor = 0;
}

/************************************************
* Advance：移动游标，读取并返回下一个Token
************************************************/
Token Parser::Advance()
{
	if (IsAtEnd())
		return Token(TokenType::NullType, "", SourceLocation());

	Token t = _tokenList[_cursor++];

	return t;
}

/************************************************
* Peek：不移动游标，读取并返回下一个字符
************************************************/
Token Parser::Peek()
{
	if (_cursor >= _tokenList.size())
		return Token(TokenType::NullType, "", SourceLocation());

	return _tokenList[_cursor];
}
Token Parser::PeekNext()
{
	if (_cursor + 1 >= _tokenList.size())
		return Token(TokenType::NullType, "", SourceLocation());

	return _tokenList[_cursor + 1];
}

/************************************************
* IsAtEnd：判断游标是否到达源代码末尾
************************************************/
bool Parser::IsAtEnd()
{
	return _cursor >= _tokenList.size()
		|| _tokenList[_cursor].EqualTo(TokenType::Terminator);
}

/************************************************
* Consume：读入一个Token
************************************************/
Token Parser::Consume(TokenType type)
{
	if (!Peek().EqualTo(type))
	{
		AddError(ParserErrorType::UnexpectedToken, Peek());
	}

	return Advance();
}

/************************************************
* AddError：向错误列表添加一个错误
************************************************/
void Parser::AddError(ParserErrorType type, Token token)
{
	_errorList.push_back(ParserError(type, token));
}

/************************************************
* Synchronize: 分析出错时，回到一个安全位置继续分析
************************************************/
void Parser::Synchronize()
{
	while (!IsAtEnd())
	{
		if (Peek().EqualTo(TokenType::Separator_Semicolon))
		{
			Advance();
			return;
		}
		if (Peek().EqualTo(TokenType::Delimiter_BracketR))
		{
			return;
		}
		if (Peek().EqualTo(TokenType::Keyword_Fn))
		{
			return;
		}

		Advance();
	}
}

/************************************************
* 递归下降分析
************************************************/

// <变量属性> → mut
unique_ptr<Node_VariableProperty> Parser::Parse_VariableProperty()
{
	if (Peek().EqualTo(TokenType::Keyword_Mut))
	{
		Consume(TokenType::Keyword_Mut);

		auto property = make_unique<Node_VariableProperty>();
		property->property = VariableProperty::Mut;

		return property;
	}

	AddError(ParserErrorType::UnexpectedToken, Peek());
	Advance();

	return nullptr;
}

// <类型> → i32
unique_ptr<Node_Type> Parser::Parse_Type()
{
	if (Peek().EqualTo(TokenType::Keyword_I32))
	{
		Consume(TokenType::Keyword_I32);

		auto type = make_unique<Node_PrimitiveType>();
		type->type = PrimitiveType::I32;

		return type;
	}

	AddError(ParserErrorType::UnexpectedToken, Peek());
	Advance();

	return nullptr;
}

// <左值> → <可取引用>
// <可取引用> → <可取元素>
// <可取元素> → <ID>
unique_ptr<Node_LeftValue> Parser::Parse_LeftValue()
{
	if (Peek().EqualTo(TokenType::Identifier))
	{
		auto leftValue = make_unique<Node_LeftValue_Identifier>();
		leftValue->name = Consume(TokenType::Identifier).content;

		return leftValue;
	}

	AddError(ParserErrorType::UnexpectedToken, Peek());
	Advance();

	return nullptr;
}

// Program → <声明串>
unique_ptr<Node_Program> Parser::Parse_Program()
{
	auto program = make_unique<Node_Program>();

	program->declarations = Parse_DeclarationSequence();

	return program;
}

// <声明串> → 空 | <声明> <声明串>
vector<unique_ptr<Node_Declaration>> Parser::Parse_DeclarationSequence()
{
	auto declarationList = vector<unique_ptr<Node_Declaration>>();

	while (Peek().EqualTo(TokenType::Keyword_Fn))
	{
		auto declaration = Parse_Declaration();
		if (declaration)
			declarationList.push_back(std::move(declaration));
	}

	return declarationList;
}

// <声明> → <函数声明>
unique_ptr<Node_Declaration> Parser::Parse_Declaration()
{
	return Parse_FunctionDeclaration();
}

// <函数声明> → <函数头声明> <语句块>
unique_ptr<Node_FunctionDeclaration> Parser::Parse_FunctionDeclaration()
{
	auto functionDeclaration = make_unique<Node_FunctionDeclaration>();

	functionDeclaration->header = Parse_FunctionHeaderDeclaration();
	functionDeclaration->body   = Parse_StatementBlock();

	return functionDeclaration;
}

// <函数头声明> → fn <ID> '(' <形参列表> ')' <函数类型声明>
unique_ptr<Node_FunctionHeader> Parser::Parse_FunctionHeaderDeclaration()
{
	auto header = make_unique<Node_FunctionHeader>();

	Consume(TokenType::Keyword_Fn);

	header->name = Consume(TokenType::Identifier).content;

	Consume(TokenType::Delimiter_ParenL);
	header->parameterList = Parse_ParameterList();
	Consume(TokenType::Delimiter_ParenR);

	// 函数类型声明
	header->type = Parse_FunctionTypeDeclaration();

	return header;
}

// <函数类型声明> → 空 | '->' <类型>
unique_ptr<Node_Type> Parser::Parse_FunctionTypeDeclaration()
{
	if (Peek().EqualTo(TokenType::Arrow))
	{
		Consume(TokenType::Arrow);
		return Parse_Type();
	}

	return nullptr;
}

// <形参列表> → 空 | <形参><后续形参>
// <后续形参> → 空 | ',' <形参><后续形参>
vector<unique_ptr<Node_Parameter>> Parser::Parse_ParameterList()
{
	auto parameterList = vector<unique_ptr<Node_Parameter>>();

	if (Peek().EqualTo(TokenType::Keyword_Mut))
	{
		parameterList.push_back(Parse_Parameter());

		while (Peek().EqualTo(TokenType::Separator_Comma))
		{
			Consume(TokenType::Separator_Comma);
			parameterList.push_back(Parse_Parameter());
		}
	}

	return parameterList;
}

// <形参> → <变量属性> <ID> ':' <类型>
unique_ptr<Node_Parameter> Parser::Parse_Parameter()
{
	unique_ptr<Node_Parameter> parameter = make_unique<Node_Parameter>();

	parameter->varProperty = Parse_VariableProperty();
	if (!parameter->varProperty)
		return nullptr;

	parameter->name = Consume(TokenType::Identifier).content;

	Consume(TokenType::Separator_Colon);

	parameter->type = Parse_Type();

	return parameter;
}

//<语句块> → '{' <语句串> '}'
unique_ptr<Node_StatementBlock> Parser::Parse_StatementBlock()
{
	auto statementBlock = make_unique<Node_StatementBlock>();

	Consume(TokenType::Delimiter_BracketL);
	statementBlock->statements = Parse_StatementSequence();
	Consume(TokenType::Delimiter_BracketR);

	return statementBlock;
}

// <语句串> → 空 | <语句><语句串>
vector<unique_ptr<Node_Statement>> Parser::Parse_StatementSequence()
{
	auto statementList = vector<unique_ptr<Node_Statement>>();

	Token t = Peek();

	while (t.EqualTo(TokenType::Separator_Semicolon) 
		|| t.EqualTo(TokenType::Keyword_Return)
		|| t.EqualTo(TokenType::Keyword_Let)
		|| t.EqualTo(TokenType::Keyword_If)
		|| t.EqualTo(TokenType::Keyword_While)
		|| t.EqualTo(TokenType::Identifier)
		|| t.EqualTo(TokenType::Value)
		|| t.EqualTo(TokenType::Delimiter_ParenL))
	{
		auto statement = Parse_Statement();
		if (statement)
			statementList.push_back(std::move(statement));
		t = Peek();
	}

	return statementList;
}

// <语句> → ';' | <返回语句> | <变量声明语句> | <if语句> | <循环语句> | <赋值语句> | <表达式语句>
unique_ptr<Node_Statement> Parser::Parse_Statement()
{
	Token t = Peek();

	if (t.EqualTo(TokenType::Separator_Semicolon))
	{
		Consume(TokenType::Separator_Semicolon);
		return make_unique<Node_EmptyStatement>();
	}
	else if (t.EqualTo(TokenType::Keyword_Return))
	{
		return Parse_ReturnStatement();
	}
	else if (t.EqualTo(TokenType::Keyword_Let))
	{
		return Parse_VariableDeclarationStatement();
	}
	else if (t.EqualTo(TokenType::Keyword_If))
	{
		return Parse_IfStatement();
	}
	else if (t.EqualTo(TokenType::Keyword_While))
	{
		return Parse_LoopStatement();
	}
	else if (t.EqualTo(TokenType::Identifier) && PeekNext().EqualTo(TokenType::Operator_Assign))	// 这里相当于用LL(2)来写，根据后两个token来判断是赋值语句还是表达式语句
	{
		return Parse_AssignmentStatement();
	}
	else if (t.EqualTo(TokenType::Identifier)
		  || t.EqualTo(TokenType::Value)
		  || t.EqualTo(TokenType::Delimiter_ParenL))
	{
		return Parse_ExpressionStatement();
	}

	AddError(ParserErrorType::UnexpectedToken, Peek());
	Synchronize();

	return nullptr;
}

// <返回语句> → return <可选表达式> ';'
unique_ptr<Node_ReturnStatement> Parser::Parse_ReturnStatement()
{
	auto returnStatement = make_unique<Node_ReturnStatement>();

	Consume(TokenType::Keyword_Return);
	returnStatement->expr = Parse_OptionalExpression();
	Consume(TokenType::Separator_Semicolon);

	return returnStatement;
}

// <变量声明语句> → let <变量属性> <ID> <变量类型声明> ';' 
unique_ptr<Node_VariableDeclarationStatement> Parser::Parse_VariableDeclarationStatement()
{
	auto variableDeclarationStatement = make_unique<Node_VariableDeclarationStatement>();

	Consume(TokenType::Keyword_Let);

	variableDeclarationStatement->varProperty = Parse_VariableProperty();
	if (!variableDeclarationStatement->varProperty)
	{
		return nullptr;
	}

	variableDeclarationStatement->name = Consume(TokenType::Identifier).content;

	variableDeclarationStatement->type = Parse_VariableTypeDeclaration();

	Consume(TokenType::Separator_Semicolon);

	return variableDeclarationStatement;
}

// <变量类型声明> → 空 | ':' <类型>
unique_ptr<Node_Type> Parser::Parse_VariableTypeDeclaration()
{
	if (Peek().EqualTo(TokenType::Separator_Colon))
	{
		Consume(TokenType::Separator_Colon);
		return Parse_Type();
	}
	
	return nullptr;
}

// <if语句> → if <表达式> <语句块> <else部分>
unique_ptr<Node_IfStatement> Parser::Parse_IfStatement()
{
	auto ifStatement = make_unique<Node_IfStatement>();

	Consume(TokenType::Keyword_If);

	ifStatement->condition = Parse_Expression();
	ifStatement->thenBlock = Parse_StatementBlock();
	ifStatement->elseBlock = Parse_ElseStatement();

	return ifStatement;
}

// <else部分> → 空
unique_ptr<Node_StatementBlock> Parser::Parse_ElseStatement()
{
	return nullptr;
}

// <循环语句> → <while语句> 
unique_ptr<Node_LoopStatement> Parser::Parse_LoopStatement()
{
	return Parse_WhileStatement();
}

// <while语句> → while <表达式> <语句块>
unique_ptr<Node_WhileStatement> Parser::Parse_WhileStatement()
{
	auto whileStatement = make_unique<Node_WhileStatement>();

	Consume(TokenType::Keyword_While);
	whileStatement->condition = Parse_Expression();
	whileStatement->body = Parse_StatementBlock();

	return whileStatement;
}

// <赋值语句> → <左值> '=' <表达式> ';'
unique_ptr<Node_AssignmentStatement> Parser::Parse_AssignmentStatement()
{
	auto assignmentStatement = make_unique<Node_AssignmentStatement>();

	assignmentStatement->leftValue = Parse_LeftValue();
	Consume(TokenType::Operator_Assign);
	assignmentStatement->expr = Parse_Expression();
	Consume(TokenType::Separator_Semicolon);

	return assignmentStatement;
}

// <表达式语句> → <表达式> ';'
unique_ptr<Node_ExpressionStatement> Parser::Parse_ExpressionStatement()
{
	auto expressionStatement = make_unique<Node_ExpressionStatement>();

	expressionStatement->expr =	Parse_Expression();
	Consume(TokenType::Separator_Semicolon);

	return expressionStatement;
}

// <可选表达式> → 空 | <表达式>
unique_ptr<Node_Expression>	Parser::Parse_OptionalExpression()
{
	Token t = Peek();

	if (t.EqualTo(TokenType::Identifier)
	 || t.EqualTo(TokenType::Value)
	 || t.EqualTo(TokenType::Delimiter_ParenL))
	{
		return Parse_Expression();
	}

	return nullptr;
}

// <表达式> → <加减表达式> <后续表达式>
// <后续表达式> → 空 | <比较运算符> <加减表达式> <后续表达式>
unique_ptr<Node_Expression>	Parser::Parse_Expression()
{
	// 第一个加减表达式
	auto expr = Parse_AddSubExpression();

	BinaryOperator nextOp = TokenToBinaryOp(Peek());

	// 后续加减表达式
	while (OpIsComparison(nextOp))
	{
		Consume(Peek().type);

		auto expr_temp = make_unique<Node_BinaryExpression>();

		expr_temp->left = move(expr);
		expr_temp->op = nextOp;
		expr_temp->right = Parse_AddSubExpression();

		nextOp = TokenToBinaryOp(Peek());

		expr = move(expr_temp);
	}

	return expr;
}

// <加减表达式> → <项> <后续加减表达式> 
// <后续加减表达式> → 空 | <加减运算符> <项> <后续加减表达式>
unique_ptr<Node_Expression>	Parser::Parse_AddSubExpression()
{
	// 第一个项
	auto expr = Parse_Term();

	BinaryOperator nextOp = TokenToBinaryOp(Peek());

	// 后续项
	while (OpIsAddSub(nextOp))
	{
		Consume(Peek().type);

		auto expr_temp = make_unique<Node_BinaryExpression>();

		expr_temp->left = move(expr);
		expr_temp->op = nextOp;
		expr_temp->right = Parse_Term();

		nextOp = TokenToBinaryOp(Peek());

		expr = move(expr_temp);
	}

	return expr;
}

// <项> → <因子> <后续项> 
// <后续项> → 空 | <乘除运算符> <因子> <后续项>
unique_ptr<Node_Expression>	Parser::Parse_Term()
{
	// 第一个因子
	auto expr = Parse_Oprand();

	BinaryOperator nextOp = TokenToBinaryOp(Peek());

	// 后续因子
	while (OpIsMultDiv(nextOp))
	{
		Consume(Peek().type);

		auto expr_temp = make_unique<Node_BinaryExpression>();

		expr_temp->left = move(expr);
		expr_temp->op = nextOp;
		expr_temp->right = Parse_Term();

		nextOp = TokenToBinaryOp(Peek());

		expr = move(expr_temp);
	}

	return expr;
}

// <因子> → <NUM> | '(' < 表达式 > ')' | <ID> <因子参数列表>
// <因子参数列表> → 空 | '(' < 实参列表 > ')'
unique_ptr<Node_Expression>	Parser::Parse_Oprand()
{
	Token t = Peek();

	if (t.EqualTo(TokenType::Value))
	{
		auto number = make_unique<Node_NumberExpression>();

		number->value = Consume(TokenType::Value).content;

		return number;
	}
	else if (t.EqualTo(TokenType::Delimiter_ParenL))
	{
		Consume(TokenType::Delimiter_ParenL);
		auto expr = Parse_Expression();
		Consume(TokenType::Delimiter_ParenR);

		return expr;
	}
	else if (t.EqualTo(TokenType::Identifier))
	{
		if (PeekNext().EqualTo(TokenType::Delimiter_ParenL))
		{
			auto call = make_unique<Node_CallExpression>();

			call->name = Consume(TokenType::Identifier).content;
			Consume(TokenType::Delimiter_ParenL);
			call->argumentList = Parse_ArgumentList();
			Consume(TokenType::Delimiter_ParenR);

			return call;
		}

		auto variable = make_unique<Node_VariableExpression>();
		variable->leftValue = Parse_LeftValue();
		return variable;
	}

	AddError(ParserErrorType::UnexpectedToken, Peek());
	Advance();

	return nullptr;
}

// <实参列表> → 空 | <表达式><后续实参>
// <后续实参> → 空 | ',' < 实参列表 >
vector<unique_ptr<Node_Expression>> Parser::Parse_ArgumentList()
{
	auto argumentList = vector<unique_ptr<Node_Expression>>();

	Token t = Peek();

	if (t.EqualTo(TokenType::Identifier)
		|| t.EqualTo(TokenType::Value)
		|| t.EqualTo(TokenType::Delimiter_ParenL))
	{
		argumentList.push_back(Parse_Expression());

		while (Peek().EqualTo(TokenType::Separator_Comma))
		{
			Consume(TokenType::Separator_Comma);
			argumentList.push_back(Parse_Expression());
		}
	}

	return argumentList;
}


/************************************************
* Parse：语法分析，返回抽象语法树根节点
************************************************/
const ASTNode* Parser::Parse()
{
	_root = Parse_Program();

	if (!IsAtEnd())
	{
		AddError(ParserErrorType::UnterminatedProgram, _tokenList[_cursor]);
	}

	return _root.get();
}

/************************************************
* GetErrorList：返回错误列表
************************************************/
const vector<ParserError>& Parser::GetErrorList()
{
	return _errorList;
}