/*
MIT License

Copyright (c) 2021 Jesper

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#include "syntax.h"
#include "compiler.h"

uint64 Syntax::Analyze(Tokens& tokens, uint64 start, ASTNode* currentNode, Language* lang) {
	Syntax syn(lang);

	return syn.Analyze(tokens, start, currentNode);
}

uint64 Syntax::Analyze(Tokens& tokens, uint64 start, ASTNode* currentNode) {
	HC_ASSERT(currentNode != nullptr);

	static uint64 currentScope = 0;

	for (uint64 i = start; i < tokens.GetSize(); i++) {
		Token& t = tokens[i];

		if (t.type == TokenType::Keyword) {
			if (t.keyword == KeywordType::Typedef) {
				i = ParseTypedef(tokens, i + 1, currentNode);

				if (i == ~0)
					return ~0;

			} else if (t.keyword == KeywordType::Layout) {
				i = ParseLayout(tokens, i + 1, currentNode);

				if (i == ~0)
					return ~0;

			} else if (t.keyword == KeywordType::Struct) {
				i = ParseStruct(tokens, i + 1, currentNode);

				if (i == ~0)
					return ~0;

			} else if (t.keyword == KeywordType::If) {
			} else if (t.keyword == KeywordType::For) {
			} else if (t.keyword == KeywordType::While) {
			} else if (t.keyword == KeywordType::Switch) {
			} else if (t.keyword == KeywordType::Return) {
				Token&   next = tokens[i + 1];
				ASTNode* ret  = new ASTNode(ASTType::Return, &t);

				currentNode->AddNode(ret);

				if (next.type == TokenType::Semicolon) {
					i++;
				} else {
					i = ParseExpression(tokens, i + 1, ret);

					if (i == ~0)
						return i;
				}

			} else {
				Compiler::Log(t, HC_ERROR_SYNTAX_ERROR);
				return ~0;
			}

		} else if (t.type == TokenType::BracketClose) {
			return i;
		} else {
			TypeNode* typeNode = new TypeNode(&t);
			uint64    index    = ParseTypeDeclaration(tokens, i, typeNode);

			if (index == ~0)
				return ~0;

			Token& nameToken = tokens[index];

			if (nameToken.type == TokenType::Operator) {
				index = ParseExpression(tokens, i, currentNode);

				if (index == ~0)
					return ~0;

				i = index;
			} else {
				if (!CheckName(nameToken)) {
					Compiler::Log(nameToken, HC_ERROR_SYNTAX_ILLEGAL_VARIABLE_NAME);
					return ~0;
				}

				StringNode* stringNode = new StringNode(nameToken.string, &nameToken);

				const Token& next = tokens[++index];

				if (next.type == TokenType::ParenthesisOpen) {
					ASTNode* func = new ASTNode(ASTType::FunctionDeclaration, &nameToken);

					func->AddNode(typeNode);
					func->AddNode(stringNode);

					index = ParseFunctionParameters(tokens, index, func);

					if (index == ~0)
						return index;

					Token& semicolonOrBracket = tokens[index];

					if (semicolonOrBracket.type == TokenType::Semicolon) {
						currentNode->AddNode(func);
					} else if (semicolonOrBracket.type == TokenType::BracketOpen) {
						currentNode->AddNode(func);

						func->nodeType = ASTType::FunctionDefinition;

						currentScope++;

						index = Analyze(tokens, index + 1, func);

						if (index == ~0)
							return ~0;
					}

					i = index;

				} else {
					ASTNode* var = new ASTNode(ASTType::VariableDefinition, &nameToken);

					currentNode->AddNode(var);

					var->AddNode(typeNode);
					var->AddNode(stringNode);

					if (next.type == TokenType::Semicolon) {
						i = index;
						continue;
					} else if (next.operatorType == OperatorType::OpAssign) {
						index = ParseExpression(tokens, index + 1, var);

						if (index == ~0)
							return ~0;

						i = index;
					} else {
						Compiler::Log(next, HC_ERROR_SYNTAX_EXPECTED, ";");
					}
				}
			}
		}
	}

	return 0;
}

bool Syntax::CheckName(const Token& token) {
	String name = token.string;

	if (token.type != TokenType::Identifier) {
		return false;
	}

	if (!(name[0] == '_' || (name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z'))) {
		return false;
	}

	for (uint64 i = 0; i < lang->keywords.GetSize(); i++) {
		auto& t = lang->keywords[i];

		if (t.def == name) {
			return false;
		}
	}

	for (uint64 i = 0; i < lang->primitiveTypes.GetSize(); i++) {
		auto& t = lang->primitiveTypes[i];

		if (t.def == name) {
			return false;
		}
	}

	return true;
}

OperatorTypeDef Syntax::GetOperator(OperatorType type, OperandType left, OperandType right, bool ignoreOperands) {
	for (OperatorTypeDef& def : lang->operators) {
		if (def.type == type) {
			if (left == OperandType::None && !ignoreOperands) {
				if (def.leftOperand != OperandType::None)
					continue;
			} /*else if (left == OperandType::Variable) {
				if (def.leftOperand != OperandType::Variable && def.rightOperand != OperandType::Any) continue;
			} else if (left == OperandType::Value) {
				if (def.leftOperand != OperandType::Value && def.leftOperand != OperandType::Any) continue;
			}*/

			if (right == OperandType::None && !ignoreOperands) {
				if (def.rightOperand != OperandType::None)
					continue;
			} /*else if (right == OperandType::Variable) {
				if (def.rightOperand != OperandType::Variable && def.rightOperand != OperandType::Any) continue;
			} else if (right == OperandType::Value) {
				if (def.rightOperand != OperandType::Value && def.rightOperand != OperandType::Any) continue;
			}*/

			return def;
		}
	}

	OperatorTypeDef tmp;

	tmp.leftOperand = left;
	tmp.rightOperand = right;

	return tmp;
}

