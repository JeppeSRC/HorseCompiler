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

#include "semantic.h"
#include <core/compiler/compiler.h>
#include <core/compiler/misc/type.h>

uint64 Semantic::Analyze(ASTNode* node, TypeTable* typeTable, SymbolTable* symbolTable) {
	Semantic sem(typeTable, symbolTable);

	return sem.Analyze(node);
}

uint64 Semantic::Analyze(ASTNode* root) {
	List<ASTNode*>& branches = root->branches;

	for (uint64 i = 0; i < branches.GetSize(); i++) {
		ASTNode* node = branches[i];

		if (node->nodeType == ASTType::VariableDefinition) {
			uint64 res = VariableDefinition(node);
		}
	}

	return 0;
}

uint64 Semantic::VariableDefinition(ASTNode* node) {
	bool  isConst = false;
	Type* type    = typeTable->CreateType(node->branches[0], &isConst);

	bool        sameScope = false;
	StringNode* name      = (StringNode*)node->branches[1];
	Symbol*     symbol    = symbolTable->GetSymbol(name->string, &sameScope);

	if (symbol == nullptr || !sameScope) {
		if (symbol) {
			//Compiler::Log(*(name->token), HC_WARN_SEMANTIC_PARENT_SCOPE_SYMBOL_REDEFINITION);
		}

		symbol = new SymbolVariable(name->string, type, isConst, name->token);
	} else {
		Compiler::Log(*(name->token), HC_ERROR_SEMANTIC_SYMBOL_REDEFINITION);
		return ~0;
	}

	symbolTable->AddSymbol(symbol);

	if (node->branches.GetSize() == 3) {
		ASTNode* result;

		uint64 error = ConstantEvaluation(node->branches[2], &result);

		if (result->nodeType != ASTType::Constant) {
			// TODO: handle errors with new logging system
		}

		((SymbolVariable*)symbol)->initialValue = (ConstantNode*)result;
	}

	return 0;
}

uint64 Semantic::ConstantEvaluation(ASTNode* node, ASTNode** result) {

	if (node->nodeType == ASTType::Constant) {
		*result = node;
	} else if (node->nodeType == ASTType::Variable) {
		Symbol* symbol = symbolTable->GetSymbol(((StringNode*)node->branches[0])->string);

		if (symbol->type == SymbolType::Variable) {
			SymbolVariable* smbl = (SymbolVariable*)symbol;

			if (smbl->initialValue != nullptr && !smbl->modified) {
				*result = smbl->initialValue;
			} else {
				*result = node;
			}
		} else {
			//TODO: error
			HC_ASSERT(false);
		}
	} else if (node->nodeType == ASTType::Operator) {
		OperatorNode* op = (OperatorNode*)node;

		uint64 res = ProcessOperator(op, result);

	} else {
		//TODO: error
		HC_ASSERT(false);
	}

	return uint64();
}

uint64 Semantic::ProcessOperator(OperatorNode* node, ASTNode** result) {
	ASTNode* left = node->branches[0];
	ASTNode* right = node->branches.GetSize() > 1 ? node->branches[1] : nullptr;

	switch (node->type) {
		case OperatorType::OpAssign:
		case OperatorType::OpAdd:
		case OperatorType::OpSub:
		case OperatorType::OpMul:
		case OperatorType::OpDiv:
		case OperatorType::OpCompoundAdd:
		case OperatorType::OpCompoundSub:
		case OperatorType::OpCompoundMul:
		case OperatorType::OpCompoundDiv:
		case OperatorType::OpBitAnd:
		case OperatorType::OpBitOr:
		case OperatorType::OpBitXor:
		case OperatorType::OpLeftShift:
		case OperatorType::OpRightShift:
		case OperatorType::OpAnd:
		case OperatorType::OpOr:
		case OperatorType::OpEqual:
		case OperatorType::OpNotEqual:
		case OperatorType::OpLess:
		case OperatorType::OpGreater:
		case OperatorType::OpLessEq:
		case OperatorType::OpGreaterEq:
			*result = ExecuteOperator(node->type, left, right);
			break;
		case OperatorType::OpNegate:
		case OperatorType::OpBitNot:
		case OperatorType::OpNot:
		case OperatorType::OpPostInc:
		case OperatorType::OpPreInc:
		case OperatorType::OpPostDec:
		case OperatorType::OpPreDec:
			*result = ExecuteOperator(node->type, left);
			break;
		default:
			HC_ASSERT(false);

	}

	return 0;
}

ASTNode* Semantic::ExecuteOperator(OperatorType op, ASTNode* left, ASTNode* right) {
	if (op == OperatorType::OpAssign || op == OperatorType::OpCompoundAdd || op == OperatorType::OpCompoundSub || op == OperatorType::OpCompoundMul || op == OperatorType::OpCompoundDiv) {
		if (left->nodeType != ASTType::Variable) {
			//TODO: error
			HC_ASSERT(false);
		}

		StringNode* lName = (StringNode*)left->branches[0];
		Symbol* symbol = symbolTable->GetSymbol(lName->string);

		if (symbol == nullptr) {
			//TODO: error
			HC_ASSERT(false);
		}

		if (symbol->type != SymbolType::Variable) {
			//TODO: error
			HC_ASSERT(false);
		}

		SymbolVariable* smbl = (SymbolVariable*)symbol;

		if (smbl->constness) {
			//TODO: error
			HC_ASSERT(false);
		}

		ConstantNode* rConstant = nullptr;

		if (right->nodeType == ASTType::Operator) {
			uint64 res = ProcessOperator((OperatorNode*)right, &right);
		}

		if (right->nodeType == ASTType::Variable) {
			StringNode* rName = (StringNode*)right->branches[0];
			Symbol* symbol = symbolTable->GetSymbol(rName->string);

			if (symbol->type != SymbolType::Variable) {
				//TODO: error
				HC_ASSERT(false);
			}

			SymbolVariable* smbl = (SymbolVariable*)symbol;

			rConstant = smbl->initialValue;
		} else if (right->nodeType == ASTType::Function) {
			rConstant = nullptr;
		} else if (right->nodeType == ASTType::Constant) {
			rConstant = (ConstantNode*)right;
		}  else {
			//TODO: error
			HC_ASSERT(false);
		}

		if (rConstant == nullptr) return nullptr;


	}
}

ASTNode* Semantic::ExecuteOperator(OperatorType op, ASTNode* operand) {
	return nullptr;
}