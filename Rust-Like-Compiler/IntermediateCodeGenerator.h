#pragma once
#include <string>
#include <vector>
#include "AST.h"

using namespace std;

struct Quadruple
{
    string op;
    string arg1;
    string arg2;
    string result;

    Quadruple(const string& op, const string& arg1, const string& arg2, const string& result)
        : op(op), arg1(arg1), arg2(arg2), result(result)
    {
    }
};

class IntermediateCodeGenerator
{
private:
    vector<Quadruple> _quadruples;
    int _tempCounter = 0;
    int _labelCounter = 0;

private:
    string NewTemp();
    string NewLabel();

    void Emit(const string& op, const string& arg1, const string& arg2, const string& result);

    void GenerateNode(ASTNode* node);
    void GenerateFunction(Node_FunctionDeclaration* node);
    void GenerateStatementBlock(Node_StatementBlock* node);
    void GenerateStatement(Node_Statement* node);

    string GenerateExpression(Node_Expression* node);
    string GenerateLeftValue(Node_LeftValue* node);

    string OperatorToString(BinaryOperator op);

public:
    void Generate(ASTNode* root);
    const vector<Quadruple>& GetQuadruples() const;
};