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

#pragma once

#include <util/string.h>
#include <util/list.h>
#include "type.h"
#include <core/compiler/lexer/token.h>

enum class SymbolType {
	Root,
	Variable,
	Function,
	Struct
};

class Symbol {
public:
	Token* token;
	Symbol*    parent;
	SymbolType type;

	String name;

	Symbol(SymbolType type, const String& name, Token* token) : token(token), parent(nullptr), type(type), name(name) { }

	List<Symbol*> symbols;

	void AddSymbol(Symbol* symbol) {
		symbol->parent = this;
		symbols.PushBack(symbol);
	}
};

class SymbolVariable : public Symbol {
public:
	Type* type;

	bool modified;
	bool constness;

	ConstantNode* initialValue;

	SymbolVariable(const String& name, Type* type, bool constness, Token* token) : Symbol(SymbolType::Variable, name, token), type(type), constness(constness), modified(false), initialValue(nullptr) { }
};

class SymbolTable {
private:
public:
	List<Symbol*> symbols;

	Symbol* currentScope = nullptr;

	void    AddSymbol(Symbol* symbol);
	Symbol* GetSymbol(const String& name, bool* isSameScope = nullptr);
};