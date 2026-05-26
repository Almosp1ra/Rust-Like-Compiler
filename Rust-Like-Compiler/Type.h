#pragma once
#pragma once
#include <string>
#include "Token.h"
using namespace std;

/*
 * 枚举类型定义
 */

 // 数据类型
enum class DataType {
	NullType,
	I32
};

// 变量属性
enum class VariableProperty {
	NullType,
	Mut
};

// 二元运算符
enum class BinaryOperator {
	NullType,
	Add,
	Sub,
	Mult,
	Div,
	LT,
	LE,
	GT,
	GE,
	EQ,
	NE
};

// 工具函数，枚举类型和 string 之间互相转换
BinaryOperator TokenToBinaryOp(const Token& s);
string BinaryOpToString(BinaryOperator op);
string DataTypeToString(DataType type);
string VariablePropertyToString(VariableProperty property);

// 判断二元运算符是否归属某一类
bool OpIsAddSub(BinaryOperator op);
bool OpIsMultDiv(BinaryOperator op);
bool OpIsComparison(BinaryOperator op);
