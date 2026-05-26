#include "Type.h"

/************************************************
* TokenToBinaryOp: Token -> 二元运算符枚举
************************************************/
BinaryOperator TokenToBinaryOp(const Token& s)
{
	return (s.type == TokenType::Operator_Add) ? BinaryOperator::Add :
		(s.type == TokenType::Operator_Sub) ? BinaryOperator::Sub :
		(s.type == TokenType::Operator_Mult) ? BinaryOperator::Mult :
		(s.type == TokenType::Operator_Div) ? BinaryOperator::Div :
		(s.type == TokenType::Operator_Lt) ? BinaryOperator::LT :
		(s.type == TokenType::Operator_Le) ? BinaryOperator::LE :
		(s.type == TokenType::Operator_Gt) ? BinaryOperator::GT :
		(s.type == TokenType::Operator_Ge) ? BinaryOperator::GE :
		(s.type == TokenType::Operator_Eq) ? BinaryOperator::EQ :
		(s.type == TokenType::Operator_Ne) ? BinaryOperator::NE : BinaryOperator::NullType;
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
	case BinaryOperator::EQ:
		return "==";
	case BinaryOperator::LT:
		return "<";
	case BinaryOperator::LE:
		return "<=";
	case BinaryOperator::GT:
		return ">";
	case BinaryOperator::GE:
		return ">=";
	case BinaryOperator::NE:
		return "!=";
	default:
		return "unknown_op";
	}
}
string DataTypeToString(DataType type)
{
	switch (type) {
	case DataType::I32:
		return "i32";
	default:
		return "unknown_type";
	}
}
string VariablePropertyToString(VariableProperty property)
{
	switch (property) {
	case VariableProperty::Mut:
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