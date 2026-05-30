#include "AST.h"
#include "SemanticAnalyzer.h"

void Node_PrimitiveType::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_VariableProperty::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_LeftValue_Identifier::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_Program::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_FunctionDeclaration::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_FunctionHeader::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_Parameter::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_StatementBlock::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_EmptyStatement::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_ReturnStatement::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_VariableDeclarationStatement::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_AssignmentStatement::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_ExpressionStatement::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_IfStatement::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_WhileStatement::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_NumberExpression::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_VariableExpression::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_CallExpression::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
void Node_BinaryExpression::SemanticAnalyze(SemanticAnalyzer* v)
{
	v->AnalyzeNode(this);
}
