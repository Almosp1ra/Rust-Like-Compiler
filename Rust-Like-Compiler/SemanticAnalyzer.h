#pragma once
#include "SymbolTable.h"
#include "AST.h"

/*
 * 静态语义错误
 */

enum class ErrorLevel
{
	Error,
	Warning
};

enum class SemanticErrorType
{
	// 符号相关
	UndefinedSymbol,
	RedefinedSymbol,
	InvalidLeftValue,

	// 类型相关
	TypeMismatch,
	InvalidBinaryOperand,
	InvalidReturnType,

	// 函数相关
	FunctionNotFound,
	ArgumentCountMismatch,
	ArgumentTypeMismatch,
	VoidFunctionUsedAsExpression,

	// 变量相关
	AssignToConst,
	UninitializedVariable,
	CannotInferType,

	// 控制流相关
	MissingReturn,
	UnreachableStatement,

	// 内部错误
	InternalError
};

struct SemanticError
{
	ErrorLevel level;

	SemanticErrorType type;

	string message;

	SourceLocation begin;
	SourceLocation end;

	SemanticError(ErrorLevel level, SemanticErrorType type, const string& message,
		const SourceLocation& begin, const SourceLocation& end)
		: level(level), type(type), message(message), begin(begin), end(end)
	{
	}
};

/*
 * 静态语义检查器
 */
class SemanticAnalyzer
{
private:
	SymbolTable _symbolTable;
	vector<SemanticError> _errorList;

	// 1. 扫描一遍 AST，构造全局符号表中的函数表项
	void GlobalAnalyze(ASTNode* root);

	// 2. 扫描一遍 AST，边做静态语义检查，边构造符号表
	void ComprehensiveAnalyze(ASTNode* root);

	// 3. 两遍扫完后，收尾检查
	void FinalAnalyze();

	// 语义检查上下文
	Symbol_Function* _currentFunctionSymbol = nullptr;	// 当前分析的函数
	DataType _currentReturnType = DataType::NullType;	// 当前语句块的返回类型
	bool _hasReturn = false;							// 当前语句块是否包含返回语句

public:
	// 单结点分析
	void AnalyzeNode(Node_VariableProperty* node);
	void AnalyzeNode(Node_PrimitiveType* node);
	void AnalyzeNode(Node_LeftValue_Identifier* node);
	void AnalyzeNode(Node_Program* node);
	void AnalyzeNode(Node_FunctionDeclaration* node);
	void AnalyzeNode(Node_FunctionHeader* node);
	void AnalyzeNode(Node_Parameter* node);
	void AnalyzeNode(Node_StatementBlock* node);
	void AnalyzeNode(Node_EmptyStatement* node);
	void AnalyzeNode(Node_ReturnStatement* node);
	void AnalyzeNode(Node_VariableDeclarationStatement* node);
	void AnalyzeNode(Node_AssignmentStatement* node);
	void AnalyzeNode(Node_ExpressionStatement* node);
	void AnalyzeNode(Node_IfStatement* node);
	void AnalyzeNode(Node_WhileStatement* node);
	void AnalyzeNode(Node_NumberExpression* node);
	void AnalyzeNode(Node_VariableExpression* node);
	void AnalyzeNode(Node_CallExpression* node);
	void AnalyzeNode(Node_BinaryExpression* node);

public:
	void SemanticAnalyze(ASTNode* root);

	const SymbolTable& GetSymbolTable();
	const vector<SemanticError>& GetErrorList();
};