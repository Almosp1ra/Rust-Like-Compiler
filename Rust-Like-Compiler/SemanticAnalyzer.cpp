#include "SemanticAnalyzer.h"

/************************************************
* GlobalAnalyze: 扫描一遍 AST，构造全局符号表中的函数表项
************************************************/
void SemanticAnalyzer::GlobalAnalyze(ASTNode* root)
{
	auto program = dynamic_cast<Node_Program*>(root);
	if (program == nullptr)
	{
		// 报错：非程序根节点;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::InternalError,
			"输入分析的 AST 节点非程序根节点",
			root->begin,
			root->end
		));

		return;
	}

	// 解析函数声明结点
	const auto& declarations = program->declarations;

	for (size_t i = 0; i < declarations.size(); i++)
	{
		auto decl = dynamic_cast<const Node_FunctionDeclaration*>(declarations[i].get());
		if (decl != nullptr)
		{
			decl->header->SemanticAnalyze(this);	// 解析头结点 + 参数
		}
		else
		{
			// 报错：未知声明;
			_errorList.push_back(SemanticError(
				ErrorLevel::Error,
				SemanticErrorType::InternalError,
				"未知声明",
				declarations[i]->begin,
				declarations[i]->end
			));
		}
	}
}

/************************************************
* ComprehensiveAnalyze: 扫描一遍 AST，构造整张符号表，并执行静态语义检查
************************************************/
void SemanticAnalyzer::ComprehensiveAnalyze(ASTNode* root)
{
	auto program = dynamic_cast<Node_Program*>(root);
	if (program == nullptr)
	{
		// 报错：非程序根节点;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::InternalError,
			"输入分析的 AST 节点非程序根节点",
			root->begin,
			root->end
		));

		return;
	}

	program->SemanticAnalyze(this);
}


/************************************************
* FinalAnalyze: 扫描两遍 AST 后进行收尾检查
************************************************/
void SemanticAnalyzer::FinalAnalyze()
{
	// 扫描符号表，发现类型未确定的变量
	for (size_t i = 0; i < _symbolTable.symbols.size(); i++)
	{
		auto symbol = _symbolTable.symbols[i].get();

		auto var = dynamic_cast<Symbol_Var*>(symbol);
		if (var && var->isAutoType && !var->isInitialized)
		{
			// 报错：无法推导变量的类型;
			_errorList.push_back(SemanticError(
				ErrorLevel::Error,
				SemanticErrorType::CannotInferType,
				"无法推导变量的类型: " + var->name,
				var->begin,
				var->end
			));
		}
	}
}


/************************************************
* AnalyzeNode: 分析结点，检查语义、构造符号表
************************************************/
/* 变量属性（mut） */
void SemanticAnalyzer::AnalyzeNode(Node_VariableProperty* node)
{
}
/* 类型（i32） */
void SemanticAnalyzer::AnalyzeNode(Node_PrimitiveType* node)
{
}

/* 左值，查找和记录左值符号 */
void SemanticAnalyzer::AnalyzeNode(Node_LeftValue_Identifier* node)
{
	// 查找符号表，判断符号是否存在
	Symbol* symbol = _symbolTable.currentScope->LookupSymbol(node->name);

	if (symbol == nullptr)
	{
		// 报错：未定义的符号;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::UndefinedSymbol,
			"未定义的符号: " + node->name,
			node->begin,
			node->end
		));

		return;
	}

	// 验证符号可以作为左值使用
	if (!symbol->CanBeLeftValue())
	{
		// 报错：符号不能作为左值;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::InvalidLeftValue,
			"符号不能作为左值: " + node->name,
			node->begin,
			node->end
		));

		return;
	}

	node->symbol = symbol;
}

/* 程序段，遍历检查每个声明 */
void SemanticAnalyzer::AnalyzeNode(Node_Program* node)
{
	auto& decls = node->declarations;

	for (int i = 0; i < decls.size(); i++)
	{
		decls[i]->SemanticAnalyze(this);
	}
}