OperatorTypeDef Syntax::GetOperator(List<ASTNode*>& nodes, uint64 index) {
	HC_ASSERT(nodes[index]->nodeType == ASTType::Operator);

	OperatorNode* node = (OperatorNode*)nodes[index];
	OperandType left = OperandType::None;
	OperandType right = OperandType::None;

	ASTNode* operand = nullptr;

	if (index > 0) {
		operand = nodes[index - 1];

		if (operand->nodeType == ASTType::Variable || operand->nodeType == ASTType::Constant || operand->nodeType == ASTType::Function) {
			left = OperandType::Any;
		} else if (operand->nodeType == ASTType::Operator) {
			if (operand->branches.GetSize() > 0) {
				left = OperandType::Any;
			}
		}
	}

	if (index < nodes.GetSize() - 1) {
		operand = nodes[index + 1];

		if (operand->nodeType == ASTType::Variable || operand->nodeType == ASTType::Constant || operand->nodeType == ASTType::Function) {
			right = OperandType::Any;
		} else if (operand->nodeType == ASTType::Operator) {
			right = OperandType::Any;
		}
	}

	return GetOperator(node->type, left, right, false);
}

ASTNode* Syntax::CreateOperandNode(Tokens& tokens, uint64* index) {
	Token& token = tokens[*index];
	ASTNode* node = nullptr;

	if (token.type == TokenType::Literal) {

		switch (token.primitiveType) {
			case PrimitiveType::Int:
				node = new ConstantNode(token.primitiveType, (uint32)atoi(token.string.str), &token);
				break;
			case PrimitiveType::Float:
				node = new ConstantNode(token.primitiveType, (float)atof(token.string.str), &token);
				break;
		}
	} else if (token.type == TokenType::Identifier || token.type == TokenType::PrimitiveType) {
		Token& next = tokens[*index + 1];

		if (next.type == TokenType::ParenthesisOpen) {
			(*index)++;

			node = new ASTNode(ASTType::Function, &token);

			node->AddNode(new StringNode(token.string, &token));

			*index = ParseExpression(tokens, *index + 1, node);

		} else {
			node = new ASTNode(ASTType::Variable, &token);
			node->AddNode(new StringNode(token.string, &token));
		}
	}

	return node;
}

