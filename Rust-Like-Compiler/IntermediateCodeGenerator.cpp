#include "IntermediateCodeGenerator.h"

string IntermediateCodeGenerator::NewTemp()
{
    _tempCounter++;
    return "t" + to_string(_tempCounter);
}

string IntermediateCodeGenerator::NewLabel()
{
    _labelCounter++;
    return "L" + to_string(_labelCounter);
}

void IntermediateCodeGenerator::Emit(const string& op, const string& arg1, const string& arg2, const string& result)
{
    _quadruples.emplace_back(op, arg1, arg2, result);
}

string IntermediateCodeGenerator::OperatorToString(BinaryOperator op)
{
    switch (op)
    {
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
    case BinaryOperator::LE:
        return "<=";
    case BinaryOperator::GT:
        return ">";
    case BinaryOperator::GE:
        return ">=";
    case BinaryOperator::EQ:
        return "==";
    case BinaryOperator::NE:
        return "!=";
    default:
        return "?";
    }
}

void IntermediateCodeGenerator::Generate(ASTNode* root)
{
    _quadruples.clear();
    _tempCounter = 0;
    _labelCounter = 0;

    GenerateNode(root);
}

const vector<Quadruple>& IntermediateCodeGenerator::GetQuadruples() const
{
    return _quadruples;
}

void IntermediateCodeGenerator::GenerateNode(ASTNode* node)
{
    if (node == nullptr)
    {
        return;
    }

    if (auto program = dynamic_cast<Node_Program*>(node))
    {
        for (auto& declaration : program->declarations)
        {
            GenerateNode(declaration.get());
        }
    }
    else if (auto functionDecl = dynamic_cast<Node_FunctionDeclaration*>(node))
    {
        GenerateFunction(functionDecl);
    }
    else if (auto block = dynamic_cast<Node_StatementBlock*>(node))
    {
        GenerateStatementBlock(block);
    }
    else if (auto statement = dynamic_cast<Node_Statement*>(node))
    {
        GenerateStatement(statement);
    }
}

void IntermediateCodeGenerator::GenerateFunction(Node_FunctionDeclaration* node)
{
    if (node == nullptr || node->header == nullptr)
    {
        return;
    }

    Emit("func", "_", "_", node->header->name);

    // 新增了形参表示
    for (const auto& param : node->header->parameterList)
    {
        Emit("param", param->symbol->uniqueName, "_", "_");
    }

    GenerateStatementBlock(node->body.get());

    Emit("endfunc", "_", "_", node->header->name);
}

void IntermediateCodeGenerator::GenerateStatementBlock(Node_StatementBlock* node)
{
    if (node == nullptr)
    {
        return;
    }

    for (auto& statement : node->statements)
    {
        GenerateStatement(statement.get());
    }
}

void IntermediateCodeGenerator::GenerateStatement(Node_Statement* node)
{
    if (node == nullptr)
    {
        return;
    }

    if (dynamic_cast<Node_EmptyStatement*>(node))
    {
        return;
    }

    if (auto varDecl = dynamic_cast<Node_VariableDeclarationStatement*>(node))
    {
        // Emit("decl", "_", "_", varDecl->name);
        // 可能更应该用唯一名声明
        Emit("decl", "_", "_", varDecl->symbol->uniqueName);
    }
    else if (auto assign = dynamic_cast<Node_AssignmentStatement*>(node))
    {
        string right = GenerateExpression(assign->expr.get());
        string left = GenerateLeftValue(assign->leftValue.get());

        Emit("=", right, "_", left);
    }
    else if (auto ret = dynamic_cast<Node_ReturnStatement*>(node))
    {
        string value = "_";

        if (ret->expr != nullptr)
        {
            value = GenerateExpression(ret->expr.get());
        }

        Emit("return", value, "_", "_");
    }
    else if (auto exprStmt = dynamic_cast<Node_ExpressionStatement*>(node))
    {
        GenerateExpression(exprStmt->expr.get());
    }
    else if (auto ifStmt = dynamic_cast<Node_IfStatement*>(node))
    {
        string condition = GenerateExpression(ifStmt->condition.get());
        string falseLabel = NewLabel();
        string endLabel = NewLabel();

        Emit("jz", condition, "_", falseLabel);

        GenerateStatementBlock(ifStmt->thenBlock.get());

        if (ifStmt->elseBlock != nullptr)
        {
            Emit("jmp", "_", "_", endLabel);
            Emit("label", "_", "_", falseLabel);
            GenerateStatementBlock(ifStmt->elseBlock.get());
            Emit("label", "_", "_", endLabel);
        }
        else
        {
            Emit("label", "_", "_", falseLabel);
        }
    }
    else if (auto whileStmt = dynamic_cast<Node_WhileStatement*>(node))
    {
        string beginLabel = NewLabel();
        string endLabel = NewLabel();

        Emit("label", "_", "_", beginLabel);

        string condition = GenerateExpression(whileStmt->condition.get());
        Emit("jz", condition, "_", endLabel);

        GenerateStatementBlock(whileStmt->body.get());

        Emit("jmp", "_", "_", beginLabel);
        Emit("label", "_", "_", endLabel);
    }
}

string IntermediateCodeGenerator::GenerateExpression(Node_Expression* node)
{
    if (node == nullptr)
    {
        return "_";
    }

    if (auto numberExpr = dynamic_cast<Node_NumberExpression*>(node))
    {
        return numberExpr->value;
    }

    if (auto variableExpr = dynamic_cast<Node_VariableExpression*>(node))
    {
        return GenerateLeftValue(variableExpr->leftValue.get());
    }

    if (auto callExpr = dynamic_cast<Node_CallExpression*>(node))
    {
        vector<string> args;

        for (auto& arg : callExpr->argumentList)
        {
            args.push_back(GenerateExpression(arg.get()));
        }

        for (const auto& arg : args)
        {
            Emit("arg", arg, "_", "_");   // 实参，用arg表示
        }

        string temp = NewTemp();
        Emit("call", callExpr->name, to_string(args.size()), temp);

        return temp;
    }

    if (auto binaryExpr = dynamic_cast<Node_BinaryExpression*>(node))
    {
        string left = GenerateExpression(binaryExpr->left.get());
        string right = GenerateExpression(binaryExpr->right.get());
        string temp = NewTemp();

        Emit(OperatorToString(binaryExpr->op), left, right, temp);

        return temp;
    }

    return "_";
}

string IntermediateCodeGenerator::GenerateLeftValue(Node_LeftValue* node)
{
    if (node == nullptr)
    {
        return "_";
    }

    if (auto identifier = dynamic_cast<Node_LeftValue_Identifier*>(node))
    {
        if (identifier->symbol != nullptr)
        {
            return identifier->symbol->uniqueName;
        }

        return identifier->name;
    }

    return "_";
}