/* 函数，解析函数头、形参、函数体 */
void SemanticAnalyzer::AnalyzeNode(Node_FunctionDeclaration* node)
{
	// 函数头其实在第一遍的时候解析过，这里只查表获取函数符号
	Symbol* symbol = _symbolTable.globalScope->LookupSymbol(node->header->name);
	auto function = dynamic_cast<Symbol_Function*>(symbol);

	if (function == nullptr)
	{
		// 报错：不存在的函数符号，需要检查第一遍扫描;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::InternalError,
			"无法识别函数符号（" + node->header->name + "），可能是全局语义检查时出错",
			node->header->begin,
			node->header->end
		));

		return;
	}

	_currentFunctionSymbol = function;
	node->symbol = function;

	Scope* preScope = _symbolTable.currentScope;
	_symbolTable.currentScope = function->bodyScope; // 进入函数作用域

	// 函数体
	_currentReturnType = DataType::Void;
	_hasReturn = false;

	node->body->SemanticAnalyze(this);

	// 检查返回类型
	if (_currentReturnType != _currentFunctionSymbol->dataType)
	{
		// 报错：函数未获得预期的返回类型;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::MissingReturn,
			"函数未获得预期的返回类型: " + node->header->name,
			node->header->begin,
			node->header->end
		));
	}

	_symbolTable.currentScope = preScope; // 退出函数作用域

	_currentFunctionSymbol = nullptr;
}

/* 函数头，加入函数符号 */
void SemanticAnalyzer::AnalyzeNode(Node_FunctionHeader* node)
{
	// 基本符号信息
	string name = node->name;	// 函数名

	DataType dataType = DataType::NullType;	// 函数类型

	if (node->type)
	{
		auto primitiveType = dynamic_cast<const Node_PrimitiveType*>(node->type.get());
		if (primitiveType == nullptr)
		{
			// 报错：未知函数数据类型;
			_errorList.push_back(SemanticError(
				ErrorLevel::Error,
				SemanticErrorType::InternalError,
				"未知函数类型",
				node->begin,
				node->end
			));
		}
		else
		{
			dataType = primitiveType->type;
		}
	}
	else
	{
		dataType = DataType::Void;
	}

	// 检查重复定义
	auto old = _symbolTable.currentScope->LookupSymbolInThisScope(name);

	if (old != nullptr)	// 不允许函数重复定义
	{
		// 报错: 符号重定义;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::RedefinedSymbol,
			"符号重定义: " + name,
			node->begin,
			node->end
		));
		return;
	}

	Symbol_Function function(name, dataType, _symbolTable.currentScope, node->begin, node->end);
	auto symbol = _symbolTable.InsertSymbol(function);

	_currentFunctionSymbol = symbol;

	_symbolTable.EnterScope();	// 进入函数作用域
	_currentFunctionSymbol->bodyScope = _symbolTable.currentScope;

	// 参数列表
	const auto& paramList = node->parameterList;

	for (size_t i = 0; i < paramList.size(); i++)
	{
		paramList[i]->SemanticAnalyze(this);
	}
	
	_currentFunctionSymbol->paramCount = (int)_currentFunctionSymbol->params.size();

	_symbolTable.ExitScope();	// 退出函数作用域

	_currentFunctionSymbol = nullptr;
}

/* 形参，加入形参符号 */
void SemanticAnalyzer::AnalyzeNode(Node_Parameter* node)
{
	// 基本符号信息
	string name = node->name;	// 形参名

	DataType dataType = DataType::NullType;	// 形参类型

	auto paramType = dynamic_cast<const Node_PrimitiveType*>(node->type.get());
	if (paramType == nullptr)
	{
		// 报错：未知参数类型;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::InternalError,
			"未知参数类型 ",
			node->begin,
			node->end
		));

		return;
	}
	else
	{
		dataType = paramType->type;
	}

	// 检查重复定义
	auto old = _symbolTable.currentScope->LookupSymbolInThisScope(name);

	if (old != nullptr)	// 不允许形参重复定义
	{
		// 报错: 符号重定义;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::RedefinedSymbol,
			"符号重定义: " + name,
			node->begin,
			node->end
		));

		return;
	}

	Symbol_Param param(name, dataType, _symbolTable.currentScope, node->begin, node->end);

	// 是否为const参数
	if (node->varProperty->property != VariableProperty::Mut)
	{
		param.isConst = true;
	}

	// 插入
	auto symbol = _symbolTable.InsertSymbol(param);

	_currentFunctionSymbol->params.push_back(symbol);
	node->symbol = symbol;
}

