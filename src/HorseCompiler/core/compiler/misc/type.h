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
#include <util/list.h>
#include <core/compiler/parsing/ast.h>

class Type {
private:
	friend class TypeScalar;
	friend class TypeVec;
	friend class TypeMat;
	friend class TypeStruct;
	friend class TypeTypeDef;

public:
	String name;

	enum {
		Unknown,
		Scalar,
		Vec,
		Mat,
		Struct,
		TypeDef,
		Other
	};

	uint8 type;

	virtual bool operator==(const Type* other) const {
		if (type != other->type)
			return false;

		switch (type) {
			case Type::Scalar:
				return operator==((TypeScalar*)other);
			case Type::Vec:
				return operator==((TypeVec*)other);
			case Type::Mat:
				return operator==((TypeMat*)other);
			case Type::Struct:
				return operator==((TypeStruct*)other);
			case Type::TypeDef:
				return operator==((TypeTypeDef*)other);
		}

		return false;
	}

	virtual bool operator==(const Type& other) const {
		return operator==(&other);
	}

	virtual bool operator==(const TypeScalar* other) const { return false; }
	virtual bool operator==(const TypeVec* other) const { return false; }
	virtual bool operator==(const TypeMat* other) const { return false; }
	virtual bool operator==(const TypeStruct* other) const { return false; }
	virtual bool operator==(const TypeTypeDef* other) const { return false; }

	virtual bool operator==(const TypeScalar& other) const { return false; }
	virtual bool operator==(const TypeVec& other) const { return false; }
	virtual bool operator==(const TypeMat& other) const { return false; }
	virtual bool operator==(const TypeStruct& other) const { return false; }
	virtual bool operator==(const TypeTypeDef& other) const { return false; }

protected:
	Type(const String& name, uint8 type) : name(name), type(type) { }
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

	TypeScalar(const String& name, uint8 type, uint8 bits, uint8 sign) : Type(name, Type::Scalar), scalarType(type), bits(bits), sign(sign) { }

	bool operator==(const TypeScalar* other) const override {
		return operator==(*other);
	}

	bool operator==(const TypeScalar& other) const override {
		return scalarType == other.scalarType && bits == other.bits && sign == other.sign;
	}
};

class TypeVec : public Type {
public:
	TypeScalar* component;
	uint8       components;

	TypeVec(const String& name, TypeScalar* component, uint8 components) : Type(name, Type::Vec), component(component), components(components) { }

	bool operator==(const TypeVec* other) const override {
		return operator==(*other);
	}

	bool operator==(const TypeVec& other) const override {
		return *component == other.component && components == other.components;
	}
};

class TypeMat : public Type {
public:
	TypeScalar* component;
	uint8       columns;
	uint8       rows;

	TypeMat(const String& name, TypeScalar* component, uint8 columns, uint8 rows) : Type(name, Type::Mat), component(component), columns(columns), rows(rows) { }

	bool operator==(const TypeMat* other) const override {
		return operator==(*other);
	}

	bool operator==(const TypeMat& other) const override {
		return *component == other.component && rows == other.rows && columns == other.columns;
	}
};

class TypeStruct : public Type {
public:
	List<Type*> elements;

	TypeStruct(const String& name, const List<Type*>& elements) : Type(name, Type::Struct), elements(elements) { }

	bool operator==(const TypeStruct* other) const override {
		return operator==(*other);
	}

	bool operator==(const TypeStruct& other) const override {
		return name == other.name;
	}
};

class TypeTypeDef : public Type {
public:
	Type* actualType;
	bool  constness;

	TypeTypeDef(const String& name, Type* type, bool constness) : Type(name, Type::TypeDef), actualType(type), constness(constness) { }

	bool operator==(const TypeTypeDef* other) const override {
		return operator==(*other);
	}

	bool operator==(const TypeTypeDef& other) const override {
		return name == other.name;
	}
};

class TypeTable {
public:
	List<Type*> types;

	Type* CreateType(ASTNode* node, bool* isConst);
	Type* GetType(const String& name);

	String GetPrimitiveTypeString(PrimitiveType type);

	TypeScalar* MakeTypeScalar(PrimitiveType type, uint8 sign);
	TypeVec*    MakeTypeVec(PrimitiveType type);
	TypeMat*    MakeTypeMat(PrimitiveType type);
};