uint64 Syntax::ParseTypedef(Tokens& tokens, uint64 start, ASTNode* currentNode) {
	TypeNode* type  = new TypeNode(&tokens[start]);
	uint64    index = ParseTypeDeclaration(tokens, start, type);

	if (index == ~0)
		return ~0;

	Token&      name       = tokens[index++];
	Token&      semiColon  = tokens[index];
	ASTNode*    node       = new ASTNode(ASTType::Typedef, &tokens[start - 1]);
	StringNode* stringNode = new StringNode(name.string, &name);

	node->AddNode(type);
	node->AddNode(stringNode);

	if (semiColon.type != TokenType::Semicolon) {
		Compiler::Log(semiColon, HC_ERROR_SYNTAX_EXPECTED, ";");
		return ~0;
	}

	currentNode->AddNode(node);

	return index;
}

uint64 Syntax::ParseTypeDeclaration(Tokens& tokens, uint64 start, TypeNode* typeNode) {
	bool identifierAdded = false;

	while (true) {
		Token& token = tokens[start];

		if ((token.type == TokenType::Identifier || token.type == TokenType::PrimitiveType) && !identifierAdded) {
			typeNode->AddToken(&token);

			if (token.type == TokenType::Identifier) {
				identifierAdded = true;
			}
		} else if (token.type == TokenType::Semicolon || (token.type == TokenType::Operator && token.operatorType == OperatorType::OpAssign) || token.type == TokenType::ParenthesisOpen || token.type == TokenType::ParenthesisClose || token.type == TokenType::Comma) {
			if (typeNode->tokens.GetSize() > 1 && identifierAdded) {
				typeNode->tokens.PopBack();
				start -= 1;
			}

			break;
		} else {
			break;
		}

		start += 1;
	}

	return start;
}

uint64 Syntax::ParseStruct(Tokens& tokens, uint64 start, ASTNode* currentNode) {
	ASTNode* strct  = new ASTNode(ASTType::Struct, &tokens[start - 1]);
	Token&   stName = tokens[start++];

	if (!CheckName(stName)) {
		Compiler::Log(stName, HC_ERROR_SYNTAX_ILLEGAL_TYPENAME);
		return ~0;
	}

	strct->AddNode(new StringNode(stName.string, &stName));

	Token& bracket = tokens[start++];

	if (bracket.type != TokenType::BracketOpen) {
		Compiler::Log(bracket, HC_ERROR_SYNTAX_EXPECTED, "{");
		return ~0;
	}

	while (true) {
		TypeNode* type = new TypeNode(&tokens[start]);

		start = ParseTypeDeclaration(tokens, start, type);

		if (start == ~0)
			return ~0;

		Token&      name       = tokens[start++];
		StringNode* stringNode = new StringNode(name.string, &name);

		if (!CheckName(name)) {
			Compiler::Log(stName, HC_ERROR_SYNTAX_ILLEGAL_VARIABLE_NAME);
			return ~0;
		}

		Token& semiColon = tokens[start++];

		if (semiColon.type != TokenType::Semicolon) {
			Compiler::Log(semiColon, HC_ERROR_SYNTAX_EXPECTED, ";");
			return ~0;
		}

		strct->AddNode(type);
		strct->AddNode(stringNode);

		Token& closeBracket = tokens[start];

		if (closeBracket.type == TokenType::BracketClose) {
			Token& semi = tokens[++start];

			if (semi.type != TokenType::Semicolon) {
				Compiler::Log(semi, HC_ERROR_SYNTAX_EXPECTED, ";");
				return ~0;
			}

			break;
		}
	}

	currentNode->AddNode(strct);

	return start;
}

