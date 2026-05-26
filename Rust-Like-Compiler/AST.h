#pragma once
#include <memory>
#include <vector>
#include <string>
#include "Token.h"
#include "Type.h"
using namespace std;

/*
 * 抽象语法树节点设计 
 * 通过继承基类 ASTNode, 构造子类
 * 子类根据节点性质（即语义）持有其他 ASTNode 子类的指针
 */

// 节点基类
struct ASTNode;
struct Node_Error;

// 类型
struct Node_Type;
struct Node_PrimitiveType;

// 变量属性
struct Node_VariableProperty;

// 左值
struct Node_LeftValue;
struct Node_LeftValue_Identifier;

// 程序
struct Node_Program;

// 声明
struct Node_Declaration;
struct Node_FunctionDeclaration;
struct Node_FunctionHeader;

// 参数
struct Node_Parameter;

// 语句块
struct Node_Statement;
struct Node_StatementBlock;
struct Node_EmptyStatement;

// 返回语句
struct Node_ReturnStatement;

// 变量声明和赋值语句
struct Node_VariableDeclarationStatement;
struct Node_AssignmentStatement;
struct Node_ExpressionStatement;
struct Node_IfStatement;
struct Node_LoopStatement;
struct Node_WhileStatement;

// 表达式
struct Node_Expression;
struct Node_NumberExpression;
struct Node_VariableExpression;
struct Node_CallExpression;
struct Node_BinaryExpression;

// 节点基类
struct ASTNode
{
	virtual ~ASTNode() = default;
};
struct Node_Error : ASTNode
{
};

// 类型
struct Node_Type : ASTNode
{
};
struct Node_PrimitiveType : Node_Type
{
	DataType type;
};

// 变量属性
struct Node_VariableProperty : ASTNode
{
	VariableProperty property;
};

// 左值
struct Node_LeftValue : ASTNode
{
};
struct Node_LeftValue_Identifier : Node_LeftValue
{
	string name;
};

// 程序
struct Node_Program : ASTNode
{
	vector<unique_ptr<Node_Declaration>> declarations;
};

// 声明
struct Node_Declaration : ASTNode
{
};
struct Node_FunctionDeclaration : Node_Declaration
{
	unique_ptr<Node_FunctionHeader> header;
	unique_ptr<Node_StatementBlock> body;
};
struct Node_FunctionHeader : ASTNode
{
	string name;
	vector<unique_ptr<Node_Parameter>> parameterList;
	unique_ptr<Node_Type> type;
};

// 参数
struct Node_Parameter : ASTNode
{
	unique_ptr<Node_VariableProperty>  varProperty;
	string name;
	unique_ptr<Node_Type> type;
};

// 语句块
struct Node_Statement : ASTNode
{
};
struct Node_StatementBlock : Node_Statement
{
	vector<unique_ptr<Node_Statement>>  statements;
};
struct Node_EmptyStatement : Node_Statement
{
};

// 返回语句
struct Node_ReturnStatement : Node_Statement
{
	unique_ptr<Node_Expression> expr;
};

// 变量声明和赋值语句
struct Node_VariableDeclarationStatement : Node_Statement
{
	unique_ptr<Node_VariableProperty>  varProperty;
	string name;
	unique_ptr<Node_Type> type;
};
struct Node_AssignmentStatement : Node_Statement
{
	unique_ptr<Node_LeftValue> leftValue;
	unique_ptr<Node_Expression> expr;
};
struct Node_ExpressionStatement : Node_Statement
{
	unique_ptr<Node_Expression> expr;
};
struct Node_IfStatement : Node_Statement
{
	unique_ptr<Node_Expression> condition;
	unique_ptr<Node_StatementBlock> thenBlock;
	unique_ptr<Node_StatementBlock> elseBlock;
};
struct Node_LoopStatement : Node_Statement
{
};
struct Node_WhileStatement : Node_LoopStatement
{
	unique_ptr<Node_Expression> condition;
	unique_ptr<Node_StatementBlock> body;
};

// 表达式
struct Node_Expression : ASTNode
{
};
struct Node_NumberExpression : Node_Expression
{
	string value;
};
struct Node_VariableExpression : Node_Expression
{
	unique_ptr<Node_LeftValue> leftValue;
};
struct Node_CallExpression : Node_Expression
{
	string name;
	vector<unique_ptr<Node_Expression>> argumentList;
};
struct Node_BinaryExpression : Node_Expression
{
	BinaryOperator op;
	unique_ptr<Node_Expression> left;
	unique_ptr<Node_Expression> right;
};