/* 语句块，确定作用域、逐条分析语句 */
void SemanticAnalyzer::AnalyzeNode(Node_StatementBlock* node)
{
	node->scope = _symbolTable.currentScope;

	bool hasUnreachableStatementWarning = false;

	for (size_t i = 0; i < node->statements.size(); i++)
	{
		if (_hasReturn && !hasUnreachableStatementWarning)
		{
			// 警告：不可到达的语句;
			_errorList.push_back(SemanticError(
				ErrorLevel::Warning,
				SemanticErrorType::UnreachableStatement,
				"不可到达的语句",
				node->begin,
				node->end
			));

			hasUnreachableStatementWarning = true;
		}

		node->statements[i]->SemanticAnalyze(this);
	}
}

/* 空语句 */
void SemanticAnalyzer::AnalyzeNode(Node_EmptyStatement* node)
{
}

/* 返回语句，检查返回类型和当前函数数据类型是否一致 */
void SemanticAnalyzer::AnalyzeNode(Node_ReturnStatement* node)
{
	// 返回语句本身语义
	auto returnType = DataType::NullType;

	if (node->expr)
	{
		node->expr->SemanticAnalyze(this);

		if (node->expr->dataType == DataType::NullType)	// 表达式出错，直接返回
		{
			return;
		}

		returnType = node->expr->dataType;
	}
	else
	{
		returnType = DataType::Void;
	}

	if (returnType != _currentFunctionSymbol->dataType)
	{
		// 报错：表达式与函数预期返回类型不一致;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::InvalidReturnType,
			"返回类型与函数预期不一致",
			node->begin,
			node->end
		));

		return;
	}

	// 对于当前分析过程的语义
	if (_hasReturn)	// 已有返回语句时，不改变返回语句类型
	{
		return;
	}

	_currentReturnType = returnType;
	_hasReturn = true;
}

/* 变量声明语句，加入变量符号 */
void SemanticAnalyzer::AnalyzeNode(Node_VariableDeclarationStatement* node)
{
	// 基本符号信息
	string name = node->name;	// 变量名

	DataType dataType = DataType::NullType;	// 变量类型

	if (node->type)
	{
		auto primitiveType = dynamic_cast<const Node_PrimitiveType*>(node->type.get());
		if (primitiveType == nullptr)
		{
			// 报错：未知变量类型;
			_errorList.push_back(SemanticError(
				ErrorLevel::Error,
				SemanticErrorType::InternalError,
				"未知变量类型",
				node->begin,
				node->end
			));

			return;
		}
		else
		{
			dataType = primitiveType->type;
		}
	}

	// 检查重复定义
	auto old = _symbolTable.currentScope->LookupSymbolInThisScope(name);

	if (old != nullptr)	// 允许变量重复声明，但是不允许和函数、形参等其他符号重名
	{
		if (dynamic_cast<Symbol_Var*>(old) == nullptr)
		{
			// 报错: 符号重定义;
			_errorList.push_back(SemanticError(
				ErrorLevel::Error,
				SemanticErrorType::RedefinedSymbol,
				"符号重定义: " + name,
				node->begin,
				node->end
			));

			return;
		}
	}

	Symbol_Var var(name, dataType, _symbolTable.currentScope, node->begin, node->end);

	// 是否为const参数
	if (node->varProperty->property != VariableProperty::Mut)
	{
		var.isConst = true;
	}

	// 是否需要编译器推导类型
	if (!node->type)
	{
		var.isAutoType = true;
	}

	// 插入
	auto symbol = _symbolTable.InsertSymbol(var);

	node->symbol = symbol;
}