uint64 Syntax::ParseFunctionParameters(Tokens& tokens, uint64 start, ASTNode* functionNode) {
	for (uint64 i = start + 1; i < tokens.GetSize(); i++) {
		Token& token = tokens[i];

		if (token.type == TokenType::ParenthesisClose)
			return i + 1;

		TypeNode* paramType = new TypeNode(&token);

		i = ParseTypeDeclaration(tokens, i, paramType);

		if (i == ~0)
			return ~0;

		ASTNode* param = new ASTNode(ASTType::Parameter, &token);

		param->AddNode(paramType);

		Token& nameToken = tokens[i];

		if (nameToken.type == TokenType::Comma || nameToken.type == TokenType::ParenthesisClose) {
			functionNode->AddNode(param);

			if (nameToken.type == TokenType::ParenthesisClose) {
				return i + 1;
			}

			continue;
		} else {
			if (!CheckName(nameToken)) {
				Compiler::Log(nameToken, HC_ERROR_SYNTAX_ILLEGAL_VARIABLE_NAME);
				return ~0;
			}

			param->AddNode(new StringNode(nameToken.string, &nameToken));
			param->token = &nameToken;
		}

		Token& commaToken = tokens[++i];

		if (commaToken.type == TokenType::Comma || commaToken.type == TokenType::ParenthesisClose) {
			functionNode->AddNode(param);

			if (commaToken.type == TokenType::ParenthesisClose) {
				return i + 1;
			}
		} else {
			Compiler::Log(commaToken, HC_ERROR_SYNTAX_ERROR);
			return ~0;
		}
	}

	return ~0;
}

uint64 Syntax::ParseExpression(Tokens& tokens, uint64 start, ASTNode* currentNode) {
	List<ASTNode*> nodes;
	List<ASTNode*> addAfter;

	static uint32 parenthesesCount = 0;

	for (uint64 i = start; i < tokens.GetSize(); i++) {
		Token& token = tokens[i];

		if (token.type == TokenType::ParenthesisOpen) {
			parenthesesCount++;
			ASTNode tmp(ASTType::Root);

			i = ParseExpression(tokens, i + 1, &tmp);

			nodes.PushBack(tmp.branches[0]);
		} else if (token.type == TokenType::ParenthesisClose) {
			if (parenthesesCount == 0 && !(currentNode->nodeType == ASTType::Function || currentNode->nodeType == ASTType::Layout)) {
				Compiler::Log(token, HC_ERROR_SYNTAX_ERROR);
				return ~0;
			}

			parenthesesCount--;

			start = i;

			break;
		} else if (token.type == TokenType::Literal || token.type == TokenType::Identifier || token.type == TokenType::PrimitiveType) {
			if (nodes.GetSize() > 0) {
				ASTNode* node = nodes.Back();

				if (!(node->nodeType == ASTType::Operator && node->branches.GetSize() == 0)) {
					Compiler::Log(token, HC_ERROR_SYNTAX_EXPECTED, ";");
					return ~0;
				}
			}

			nodes.PushBack(CreateOperandNode(tokens, &i));
		} else if (token.type == TokenType::Operator) {
			nodes.PushBack(new OperatorNode(token.operatorType, &token));
		} else if (token.type == TokenType::Semicolon) {
			start = i;
			break;
		} else if (token.type == TokenType::Comma) {
			ASTNode tmp(currentNode->nodeType);

			i = ParseExpression(tokens, i + 1, &tmp);

			if (i == ~0) {
				//TODO: error
				HC_ASSERT(false);
			}

			for (ASTNode* node : tmp.branches) {
				addAfter.PushBack(node);
			}

			start = i;
			break;
		}
	}

	for (uint64 i = 0; i < nodes.GetSize(); i++) {
		ASTNode* node = nodes[i];

		if (node->nodeType == ASTType::Operator && node->branches.GetSize() == 0) {
			OperatorTypeDef op = GetOperator(nodes, i);

			bool cont = false;
			uint64 nextOpEnd = 2;

			for (uint64 j = 1; j <= nextOpEnd; j++) {
				uint64 index = i + j;

				if (index >= nodes.GetSize()) break;

				ASTNode* next = nodes[index];

				if (next->nodeType == ASTType::Operator) {
					if (next->branches.GetSize() > 0) {
						nextOpEnd += 2;
						continue;
					}

					OperatorTypeDef nextOp = GetOperator(((OperatorNode*)next)->type, OperandType::Any, OperandType::Any, true);

					if ((nextOp.precedence < op.precedence) || (nextOp.precedence == op.precedence && op.associativty == OperatorAssociativity::RTL)) {
						// Next operator has priority

						i += j - 1;

						cont = true;

						break;
					}
				}
			}

			if (cont) continue;

			if (op.leftOperand != OperandType::None) {
				ASTNode* operand = nodes[--i];

				if (operand->nodeType == ASTType::Operator && operand->branches.GetSize() == 0) {
					//TODO: error
					HC_ASSERT(false);
				}

				node->AddNode(operand);
				nodes.Remove(i);
			}

			if (op.rightOperand != OperandType::None) {
				ASTNode* operand = nodes[i + 1];

				if (operand->nodeType == ASTType::Operator && operand->branches.GetSize() == 0) {
					//TODO: error
					HC_ASSERT(false);
				}

				node->AddNode(operand);
				nodes.Remove(i + 1);
			}

			i = -1;
		}
	}

	currentNode->AddNode(nodes[0]);

	for (ASTNode* node : addAfter) {
		currentNode->AddNode(node);
	}

	return start;
}

