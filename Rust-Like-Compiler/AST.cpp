#include "AST.h"

/************************************************
* StringToBinaryOp: 字符串 -> 二元运算符枚举
************************************************/
BinaryOperator StringToBinaryOp(const string& s)
{
	return (s == "+") ? BinaryOperator::Add :
		(s == "-") ? BinaryOperator::Sub :
		(s == "*") ? BinaryOperator::Mult :
		(s == "/") ? BinaryOperator::Div :
		(s == "<") ? BinaryOperator::LT :
		(s == "<=") ? BinaryOperator::LE :
		(s == ">") ? BinaryOperator::GT :
		(s == ">=") ? BinaryOperator::GE :
		(s == "==") ? BinaryOperator::EQ :
		(s == "!=") ? BinaryOperator::NE : BinaryOperator::NullType;
}

/************************************************
* BinaryOpToString:         二元运算符枚举 -> 字符串
* PrimitiveTypeToString:    数据类型枚举   -> 字符串
* VariablePropertyToString: 变量属性枚举   -> 字符串
************************************************/
string BinaryOpToString(BinaryOperator op)
{
	switch (op) {
	case BinaryOperator::Add:
		return "+";
	case BinaryOperator::Sub:
		return "-";
	case BinaryOperator::Mult:
		return "*";
	case BinaryOperator::Div:
		return "/";
	case BinaryOperator::LT:
		return "<";
	case BinaryOperator::EQ:
		return "==";
	default:
		return "unknown_op";
	}
}
string PrimitiveTypeToString(PrimitiveType type)
{
	switch (type) {
	case PrimitiveType::I32:
		return "i32";
	default:
		return "unknown_type";
	}
}
string VariablePropertyToString(VariableProperty property)
{
	switch (property) {
	case VariableProperty::Mut :
		return "mut";
	default:
		return "unknown_property";
	}
}

/************************************************
* OpIsAddSub:     是否为 + |-
* OpIsMultDiv:    是否为 * | /
* OpIsComparison: 是否为 < | > | <= | >= | == | !=
************************************************/
bool OpIsAddSub(BinaryOperator op)
{
	return (op == BinaryOperator::Add || op == BinaryOperator::Sub);
}
bool OpIsMultDiv(BinaryOperator op)
{
	return (op == BinaryOperator::Mult || op == BinaryOperator::Div);
}
bool OpIsComparison(BinaryOperator op)
{
	return (op == BinaryOperator::LT || op == BinaryOperator::LE
		|| op == BinaryOperator::GT || op == BinaryOperator::GE
		|| op == BinaryOperator::EQ || op == BinaryOperator::NE);
}