/* 赋值语句，检查符号表 */
void SemanticAnalyzer::AnalyzeNode(Node_AssignmentStatement* node)
{
	node->leftValue->SemanticAnalyze(this);	// 检查左值

	// 左值识别出错，直接返回
	if (node->leftValue->symbol == nullptr)
	{
		return;
	}

	Symbol* symbol = node->leftValue->symbol;

	if (symbol->isConst)	// 常量不能被赋值
	{
		// 报错：不能赋值常量;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::AssignToConst,
			"不能对常量赋值: " + symbol->name,
			node->begin,
			node->end
		));

		return;
	}

	node->expr->SemanticAnalyze(this);	// 检查右侧表达式

	if (node->expr->dataType == DataType::NullType)	// 表达式检查出错，直接返回
	{
		return;
	}

	// 未定类型变量：第一次赋值时确定类型
	auto var = dynamic_cast<Symbol_Var*>(symbol);
	if (var && var->isAutoType && !var->isInitialized)
	{
		var->dataType = node->expr->dataType;
		var->isInitialized = true;

		return;
	}

	// 已知类型：直接检查
	if (symbol->dataType != node->expr->dataType)	// 左右类型不一致，实际上只有i32这一种类型所以不会出现这种情况
	{
		// 报错：表达式类型与变量不一致;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::TypeMismatch,
			"表达式类型与变量不一致: " + symbol->name,
			node->begin,
			node->end
		));

		return;
	}

	if (var)
	{
		var->isInitialized = true;
	}
}

/* 表达式语句 */
void SemanticAnalyzer::AnalyzeNode(Node_ExpressionStatement* node)
{
	node->expr->SemanticAnalyze(this);
}

/* if语句 */
/* if 的返回类型怎么判断？
 * 如果某一块里返回错误的类型，return 语句自己会报错
 * 除非没有 return 语句，这时候分析会当成返回 void，如果要求返回非空类型但是某个分支没有返回的话，不能视为经过这段if之后函数一定有正确的返回类型
 * 因此 if 只需要检查所有路径的返回类型，如果其中有 void，则 currentReturnType 应该保持和分析 if 前一致
 */
void SemanticAnalyzer::AnalyzeNode(Node_IfStatement* node)
{
	node->condition->SemanticAnalyze(this);	// 检查条件表达式

	// 保护当前分析的返回状态
	DataType preReturnType = _currentReturnType;
	bool preHasReturn = _hasReturn;

	// then部分
	_currentReturnType = DataType::Void;
	_hasReturn = false;

	_symbolTable.EnterScope();	// 进入函数作用域
	node->thenBlock->SemanticAnalyze(this);
	_symbolTable.ExitScope();	// 退出函数作用域

	DataType thenReturnType = _currentReturnType;

	// else部分
	_currentReturnType = DataType::Void;
	_hasReturn = false;

	if (node->elseBlock != nullptr)
	{
		_symbolTable.EnterScope();	// 进入函数作用域
		node->elseBlock->SemanticAnalyze(this);
		_symbolTable.ExitScope();	// 退出函数作用域
	}

	DataType elseReturnType = _currentReturnType;

	// 分析if的返回类型
	if (thenReturnType == elseReturnType && thenReturnType != DataType::Void)	// 只有两条路径返回类型相同且不为void，才视为if对返回状态有影响
	{
		_currentReturnType = thenReturnType;
		_hasReturn = true;
	}
	else
	{
		_currentReturnType = preReturnType;
		_hasReturn = preHasReturn;
	}
}

/* while语句 */
/* while 的返回类型怎么判断？
 * while 不改变 currentReturnType，因为没法保证进入 while 块
 */
void SemanticAnalyzer::AnalyzeNode(Node_WhileStatement* node)
{
	node->condition->SemanticAnalyze(this);	// 检查条件表达式

	// 保护当前分析的返回状态
	DataType preReturnType = _currentReturnType;
	bool preHasReturn = _hasReturn;

	// 循环体
	_currentReturnType = DataType::Void;
	_hasReturn = false;

	_symbolTable.EnterScope();	// 进入函数作用域
	node->body->SemanticAnalyze(this);
	_symbolTable.ExitScope();	// 退出函数作用域

	// 恢复返回状态
	_currentReturnType = preReturnType;		
	_hasReturn = preHasReturn;
}

/* 数值表达式 */
void SemanticAnalyzer::AnalyzeNode(Node_NumberExpression* node)
{
	node->dataType = DataType::I32;
}