uint64 Syntax::ParseLayout(Tokens& tokens, uint64 start, ASTNode* currentNode) {
	Token& parenthesisOpen = tokens[start];

	if (parenthesisOpen.type != TokenType::ParenthesisOpen) {
		Compiler::Log(parenthesisOpen, HC_ERROR_SYNTAX_EXPECTED, "(");
		return ~0;
	}

	LayoutNode* layout = new LayoutNode(&tokens[start - 1]);

	start = ParseExpression(tokens, start + 1, layout);

	if (start == ~0)
		return ~0;

	Token& layoutType = tokens[++start];

	switch (layoutType.keyword) {
		case KeywordType::In:
			layout->type = LayoutType::In;
			break;
		case KeywordType::Out:
			layout->type = LayoutType::Out;
			break;
		case KeywordType::UniformBuffer:
			layout->type = LayoutType::UniformBuffer;
			break;
		case KeywordType::Sampler1D:
			layout->type = LayoutType::Sampler1D;
			break;
		case KeywordType::Sampler2D:
			layout->type = LayoutType::Sampler2D;
			break;
		case KeywordType::Sampler3D:
			layout->type = LayoutType::Sampler3D;
			break;
		default:
			Compiler::Log(layoutType, HC_ERROR_SYNTAX_EXPECTED, "<layout_type>");
			return ~0;
	}

	start += 1;

	if (layout->type == LayoutType::In || layout->type == LayoutType::Out) {
		TypeNode* typeNode = new TypeNode(&tokens[start]);

		start = ParseTypeDeclaration(tokens, start, typeNode);

		if (start == ~0)
			return ~0;

		Token&      name       = tokens[start++];
		Token&      semiColon  = tokens[start];
		StringNode* stringNode = new StringNode(name.string, &name);

		layout->AddNode(typeNode);
		layout->AddNode(stringNode);

		if (semiColon.type != TokenType::Semicolon) {
			Compiler::Log(semiColon, HC_ERROR_SYNTAX_EXPECTED, ";");
			return ~0;
		}

	} else {
		Token& bracketOpen = tokens[start + 1];

		if (bracketOpen.type == TokenType::BracketOpen) { // Explicit type
			Token&  name = tokens[start];
			ASTNode tmp(ASTType::Root);

			start = ParseStruct(tokens, start, &tmp);

			if (start == ~0)
				return ~0;

			ASTNode*    strct     = tmp.branches.Back();
			StringNode* strctName = (StringNode*)strct->branches[0];

			strctName->string += "_uniform_qwerty";

			layout->AddNode(new StringNode(name.string, &name));
			layout->AddNode(strct);

		} else {
			TypeNode*   type       = new TypeNode(&tokens[start++]);
			Token&      name       = tokens[start++];
			Token&      semiColon  = tokens[start];
			StringNode* stringNode = new StringNode(name.string, &name);

			if (!CheckName(name)) {
				Compiler::Log(name, HC_ERROR_SYNTAX_ILLEGAL_VARIABLE_NAME);
				return ~0;
			}

			if (semiColon.type != TokenType::Semicolon) {
				Compiler::Log(semiColon, HC_ERROR_SYNTAX_EXPECTED, ";");
				return ~0;
			}

			type->AddToken((Token*)type->token);

			layout->AddNode(type);
			layout->AddNode(stringNode);
		}
	}

	currentNode->AddNode(layout);

	return start;
}

