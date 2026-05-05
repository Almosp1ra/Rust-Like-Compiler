#include "CompilerTools.h"

/*
* CompilerTools 的成员函数
*/

/************************************************
* PrintUsage
************************************************/
void CompilerTools::PrintUsage(const char* programName)
{
	cout << "=================================================" << endl;
	cout << "Rust-like-Compiler - 类Rust语言语法分析器测试工具" << endl;
	cout << "=================================================" << endl;
	cout << "使用方法 (Usage):" << endl;
	cout << "  " << programName << " <source_file_path>" << endl << endl;
	cout << "参数说明 (Arguments):" << endl;
	cout << "  source_file_path : 要进行分析的目标源代码文件路径。" << endl << endl;
	cout << "示例 (Example):" << endl;
	cout << "  " << programName << " test_code.txt" << endl;
	cout << "=================================================" << endl;
}

/************************************************
* PrintLexerError：打印词法分析产生的错误
************************************************/
void CompilerTools::PrintLexerError(const vector<LexerError>& errorList)
{
	if (!errorList.size())
	{
		cout << "No Lexing Errors." << endl;
		cout << endl;
		return;
	}

	cout << "--- Lexing Errors: ---" << endl;

	for (size_t i = 0; i < errorList.size(); i++)
	{
		const auto& err = errorList[i];
		string msg =
			(err.type == LexerErrorType::UndefinedToken) ? "Undefined Token" :
			(err.type == LexerErrorType::UnterminatedComment) ? "Unterminated Comment" : "Unknown Error";
		cout << "[Error] " << msg << " (Line: " << err.location.line << ", Column" << err.location.column << ")" << endl;
	}

	cout << endl;
}

/************************************************
* PrintTokenList：打印词法分析结果（Token序列）
************************************************/
void CompilerTools::PrintTokenList(const vector<Token>& tokenList)
{
	// 表头
	cout << "--- Token List: ---" << endl;
	cout << left
		 << setw(20) << "Type"
		 << setw(20) << "(Line, Column)"
		 << "Content" << endl;

	// Token 序列
	for (size_t i = 0; i < tokenList.size(); i++)
	{
		Token t = tokenList[i];

		string typeStr;

		switch (t.type)
		{
		case TokenType::Keyword:
			typeStr = "Keyword";
			break;
		case TokenType::Identifier:
			typeStr = "Identifier";
			break;
		case TokenType::Value:
			typeStr = "Number";
			break;
		case TokenType::AssignmentOperator:
			typeStr = "Assignment Operator";
			break;
		case TokenType::Operator:
			typeStr = "Operator";
			break;
		case TokenType::Delimiter:
			typeStr = "Delimiter";
			break;
		case TokenType::Separator:
			typeStr = "Separator";
			break;
		case TokenType::Special:
			typeStr = "Special";
			break;
		case TokenType::CommentMarker:
			typeStr = "Comment Marker";
			break;
		case TokenType::Terminator:
			typeStr = "Terminator";
			break;
		default:
			typeStr = "Unknown Type";
			break;
		}

		cout << left
			<< setw(20) << typeStr
			<< setw(20) << "(" + to_string(t.location.line) + ", " + to_string(t.location.column) + ")"
			<< t.content << endl;
	}

	cout << endl;
}

/************************************************
* PrintParserError：打印语法分析产生的错误
************************************************/
void CompilerTools::PrintParserError(const vector<ParserError>& errorList)
{
	if (!errorList.size())
	{
		cout << "No Parsing Errors." << endl;
		cout << endl;
		return;
	}

	cout << "--- Parsing Errors: ---" << endl;
	for (size_t i = 0; i < errorList.size(); i++)
	{
		const auto& err = errorList[i];
		string msg =
			(err.type == ParserErrorType::UnexpectedToken) ? "Unexpected Token" :
			(err.type == ParserErrorType::UnterminatedProgram) ? "Unterminated Program" : "Unknown Error";
		cout << "[Error] " << msg << " at '" << err.token.content << "' (Line: " << err.token.location.line << ")" << endl;
	}

	cout << endl;

	return;
}

/************************************************
* PrintAST：打印语法分析结果（抽象语法树）
************************************************/
void CompilerTools::PrintAST(const ASTNode* root)
{
	if (root)
	{
		cout << "--- AST Output: ---" << endl;
		PrintNode(root, 0);
	}
	else
	{
		cout << "AST is empty." << endl;
	}
}

