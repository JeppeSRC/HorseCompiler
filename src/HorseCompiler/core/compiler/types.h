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

#include <core/def.h>
#include <util/string.h>

class Type {
public:
	String name;

	enum {
		Unkown,
		Scalar,
		Vec,
		Mat,
		Struct,
		TypeDef,
		Other
	};

	uint8 type;
	uint8 constness;

	Type(const String& name, uint8 type, uint8 constness) : name(name), type(type), constness(constness) { }
};

class TypeScalar : public Type {
public:
	enum {
		Float,
		Int
	};

	uint8 scalarType;
	uint8 bits;
	uint8 sign;

	TypeScalar(const String& name, uint8 type, uint8 constness, uint8 bits, uint8 sign) : Type(name, Type::Scalar, constness), scalarType(type), bits(bits), sign(sign) { }

	bool operator==(const TypeScalar* other) const {
		return operator==(*other);
	}

	bool operator==(const TypeScalar& other) const {
		return scalarType == other.scalarType && bits == other.bits && sign == other.sign && constness == other.constness;
	}
};

class TypeVec : public Type {
public:
	TypeScalar* component;
	uint8       components;

	TypeVec(const String& name, TypeScalar* component, uint8 components, uint8 constness) : Type(name, Type::Vec, constness), component(component), components(components) { }

	bool operator==(const TypeVec* other) const {
		return operator==(*other);
	}

	bool operator==(const TypeVec& other) const {
		return *component == other.component && components == other.components;
	}
};

class TypeMat : public Type {
public:
	TypeScalar* component;
	uint8       columns;
	uint8       rows;

	TypeMat(const String& name, TypeScalar* component, uint8 columns, uint8 rows, uint8 constness) : Type(name, Type::Mat, constness), component(component), columns(columns), rows(rows) { }

	bool operator==(const TypeMat* other) const {
		return operator==(*other);
	}

	bool operator==(const TypeMat& other) const {
		return *component == other.component && rows == other.rows && columns == other.columns;
	}
};

class TypeStruct : public Type {
public:
	List<Type*> elements;

	TypeStruct(const String& name, const List<Type*>& elements, uint8 constness) : Type(name, Type::Struct, constness), elements(elements) { }
};

class TypeTypeDef : public Type {
public:
	Type* actualType;

	TypeTypeDef(const String& name, Type* type) : Type(name, Type::TypeDef, 0), actualType(type) { }
};