/* 变量表达式 */
void SemanticAnalyzer::AnalyzeNode(Node_VariableExpression* node)
{
	node->dataType = DataType::NullType;

	node->leftValue->SemanticAnalyze(this);	// 识别左值

	// 左值识别出错，直接返回
	if (node->leftValue->symbol == nullptr)
	{
		node->dataType = DataType::NullType;
		return;
	}

	// 变量使用前必须先赋值
	auto var = dynamic_cast<Symbol_Var*>(node->leftValue->symbol);
	if (var && !var->isInitialized)
	{
		// 报错：变量未初始化;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::UninitializedVariable,
			"不能使用未初始化的变量: " + var->name,
			node->begin,
			node->end
		));

		return;
	}

	node->dataType = node->leftValue->symbol->dataType;
}

/* 函数调用表达式 */
void SemanticAnalyzer::AnalyzeNode(Node_CallExpression* node)
{
	// 1. 查表，找到函数符号
	Symbol* symbol = _symbolTable.globalScope->LookupSymbol(node->name);
	auto function = dynamic_cast<Symbol_Function*>(symbol);

	if (function == nullptr)
	{
		// 报错：不存在的函数符号;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::FunctionNotFound,
			"不存在的函数符号: " + node->name,
			node->begin,
			node->end
		));

		return;
	}

	// 不应该进行下面的检查，应该允许void类型的函数作为表达式，但不允许进一步参与运算
	//if (function->dataType == DataType::Void)
	//{
	//	// 报错：无返回值的函数不能作为表达式;
	//	_errorList.push_back(SemanticError(
	//		ErrorLevel::Error,
	//		SemanticErrorType::VoidFunctionUsedAsExpression,
	//		"无返回值的函数不能作为表达式: " + node->name,
	//		node->begin,
	//		node->end
	//	));

	//	return;
	//}

	node->symbol = function;

	// 2. 检查参数个数和类型
	const auto& argList = node->argumentList;

	if (argList.size() != node->symbol->paramCount)
	{
		// 报错：函数参数个数与声明不符;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::ArgumentCountMismatch,
			"函数参数个数不匹配: " + node->name,
			node->begin,
			node->end
		));

		return;
	}

	for (size_t i = 0; i < argList.size(); i++)
	{
		argList[i]->SemanticAnalyze(this);

		if (argList[i]->dataType != node->symbol->params[i]->dataType)
		{
			// 报错：函数参数类型与声明不符;
			_errorList.push_back(SemanticError(
				ErrorLevel::Error,
				SemanticErrorType::ArgumentTypeMismatch,
				"函数参数类型不匹配: " + node->name,
				node->begin,
				node->end
			));

			return;
		}
	}

	node->dataType = node->symbol->dataType;
}

/* 二元运算表达式 */
void SemanticAnalyzer::AnalyzeNode(Node_BinaryExpression* node)
{
	// 左表达式
	node->left->SemanticAnalyze(this);

	// 右表达式
	node->right->SemanticAnalyze(this);

	// 子表达式有错误，直接返回
	if (node->left->dataType == DataType::NullType || node->right->dataType == DataType::NullType)
	{
		node->dataType = DataType::NullType;
		return;
	}

	// 检查两侧类型一致
	if (node->left->dataType != node->right->dataType)
	{
		// 报错：二元表达式左右类型不一致;
		_errorList.push_back(SemanticError(
			ErrorLevel::Error,
			SemanticErrorType::TypeMismatch,
			"无法参与计算的类型",
			node->begin,
			node->end
		));

		node->dataType = DataType::NullType;
		return;
	}

	node->dataType = node->left->dataType;	// 因为现在只有I32，不需要根据运算符确定最终类型
}

/************************************************
* SemanticAnalyze: 完成整个语义检查过程，生成符号表和错误列表
************************************************/
void SemanticAnalyzer::SemanticAnalyze(ASTNode* root)
{
	GlobalAnalyze(root);

	ComprehensiveAnalyze(root);

	FinalAnalyze();
}

const SymbolTable& SemanticAnalyzer::GetSymbolTable()
{
	return _symbolTable;
}

const vector<SemanticError>& SemanticAnalyzer::GetErrorList()
{
	return _errorList;
}
