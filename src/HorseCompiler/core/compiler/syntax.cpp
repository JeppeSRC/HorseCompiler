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

OperatorTypeDef Syntax::GetOperator(Tokens& tokens, List<ASTNode*>& nodes, uint64 index) {
	Token& token = tokens[index];
	OperandType left = OperandType::None;
	OperandType right = OperandType::None;

	if (nodes.GetSize() > 0) {
		ASTNode* n = nodes.Back();

		if (n->nodeType == ASTType::Operator && (token.operatorType == OperatorType::OpInc || token.operatorType == OperatorType::OpDec || token.operatorType == OperatorType::OpNegate || token.operatorType == OperatorType::OpBitNot || token.operatorType == OperatorType::OpNot)) {
			if (n->branches.GetSize() > 0) {
				left = OperandType::Any;
			} else {
				left = OperandType::None;
			}
		} else {
			left = OperandType::Any; //n->nodeType == ASTType::Variable ? OperandType::Variable : OperandType::Value;
		}
	}

	Token& r = tokens[index + 1];

	if (r.type == TokenType::Operator) {
		if (r.operatorType == OperatorType::OpInc || r.operatorType == OperatorType::OpDec || r.operatorType == OperatorType::OpNegate || r.operatorType == OperatorType::OpBitNot || r.operatorType == OperatorType::OpNot) {
			right = OperandType::Any;
		} else {
			right = OperandType::None;
		}
	} else if (r.type == TokenType::Identifier || r.type == TokenType::Literal || r.type == TokenType::ParenthesisOpen || r.type == TokenType::PrimitiveType) {
		right = OperandType::Any;
	}

	return GetOperator(token.operatorType, left, right, false);
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

			OperatorNode* opNode = new OperatorNode(token.operatorType, &token);

			if (op.leftOperand != OperandType::None) {
				opNode->AddNode(nodes.Back());
				nodes.PopBack();

				if (op.type == OperatorType::OpInc || op.type == OperatorType::OpDec) {
					//Post inc/dec

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

					opNode->type = op.type == OperatorType::OpInc ? OperatorType::OpPostInc : OperatorType::OpPostDec;

					if (nodes.GetSize() > 0) {
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

							opNode = (OperatorNode*)leftOpNode;

							i--;
						} else {
							i--;
							nodes.PushBack(leftOpNode);
							nodes.PushBack(opNode);
							continue;
						}
					} else {
						i--;
					}
				}
			}

			if (op.rightOperand != OperandType::None) {
				Token& right = tokens[i];

				if (rightNode == nullptr) {
					rightNode = CreateOperandNode(tokens, &i);
				}

				opNode->AddNode(rightNode);

				if (op.type == OperatorType::OpInc || op.type == OperatorType::OpDec || op.type == OperatorType::OpNegate || op.type == OperatorType::OpNot || op.type == OperatorType::OpBitNot) {
					//Pre inc/dec

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

					opNode->type = op.type == OperatorType::OpInc ? OperatorType::OpPreInc : op.type == OperatorType::OpDec ?  OperatorType::OpPreDec : op.type;

					if (nodes.GetSize() > 0) {
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

						ASTNode*        leftOpNode = nodes.Back();
						OperatorTypeDef opLeft     = GetOperator(((OperatorNode*)leftOpNode)->type, OperandType::Any, OperandType::Any, true);

						if (opLeft.precedence < nextOp.precedence || (nextOp.precedence == op.precedence && op.associativty == OperatorAssociativity::LTR)) {
							ASTNode* tmp = opNode;

							nodes.PopBack();

							if (opLeft.leftOperand == OperandType::Any) {
								leftOpNode->AddNode(nodes.Back());
								nodes.PopBack();
							}

							leftOpNode->AddNode(opNode);

							opNode = (OperatorNode*)leftOpNode;
						}
					}
				}
			}

			nodes.PushBack(opNode);
		} else if (token.type == TokenType::Semicolon || token.type == TokenType::Comma) {
			BacktrackNodes(nodes);
			currentNode->AddNode(nodes.Back());


			if (token.type == TokenType::Semicolon)
				return i;

			nodes.Clear();
		}
	}

	return ~0;
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

void Syntax::BacktrackNodes(List<ASTNode*>& nodes) {
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
