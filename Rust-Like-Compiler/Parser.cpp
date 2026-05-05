#include "Parser.h"

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
	if (IsAtEnd())
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
	return _tokenList[_cursor].EqualTo(TokenType::Terminator);
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
Token Parser::Consume(TokenType type, const string& content)
{
	if (!Peek().EqualTo(type, content))
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
		if (Peek().EqualTo(TokenType::Separator, ";"))
		{
			Advance();
			return;
		}
		if (Peek().EqualTo(TokenType::Delimiter, "}"))
		{
			return;
		}
		if (Peek().EqualTo(TokenType::Keyword, "fn"))
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
	if (Peek().EqualTo(TokenType::Keyword, "mut"))
	{
		Consume(TokenType::Keyword, "mut");

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
	if (Peek().EqualTo(TokenType::Keyword, "i32"))
	{
		Consume(TokenType::Keyword, "i32");

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

	while (Peek().EqualTo(TokenType::Keyword, "fn"))
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

	Consume(TokenType::Keyword, "fn");

	header->name = Consume(TokenType::Identifier).content;

	Consume(TokenType::Delimiter, "(");
	header->parameterList = Parse_ParameterList();
	Consume(TokenType::Delimiter, ")");

	// 函数类型声明
	header->type = Parse_FunctionTypeDeclaration();

	return header;
}

// <函数类型声明> → 空 | '->' <类型>
unique_ptr<Node_Type> Parser::Parse_FunctionTypeDeclaration()
{
	if (Peek().EqualTo(TokenType::Special, "->"))
	{
		Consume(TokenType::Special, "->");
		return Parse_Type();
	}

	return nullptr;
}

// <形参列表> → 空 | <形参><后续形参>
// <后续形参> → 空 | ',' < 形参列表 >
vector<unique_ptr<Node_Parameter>> Parser::Parse_ParameterList()
{
	auto parameterList = vector<unique_ptr<Node_Parameter>>();

	if (Peek().EqualTo(TokenType::Keyword, "mut"))
	{
		parameterList.push_back(Parse_Parameter());

		while (Peek().EqualTo(TokenType::Separator, ","))
		{
			Consume(TokenType::Separator, ",");
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

	Consume(TokenType::Separator, ":");

	parameter->type = Parse_Type();

	return parameter;
}

//<语句块> → '{' <语句串> '}'
unique_ptr<Node_StatementBlock> Parser::Parse_StatementBlock()
{
	auto statementBlock = make_unique<Node_StatementBlock>();

	Consume(TokenType::Delimiter, "{");
	statementBlock->statements = Parse_StatementSequence();
	Consume(TokenType::Delimiter, "}");

	return statementBlock;
}

// <语句串> → 空 | <语句><语句串>
vector<unique_ptr<Node_Statement>> Parser::Parse_StatementSequence()
{
	auto statementList = vector<unique_ptr<Node_Statement>>();

	Token t = Peek();

	while (t.EqualTo(TokenType::Separator, ";") 
		|| t.EqualTo(TokenType::Keyword, "return")
		|| t.EqualTo(TokenType::Keyword, "let")
		|| t.EqualTo(TokenType::Keyword, "if")
		|| t.EqualTo(TokenType::Keyword, "while")
		|| t.EqualTo(TokenType::Identifier)
		|| t.EqualTo(TokenType::Value)
		|| t.EqualTo(TokenType::Delimiter, "("))
	{
		auto statement = Parse_Statement();
		if (statement)
			statementList.push_back(std::move(statement));
		t = Peek();
	}

	return statementList;
}

// <语句> → ';' | <返回语句> | <变量声明语句> | <if语句> | <循环语句> | <ID开头语句> | <NUM开头语句> | <括号开头语句>
unique_ptr<Node_Statement> Parser::Parse_Statement()
{
	Token t = Peek();

	if (t.EqualTo(TokenType::Separator, ";"))
	{
		Consume(TokenType::Separator, ";");
		return make_unique<Node_EmptyStatement>();
	}
	else if (t.EqualTo(TokenType::Keyword, "return"))
	{
		return Parse_ReturnStatement();
	}
	else if (t.EqualTo(TokenType::Keyword, "let"))
	{
		return Parse_VariableDeclarationStatement();
	}
	else if (t.EqualTo(TokenType::Keyword, "if"))
	{
		return Parse_IfStatement();
	}
	else if (t.EqualTo(TokenType::Keyword, "while"))
	{
		return Parse_LoopStatement();
	}
	else if (t.EqualTo(TokenType::Identifier) && PeekNext().EqualTo(TokenType::AssignmentOperator, "="))	// 这里相当于用LL(2)来写，根据后两个token来判断是赋值语句还是表达式语句
	{
		return Parse_AssignmentStatement();
	}
	else if (t.EqualTo(TokenType::Identifier)
		  || t.EqualTo(TokenType::Value)
		  || t.EqualTo(TokenType::Delimiter, "("))
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

	Consume(TokenType::Keyword, "return");
	returnStatement->expr = Parse_OptionalExpression();
	Consume(TokenType::Separator, ";");

	return returnStatement;
}

// <变量声明语句> → let <变量声明> ';' 
// <变量声明> → <变量属性> <ID> <变量类型声明>
unique_ptr<Node_VariableDeclarationStatement> Parser::Parse_VariableDeclarationStatement()
{
	auto variableDeclarationStatement = make_unique<Node_VariableDeclarationStatement>();

	Consume(TokenType::Keyword, "let");

	variableDeclarationStatement->varProperty = Parse_VariableProperty();
	if (!variableDeclarationStatement->varProperty)
	{
		return nullptr;
	}

	variableDeclarationStatement->name = Consume(TokenType::Identifier).content;

	variableDeclarationStatement->type = Parse_VariableTypeDeclaration();

	Consume(TokenType::Separator, ";");

	return variableDeclarationStatement;
}

// <变量类型声明> → 空 | ':' <类型>
unique_ptr<Node_Type> Parser::Parse_VariableTypeDeclaration()
{
	if (Peek().EqualTo(TokenType::Separator, ":"))
	{
		Consume(TokenType::Separator, ":");
		return Parse_Type();
	}
	
	return nullptr;
}

// <if语句> → if <表达式> <语句块> <else部分>
unique_ptr<Node_IfStatement> Parser::Parse_IfStatement()
{
	auto ifStatement = make_unique<Node_IfStatement>();

	Consume(TokenType::Keyword, "if");

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

	Consume(TokenType::Keyword, "while");
	whileStatement->condition = Parse_Expression();
	whileStatement->body = Parse_StatementBlock();

	return whileStatement;
}

// <表达式语句> → <表达式> ';'
unique_ptr<Node_ExpressionStatement> Parser::Parse_ExpressionStatement()
{
	auto expressionStatement = make_unique<Node_ExpressionStatement>();

	expressionStatement->expr =	Parse_Expression();
	Consume(TokenType::Separator, ";");

	return expressionStatement;
}

// <赋值语句> → <左值> '=' <表达式> ';'
unique_ptr<Node_AssignmentStatement> Parser::Parse_AssignmentStatement()
{
	auto assignmentStatement = make_unique<Node_AssignmentStatement>();

	assignmentStatement->leftValue = Parse_LeftValue();
	Consume(TokenType::AssignmentOperator, "=");
	assignmentStatement->expr = Parse_Expression();
	Consume(TokenType::Separator, ";");

	return assignmentStatement;
}

// <可选表达式> → 空 | <表达式>
unique_ptr<Node_Expression>	Parser::Parse_OptionalExpression()
{
	Token t = Peek();

	if (t.EqualTo(TokenType::Identifier)
	 || t.EqualTo(TokenType::Value)
	 || t.EqualTo(TokenType::Delimiter, "("))
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

	BinaryOperator nextOp = StringToBinaryOp(Peek().content);

	// 后续加减表达式
	while (Peek().EqualTo(TokenType::Operator) && OpIsComparison(nextOp))
	{
		auto expr_temp = make_unique<Node_BinaryExpression>();

		expr_temp->left = move(expr);
		expr_temp->op = StringToBinaryOp(Consume(TokenType::Operator).content);
		expr_temp->right = Parse_AddSubExpression();

		nextOp = StringToBinaryOp(Peek().content);

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

	BinaryOperator nextOp = StringToBinaryOp(Peek().content);

	// 后续项
	while (Peek().EqualTo(TokenType::Operator) && OpIsAddSub(nextOp))
	{
		auto expr_temp = make_unique<Node_BinaryExpression>();

		expr_temp->left = move(expr);
		expr_temp->op = StringToBinaryOp(Consume(TokenType::Operator).content);
		expr_temp->right = Parse_Term();

		nextOp = StringToBinaryOp(Peek().content);

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

	BinaryOperator nextOp = StringToBinaryOp(Peek().content);

	// 后续因子
	while (Peek().EqualTo(TokenType::Operator) && OpIsMultDiv(nextOp))
	{
		auto expr_temp = make_unique<Node_BinaryExpression>();
		
		expr_temp->left  = move(expr);
		expr_temp->op    = StringToBinaryOp(Consume(TokenType::Operator).content);
		expr_temp->right = Parse_Oprand();

		nextOp = StringToBinaryOp(Peek().content);

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
	else if (t.EqualTo(TokenType::Delimiter, "("))
	{
		Consume(TokenType::Delimiter, "(");
		auto expr = Parse_Expression();
		Consume(TokenType::Delimiter, ")");

		return expr;
	}
	else if (t.EqualTo(TokenType::Identifier))
	{
		if (PeekNext().EqualTo(TokenType::Delimiter, "("))
		{
			auto call = make_unique<Node_CallExpression>();

			call->name = Consume(TokenType::Identifier).content;
			Consume(TokenType::Delimiter, "(");
			call->argumentList = Parse_ArgumentList();
			Consume(TokenType::Delimiter, ")");

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
		|| t.EqualTo(TokenType::Delimiter, "("))
	{
		argumentList.push_back(Parse_Expression());

		while (Peek().EqualTo(TokenType::Separator, ","))
		{
			Consume(TokenType::Separator, ",");
			argumentList.push_back(Parse_Expression());
		}
	}

	return argumentList;
}


/************************************************
* Advance：语法分析，返回抽象语法树根节点
************************************************/
const ASTNode* Parser::Parse()
{
	_root = Parse_Program();

	if (!IsAtEnd())
	{
		AddError(ParserErrorType::UnterminatedProgram, Token(TokenType::NullType, "", SourceLocation()));
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