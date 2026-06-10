#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "SourceLocation.h"
#include "Type.h"
using namespace std;

class Scope;

/*
 * 符号项
 */

class Symbol
{
	// 基础字段
public:
	string name;		// 符号名
	string uniqueName;	// 全局唯一符号名
	DataType dataType;	// 符号的数据类型
	Scope* scope;		// 符号所在作用域
	SourceLocation begin;	// 源代码位置
	SourceLocation end;

	Symbol(const string& name, DataType dataype, Scope* scope, const SourceLocation& begin, const SourceLocation& end);
	virtual ~Symbol() = default;

	virtual bool CanBeLeftValue();

	// 扩展字段
public:
	bool isConst = false;

};

class Symbol_Var : public Symbol
{
public:
	bool isAutoType = false;	// 编译器推导类型
	bool isInitialized = false;	// 是否初始化

	Symbol_Var(const string& name, DataType dataype, Scope* scope, const SourceLocation& begin, const SourceLocation& end);

	bool CanBeLeftValue() override;
};

class Symbol_Function : public Symbol
{
public:
	int paramCount = 0;				// 参数个数
	vector<Symbol_Param*> params;			// 参数符号列表

	Scope* bodyScope = nullptr;     // 函数对应内部作用域的指针

	Symbol_Function(const string& name, DataType dataype, Scope* scope, const SourceLocation& begin, const SourceLocation& end);
};

class Symbol_Param : public Symbol
{
public:
	Symbol_Param(const string& name, DataType dataype, Scope* scope, const SourceLocation& begin, const SourceLocation& end);

	bool CanBeLeftValue() override;
};

class Symbol_Array : public Symbol
{
public:
	bool isArray = false;
	int arraySize = 0;

	Symbol_Array(const string& name, DataType dataype, Scope* scope, const SourceLocation& begin, const SourceLocation& end);
};


/*
 * 作用域
 */
class Scope
{
public:
	size_t id;	// 唯一标识符，用于全局识别作用域、生成全局唯一符号名
	size_t level;	// 作用域深度
	Scope* parent;	// 外层作用域

	unordered_map<string, Symbol*> symbols;	// 该作用域的符号列表

	Scope(size_t id, size_t level, Scope* parent);

	// 查找符号
	Symbol* LookupSymbol(const string& name);
	Symbol* LookupSymbolInThisScope(const string& name);
};

/*
 * 符号表
 */
class SymbolTable
{
public:
	vector<unique_ptr<Symbol>> symbols;		// 符号项列表，统一管理
	vector<unique_ptr<Scope>> scopes;		// 作用域列表，统一管理

	Scope* globalScope;          // 全局作用域
	Scope* currentScope;         // 当前作用域

	size_t scopeCounter;

	SymbolTable();

	void EnterScope();	// 新建并进入作用域
	void ExitScope();	// 退出当前作用域

	template<typename SymbolType>
	SymbolType* InsertSymbol(const SymbolType& symbol)
	{
		static_assert(is_base_of_v<Symbol, SymbolType>);

		auto& table = currentScope->symbols;

		// 插入符号表
		symbols.push_back(make_unique<SymbolType>(symbol));	// 构造唯一符号项

		SymbolType* ptr = static_cast<SymbolType*>(symbols.back().get());
		table[symbol.name] = ptr;	// 覆盖当前绑定

		return ptr;
	}
};