/************************************************
* PrintNode：打印AST节点
* 递归实现，通过 depth 决定打印的缩进、体现树结构
************************************************/
// 打印缩进
void CompilerTools::PrintIndent(int depth) {
	for (int i = 0; i < depth; ++i)
	{
			std::cout << "  ";
	}
}
// 打印节点
void CompilerTools::PrintNode(const ASTNode* node, int depth)
{
	if (!node)
	{
		return;
	}

	// 使用 dynamic_cast 识别节点类型并打印
	if (auto n = dynamic_cast<const Node_PrimitiveType*>(node))
	{
		PrintIndent(depth);
		cout << "Type: " << PrimitiveTypeToString(n->type) << endl;
	}
	else if (auto n = dynamic_cast<const Node_VariableProperty*>(node))
	{
		PrintIndent(depth);
		cout << "VarProperty: " << VariablePropertyToString(n->property) << endl;
	}
	else if (auto n = dynamic_cast<const Node_LeftValue_Identifier*>(node))
	{
		PrintIndent(depth);
		cout << "LeftValue : " << n->name << endl;
	}
	else if (auto n = dynamic_cast<const Node_Program*>(node))
	{
		PrintIndent(depth);
		cout << "Program" << endl;

		for (size_t i = 0; i < n->declarations.size(); i++)
		{
			PrintNode(n->declarations[i].get(), depth + 1);
		}
	}
	else if (auto n = dynamic_cast<const Node_FunctionDeclaration*>(node))
	{
		PrintIndent(depth);
		cout << "FunctionDeclaration" << endl;

		PrintNode(n->header.get(), depth + 1);
		PrintNode(n->body.get(), depth + 1);
	}
	else if (auto n = dynamic_cast<const Node_FunctionHeader*>(node))
	{
		PrintIndent(depth);
		cout << "FunctionHeader: " << n->name << endl;

		PrintIndent(depth + 1);
		cout << "[Parameters]:" << endl;
		for (size_t i = 0; i < n->parameterList.size(); i++)
		{
			PrintNode(n->parameterList[i].get(), depth + 2);
		}

		PrintNode(n->type.get(), depth + 1);
	}
	else if (auto n = dynamic_cast<const Node_Parameter*>(node))
	{
		PrintIndent(depth);
		cout << "Parameter: " << n->name << endl;

		PrintNode(n->varProperty.get(), depth + 1);
		PrintNode(n->type.get(), depth + 1);
	}
	else if (auto n = dynamic_cast<const Node_StatementBlock*>(node))
	{
		PrintIndent(depth);
		cout << "StatementBlock" << endl;

		for (size_t i = 0; i < n->statements.size(); i++)
		{
			PrintNode(n->statements[i].get(), depth + 1);
		}
	}
	else if (auto n = dynamic_cast<const Node_EmptyStatement*>(node))
	{
		PrintIndent(depth);
		cout << "EmptyStatement" << endl;
	}
	else if (auto n = dynamic_cast<const Node_ReturnStatement*>(node))
	{
		PrintIndent(depth);
		cout << "Return" << endl;

		PrintNode(n->expr.get(), depth + 1);
	}
	else if (auto n = dynamic_cast<const Node_VariableDeclarationStatement*>(node))
	{
		PrintIndent(depth);
		cout << "VarDeclaration: " << n->name << endl;

		PrintNode(n->varProperty.get(), depth + 1);
		PrintNode(n->type.get(), depth + 1);
	}
	else if (auto n = dynamic_cast<const Node_AssignmentStatement*>(node))
	{
		PrintIndent(depth);
		cout << "Assignment" << endl;

		PrintNode(n->leftValue.get(), depth + 1);
		PrintNode(n->expr.get(), depth + 1);
	}
	else if (auto n = dynamic_cast<const Node_ExpressionStatement*>(node))
	{
		PrintNode(n->expr.get(), depth);	// 直接输出表达式
	}
	else if (auto n = dynamic_cast<const Node_IfStatement*>(node))
	{
		PrintIndent(depth);
		cout << "IfStatement" << endl;

		PrintIndent(depth + 1);
		cout << "[Condition]:" << endl;
		PrintNode(n->condition.get(), depth + 2);

		PrintIndent(depth + 1);
		cout << "[Then]:" << endl;
		PrintNode(n->thenBlock.get(), depth + 2);

		if (n->elseBlock) {
			PrintIndent(depth + 1);
			cout << "[Else]:" << endl;
			PrintNode(n->elseBlock.get(), depth + 2);
		}
	}
	else if (auto n = dynamic_cast<const Node_WhileStatement*>(node))
	{
		PrintIndent(depth);
		cout << "WhileStatement" << endl;

		PrintIndent(depth + 1);
		cout << "[Condition]:" << endl;
		PrintNode(n->condition.get(), depth + 2);

		PrintIndent(depth + 1);
		cout << "[Body]:" << endl;
		PrintNode(n->body.get(), depth + 2);
	}
	else if (auto n = dynamic_cast<const Node_NumberExpression*>(node))
	{
		PrintIndent(depth);
		cout << "Number: " << n->value << endl;
	}
	else if (auto n = dynamic_cast<const Node_VariableExpression*>(node))
	{
		PrintNode(n->leftValue.get(), depth);
	}
	else if (auto n = dynamic_cast<const Node_CallExpression*>(node))
	{
		PrintIndent(depth);
		cout << "Call: " << n->name << endl;

		PrintIndent(depth + 1);
		cout << "[Arguments]:" << endl;
		for (size_t i = 0; i < n->argumentList.size(); i++)
		{
			PrintNode(n->argumentList[i].get(), depth + 2);
		}
	}
	else if (auto n = dynamic_cast<const Node_BinaryExpression*>(node))
	{
		PrintIndent(depth);
		cout << "BinaryExpr (" << BinaryOpToString(n->op) << ")" << endl;

		PrintIndent(depth + 1);
		cout << "[Left]:" << endl;
		PrintNode(n->left.get(), depth + 2);

		PrintIndent(depth + 1);
		cout << "[Right]:" << endl;
		PrintNode(n->right.get(), depth + 2);
		}
	else
	{
		PrintIndent(depth);
		cout << "[Unknown Node Type]" << endl;
	}
}