#include "SymbolTable.h"


/*
* Symbol
*/

/************************************************
* Symbol构造
************************************************/
Symbol::Symbol(const string& name, DataType dataType, Scope* scope, const SourceLocation& begin, const SourceLocation& end)
	: name(name), dataType(dataType), scope(scope), begin(begin), end(end)
{
	uniqueName = name + "_s" + to_string(scope->id);
}

Symbol_Var::Symbol_Var(const string& name, DataType dataType, Scope* scope, const SourceLocation& begin, const SourceLocation& end)
	: Symbol(name, dataType, scope, begin, end)
{
}

Symbol_Function::Symbol_Function(const string& name, DataType dataType, Scope* scope, const SourceLocation& begin, const SourceLocation& end)
	: Symbol(name, dataType, scope, begin, end)
{
}

Symbol_Param::Symbol_Param(const string& name, DataType dataType, Scope* scope, const SourceLocation& begin, const SourceLocation& end)
	: Symbol(name, dataType, scope, begin, end)
{
}

Symbol_Array::Symbol_Array(const string& name, DataType dataType, Scope* scope, const SourceLocation& begin, const SourceLocation& end)
	: Symbol(name, dataType, scope, begin, end)
{
}

/************************************************
* CanBeLeftValue: 符号能作为左值使用
************************************************/
bool Symbol::CanBeLeftValue()
{
	return false;
}
bool Symbol_Var::CanBeLeftValue()
{
	return true;
}
bool Symbol_Param::CanBeLeftValue()
{
	return true;
}

/*
* Scope
*/

/************************************************
* Scope 构造
************************************************/
Scope::Scope(size_t id, size_t level, Scope* parent)
	: id(id), level(level), parent(parent)
{
}

/************************************************
* LookupSymbol: 查找符号
************************************************/
Symbol* Scope::LookupSymbol(const string& name)
{
	Scope* scope = this;

	while (scope != nullptr) {
		auto it = scope->symbols.find(name);
		if (it != scope->symbols.end())
		{
			return it->second;
		}
		scope = scope->parent;
	}

	return nullptr;
}
Symbol* Scope::LookupSymbolInThisScope(const string& name)
{
	auto it = symbols.find(name);
	if (it != symbols.end())
	{
		return it->second;
	}

	return nullptr;
}

/*
* SymbolTable
*/

/************************************************
* SymbolTable构造
************************************************/
SymbolTable::SymbolTable()
{
	scopes.push_back(make_unique<Scope>(0, 0, nullptr));	// 构造全局作用域

	globalScope = scopes[0].get();
	currentScope = globalScope;

	scopeCounter = 1;
}

/************************************************
* EnterScope: 进入作用域
* ExitScope : 退出作用域
************************************************/
void SymbolTable::EnterScope()
{
	scopes.push_back(
		make_unique<Scope>(
			scopeCounter++,
			currentScope->level + 1,
			currentScope
		)
	);	// 构造新作用域

	Scope* newScope = scopes.back().get();

	currentScope = newScope;
}

void SymbolTable::ExitScope()
{
	if (currentScope->parent != nullptr)
	{
		currentScope = currentScope->parent;
	}
}