/*
MIT License

Copyright (c) 2020 Jesper

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

#include "compiler.h"
#include <util/util.h>

uint64 Compiler::SyntaxAnalazys(List<Token>& tokens, uint64 start, ASTNode* currentNode) {
	HC_ASSERT(currentNode != nullptr);

	static uint64 currentScope = 0;

	for (uint64 i = start; i < tokens.GetSize(); i++) {
		const Token& t = tokens[i];

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

			const Token& nameToken = tokens[index];

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

						index = SyntaxAnalazys(tokens, index + 1, func);

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

uint64 Compiler::ParseTypedef(List<Token>& tokens, uint64 start, ASTNode* currentNode) {
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

uint64 Compiler::ParseTypeDeclaration(List<Token>& tokens, uint64 start, TypeNode* typeNode) {
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
			Compiler::Log(token, HC_ERROR_SYNTAX_ERROR);
			return ~0;
		}

		start += 1;
	}

	return start;
}

uint64 Compiler::ParseStruct(List<Token>& tokens, uint64 start, ASTNode* currentNode) {
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

uint64 Compiler::ParseFunctionParameters(List<Token>& tokens, uint64 start, ASTNode* functionNode) {
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

uint64 Compiler::ParseExpression(List<Token>& tokens, uint64 start, ASTNode* currentNode) {
	List<ASTNode*> nodes;

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

			if (nodes.GetSize())
				currentNode->AddNode(nodes[0]);

			return i;
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
			OperatorTypeDef op = GetOperator(tokens, nodes, i);

			if (op.type == OperatorType::Unknown) {
				OperatorTypeDef op2 = GetOperator(token.operatorType, OperandType::Any, OperandType::Any, true);

				if (op2.rightOperand != op.rightOperand) {
					Token& err = tokens[i + 1];
					Compiler::Log(err, HC_ERROR_SYNTAX_ERROR);
					return ~0;
				} else if (op2.leftOperand != op.leftOperand) {
					Token& err = tokens[i - 1];
					Compiler::Log(err, HC_ERROR_SYNTAX_ERROR);
					return ~0;
				}
			}

			if (i + 2 == tokens.GetSize()) {
				Compiler::Log(token, HC_ERROR_SYNTAX_EOL);
				return ~0;
			}

			ASTNode* rightNode = nullptr;

			if (tokens[i + 1].type == TokenType::ParenthesisOpen) {
				parenthesesCount++;
				ASTNode tmp(ASTType::Root);

				i = ParseExpression(tokens, i + 2, &tmp) - 1;

				rightNode = tmp.branches[0];
			}

			Token& next = tokens[i + 2];

			if (next.type == TokenType::Operator) {
				OperatorTypeDef nextOp = GetOperator(next.operatorType, OperandType::Any, OperandType::Any, true);

				if ((nextOp.precedence < op.precedence) || (nextOp.precedence == op.precedence && op.associativty == OperatorAssociativity::RTL)) {
					// Next operator has priority

					nodes.PushBack(new OperatorNode(op.type, &token));

					if (rightNode) {
						nodes.PushBack(rightNode);
						i++;
					}

					continue;
				}
			} else {
				Token& next2 = tokens[i + 1];

				if (next2.type == TokenType::Operator) {
					OperatorTypeDef nextOp = GetOperator(next2.operatorType, OperandType::Any, OperandType::Any, true);

					if ((nextOp.precedence < op.precedence) || (nextOp.precedence == op.precedence && op.associativty == OperatorAssociativity::RTL)) {
						// Next operator has priority

						nodes.PushBack(new OperatorNode(op.type, &token));

						if (rightNode) {
							nodes.PushBack(rightNode);
							i++;
						}

						continue;
					}
				}
			}

			i++;

			ASTNode* opNode = new OperatorNode(token.operatorType, &token);

			if (op.leftOperand != OperandType::None) {
				opNode->AddNode(nodes.Back());
				nodes.PopBack();

				if (op.type == OperatorType::OpInc || op.type == OperatorType::OpDec) {
					//Post inc/dec

					if (i + 2 >= tokens.GetSize()) {
						Compiler::Log(token, HC_ERROR_SYNTAX_EOL);
						return ~0;
					}

					Token& nextOpToken = tokens[i];

					if (nextOpToken.type != TokenType::Operator) {
						if (nextOpToken.type == TokenType::ParenthesisClose || nextOpToken.type == TokenType::Semicolon) {
							nodes.PushBack(opNode);
							BacktrackNodes(nodes);
							currentNode->AddNode(nodes.Back());

							return i;
						}

						Compiler::Log(nextOpToken, HC_ERROR_SYNTAX_ERROR);
						return ~0;
					}

					OperatorTypeDef nextOp = GetOperator(tokens, nodes, i);
					OperatorTypeDef op2    = GetOperator(nextOpToken.operatorType, OperandType::Any, OperandType::Any, true);

					if (op2.rightOperand != nextOp.rightOperand) {
						Token& err = tokens[i + 1];
						Compiler::Log(err, HC_ERROR_SYNTAX_ERROR);
						return ~0;
					} else if (op2.leftOperand != nextOp.leftOperand) {
						Token& err = tokens[i - 1];
						Compiler::Log(err, HC_ERROR_SYNTAX_ERROR);
						return ~0;
					}

					ASTNode*        leftOpNode  = nodes.Back();
					ASTNode*        leftOpNode2 = nullptr;
					OperatorTypeDef opLeft      = GetOperator(((OperatorNode*)leftOpNode)->type, OperandType::Any, OperandType::Any, true);

					nodes.PopBack();

					if (opLeft.type == OperatorType::OpInc || opLeft.type == OperatorType::OpDec) {
						opLeft.leftOperand  = OperandType::None;
						opLeft.rightOperand = OperandType::Any;

						if (nodes.GetSize() > 1) {
							leftOpNode2 = nodes.Back();
						}
					}

					op.type = op.type == OperatorType::OpInc ? OperatorType::OpPostInc : OperatorType::OpPostDec;

					if (opLeft.precedence < nextOp.precedence || (opLeft.precedence == nextOp.precedence && nextOp.associativty == OperatorAssociativity::LTR)) {
						ASTNode* tmp = opNode;

						if (opLeft.leftOperand != OperandType::None) {
							leftOpNode->AddNode(nodes.Back());
							nodes.PopBack();
						}

						leftOpNode->AddNode(opNode);

						if (leftOpNode2) {
							opLeft = GetOperator(((OperatorNode*)leftOpNode2)->type, OperandType::Any, OperandType::Any, true);

							if (opLeft.precedence < nextOp.precedence || (opLeft.precedence == nextOp.precedence && nextOp.associativty == OperatorAssociativity::LTR)) {
								nodes.PopBack();
								leftOpNode2->AddNode(nodes.Back());
								leftOpNode2->AddNode(leftOpNode);
								nodes.PopBack();

								leftOpNode = leftOpNode2;
							}
						}

						opNode = leftOpNode;

						i--;
					} else {
						i--;
						nodes.PushBack(leftOpNode);
						nodes.PushBack(opNode);
						continue;
					}
				}
			}

			if (op.rightOperand != OperandType::None) {
				Token& right = tokens[i];

				if (rightNode == nullptr) {
					rightNode = CreateOperandNode(tokens, &i);
				}

				opNode->AddNode(rightNode);

				if (op.type == OperatorType::OpInc || op.type == OperatorType::OpDec) {
					//Pre inc/dec

					if (i + 2 >= tokens.GetSize()) {
						Compiler::Log(token, HC_ERROR_SYNTAX_EOL);
						return ~0;
					}

					Token& nextOpToken = tokens[i + 1];

					if (nextOpToken.type != TokenType::Operator) {
						if (nextOpToken.type == TokenType::ParenthesisClose || nextOpToken.type == TokenType::Semicolon) {
							nodes.PushBack(opNode);
							BacktrackNodes(nodes);
							currentNode->AddNode(nodes.Back());

							return i;
						}

						Compiler::Log(nextOpToken, HC_ERROR_SYNTAX_ERROR);
						return ~0;
					}

					OperatorTypeDef nextOp = GetOperator(tokens, nodes, i + 1);
					OperatorTypeDef op2    = GetOperator(nextOpToken.operatorType, OperandType::Any, OperandType::Any, true);

					if (op2.rightOperand != nextOp.rightOperand) {
						Token& err = tokens[i + 2];
						Compiler::Log(err, HC_ERROR_SYNTAX_ERROR);
						return ~0;
					} else if (op2.leftOperand != nextOp.leftOperand) {
						Token& err = tokens[i + 1];
						Compiler::Log(err, HC_ERROR_SYNTAX_ERROR);
						return ~0;
					}

					op.type = op.type == OperatorType::OpInc ? OperatorType::OpPreInc : OperatorType::OpPreDec;

					ASTNode*        leftOpNode = nodes.Back();
					OperatorTypeDef opLeft     = GetOperator(((OperatorNode*)leftOpNode)->type, OperandType::Any, OperandType::Any, true);

					if (opLeft.precedence < nextOp.precedence || (nextOp.precedence == op.precedence && op.associativty == OperatorAssociativity::LTR)) {
						ASTNode* tmp = opNode;

						nodes.PopBack();
						leftOpNode->AddNode(nodes.Back());
						leftOpNode->AddNode(opNode);
						nodes.PopBack();

						opNode = leftOpNode;
					} else {
						nodes.PushBack(opNode);
						continue;
					}
				}
			}

			nodes.PushBack(opNode);

			BacktrackNodes(nodes);

		} else if (token.type == TokenType::Semicolon || token.type == TokenType::Comma) {
			currentNode->AddNode(nodes.Back());

			if (token.type == TokenType::Semicolon)
				return i;

			nodes.Clear();
		}
	}

	return ~0;
}

uint64 Compiler::ParseLayout(List<Token>& tokens, uint64 start, ASTNode* currentNode) {
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

void Compiler::BacktrackNodes(List<ASTNode*>& nodes) {
	for (int64 i = nodes.GetSize() - 1; i >= 0; i--) {
		ASTNode* node = nodes[i];

		if (node->nodeType == ASTType::Operator && node->branches.GetSize() == 0) {
			OperatorNode*   op  = (OperatorNode*)node;
			OperatorTypeDef def = GetOperator(op->type, OperandType::Any, OperandType::Any, true);

			if (def.leftOperand != OperandType::None) {
				node->AddNode(nodes[i - 1]);
				nodes.Remove(i - 1);
			}

			if (def.rightOperand != OperandType::None) {
				node->AddNode(nodes.Back());
				nodes.PopBack();
			}
		}
	}
}

/*	Token signToken;
	Token constToken;
	uint8 sign      = 2;
	uint8 constness = 0;

	Type*         tmp  = nullptr;
	PrimitiveType type = PrimitiveType::Unknown;

	uint64 i = 0;

	for (i = start; i < tokens.GetSize(); i++) {
		const Token& token = tokens[i];

		if (token.type != TokenType::PrimitiveType) {
			if (token.type == TokenType::Identifier) {
				Type* tmptmp = GetType(token.string);

				if (tmptmp == nullptr) {
					break;
				} else if (type != PrimitiveType::Unknown) {
					Compiler::Log(token, HC_ERROR_SYNTAX_TYPE_FOLLOWED_BY_TYPE, GetPrimitiveTypeString(type).str, tmp->name.str);
					return ~0;
				}

				if (tmp != nullptr) {
					Compiler::Log(token, HC_ERROR_SYNTAX_TYPE_FOLLOWED_BY_TYPE, tmp->name, tmptmp->name);
					return ~0;
				}

				tmp = tmptmp;
			} else {
				break;
			}
		} else {
			switch (token.primitiveType) {
				case PrimitiveType::Const:
					if (constness) {
						Compiler::Log(token, HC_WARN_SYNTAX_SAME_TYPE_QUALIFIER);
					}

					constness  = 1;
					constToken = token;
					break;
				case PrimitiveType::Unsigned:
					if (sign == 0) {
						Compiler::Log(token, HC_WARN_SYNTAX_SAME_TYPE_QUALIFIER);
						return ~0;
					} else if (sign == 1) {
						Compiler::Log(token, HC_ERROR_SYNTAX_SIGNED_UNSIGNED_EXCLUSIVE);
						return ~0;
					}

					sign      = 0;
					signToken = token;
					break;
				case PrimitiveType::Signed:
					if (sign == 1) {
						Compiler::Log(token, HC_WARN_SYNTAX_SAME_TYPE_QUALIFIER);
						return ~0;
					} else if (sign == 0) {
						Compiler::Log(token, HC_ERROR_SYNTAX_SIGNED_UNSIGNED_EXCLUSIVE);
						return ~0;
					}

					sign      = 1;
					signToken = token;
					break;
				case PrimitiveType::Byte:
				case PrimitiveType::Short:
				case PrimitiveType::Int:
				case PrimitiveType::Float:
				case PrimitiveType::Vec2:
				case PrimitiveType::Vec3:
				case PrimitiveType::Vec4:
				case PrimitiveType::Mat4:
					if (type != PrimitiveType::Unknown && tmp != nullptr) {
						Compiler::Log(token, HC_ERROR_SYNTAX_TYPE_FOLLOWED_BY_TYPE, GetPrimitiveTypeString(type).str, GetPrimitiveTypeString(token.primitiveType).str);
						return ~0;
					}

					type = token.primitiveType;
			}
		}
	}

	if (type != PrimitiveType::Unknown) {
		if (type != PrimitiveType::Byte && type != PrimitiveType::Short && type != PrimitiveType::Int) {
			if (sign != 2) {
				Compiler::Log(signToken, HC_ERROR_SYNTAX_SIGNED_UNSIGNED_NOT_ALLOWED_ON_TYPE, sign == 0 ? "unsigned" : "signed", GetPrimitiveTypeString(type).str);
				return ~0;
			}
		}

		switch (type) {
			case PrimitiveType::Byte:
			case PrimitiveType::Short:
			case PrimitiveType::Int:
			case PrimitiveType::Float:
				*retType = MakeTypeScalar(type, sign, constness);
				break;
			case PrimitiveType::Vec2:
			case PrimitiveType::Vec4:
			case PrimitiveType::Vec3:
				*retType = MakeTypeVec(type, constness);
				break;
			case PrimitiveType::Mat4:
				*retType = MakeTypeMat(type, constness);
		}
	} else if (tmp) {
		TypeTypeDef* def = (TypeTypeDef*)tmp;

		if (constness == 1) {
			if (def->actualType->constness == 1) {
				Compiler::Log(constToken, HC_WARN_SYNTAX_SAME_TYPE_QUALIFIER);
			}
		}

		if (sign != 2) {
			Compiler::Log(signToken, HC_ERROR_SYNTAX_SIGNED_UNSIGNED_NOT_ALLOWED_ON_TYPE, def->name.str, sign == 0 ? "unsigned" : "signed");
			return ~0;
		}

		*retType = def;

		return i;
	} else {
		Compiler::Log(tokens[i], HC_ERROR_SYNTAX_ERROR);
		return ~0;
	}

	return i;*/