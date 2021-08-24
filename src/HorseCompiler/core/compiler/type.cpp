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

#include "type.h"
#include "compiler.h"

Type* TypeTable::CreateType(ASTNode* node, bool* isConst) {
	if (node->nodeType != ASTType::Type) {
		//TODO: handle internal errors
		return nullptr;
	}

	List<Token*>& tokens = ((TypeNode*)node)->tokens;
	Token         signToken;
	Token         constToken;
	uint8         sign      = 2;
	uint8         constness = 0;

	Type*         tmp  = nullptr;
	PrimitiveType type = PrimitiveType::Unknown;

	uint64 i = 0;

	for (; i < tokens.GetSize(); i++) {
		Token& token = *tokens[i];

		if (token.type != TokenType::PrimitiveType) {
			if (token.type == TokenType::Identifier) {
				Type* tmptmp = GetType(token.string);

				if (tmptmp == nullptr) {
					break;
				} else if (type != PrimitiveType::Unknown) {
					Compiler::Log(token, HC_ERROR_SEMANTIC_TYPE_FOLLOWED_BY_TYPE, GetPrimitiveTypeString(type).str, tmp->name.str);
					return nullptr;
				}

				if (tmp != nullptr) {
					Compiler::Log(token, HC_ERROR_SEMANTIC_TYPE_FOLLOWED_BY_TYPE, tmp->name, tmptmp->name);
					return nullptr;
				}

				tmp = tmptmp;
			} else {
				break;
			}
		} else {
			switch (token.primitiveType) {
				case PrimitiveType::Const:
					if (constness) {
						Compiler::Log(token, HC_WARN_SEMANTIC_SAME_TYPE_QUALIFIER);
					}

					constness  = 1;
					constToken = token;
					break;
				case PrimitiveType::Unsigned:
					if (sign == 0) {
						Compiler::Log(token, HC_WARN_SEMANTIC_SAME_TYPE_QUALIFIER);
						return nullptr;
					} else if (sign == 1) {
						Compiler::Log(token, HC_ERROR_SEMANTIC_SIGNED_UNSIGNED_EXCLUSIVE);
						return nullptr;
					}

					sign      = 0;
					signToken = token;
					break;
				case PrimitiveType::Signed:
					if (sign == 1) {
						Compiler::Log(token, HC_WARN_SEMANTIC_SAME_TYPE_QUALIFIER);
						return nullptr;
					} else if (sign == 0) {
						Compiler::Log(token, HC_ERROR_SEMANTIC_SIGNED_UNSIGNED_EXCLUSIVE);
						return nullptr;
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
						Compiler::Log(token, HC_ERROR_SEMANTIC_TYPE_FOLLOWED_BY_TYPE, GetPrimitiveTypeString(type).str, GetPrimitiveTypeString(token.primitiveType).str);
						return nullptr;
					}

					type = token.primitiveType;
			}
		}
	}

	if (isConst) {
		*isConst = (bool)constness;
	}

	if (type != PrimitiveType::Unknown) {
		if (type != PrimitiveType::Byte && type != PrimitiveType::Short && type != PrimitiveType::Int) {
			if (sign != 2) {
				Compiler::Log(signToken, HC_ERROR_SEMANTIC_SIGNED_UNSIGNED_NOT_ALLOWED_ON_TYPE, sign == 0 ? "unsigned" : "signed", GetPrimitiveTypeString(type).str);
				return nullptr;
			}
		}

		switch (type) {
			case PrimitiveType::Byte:
			case PrimitiveType::Short:
			case PrimitiveType::Int:
			case PrimitiveType::Float:
				tmp = MakeTypeScalar(type, sign);
				break;
			case PrimitiveType::Vec2:
			case PrimitiveType::Vec4:
			case PrimitiveType::Vec3:
				tmp = MakeTypeVec(type);
				break;
			case PrimitiveType::Mat4:
				tmp = MakeTypeMat(type);
		}
	} else if (tmp) {
		if (tmp->type == Type::TypeDef) {
			TypeTypeDef* def = (TypeTypeDef*)tmp;

			if (constness == 1) {
				if (def->constness == 1) {
					Compiler::Log(constToken, HC_WARN_SEMANTIC_SAME_TYPE_QUALIFIER);
				}
			}

			if (sign != 2) {
				bool error = false;
				if (def->actualType->type != Type::Scalar) {
					error = true;
				} else if (((TypeScalar*)def->actualType)->scalarType == TypeScalar::Float) {
					error = true;
				}

				if (error) {
					Compiler::Log(signToken, HC_ERROR_SEMANTIC_SIGNED_UNSIGNED_NOT_ALLOWED_ON_TYPE, def->name.str, sign == 0 ? "unsigned" : "signed");
					return nullptr;
				}
			}
		} else {
			if (sign != 2) {
				Compiler::Log(signToken, HC_ERROR_SEMANTIC_SIGNED_UNSIGNED_NOT_ALLOWED_ON_TYPE, tmp->name.str, sign == 0 ? "unsigned" : "signed");
				return nullptr;
			}
		}
	} else {
		Compiler::Log(*tokens[i], HC_ERROR_SYNTAX_ERROR);
		return nullptr;
	}

	return tmp;
}

Type* TypeTable::GetType(const String& name) {
	return nullptr;
}

String TypeTable::GetPrimitiveTypeString(PrimitiveType type) {
	Language* lang = Language::Default();
	for (uint64 i = 0; i < lang->primitiveTypes.GetSize(); i++) {
		auto& t = lang->primitiveTypes[i];

		if (t.type == type)
			return t.def;
	}

	return "";
}

TypeScalar* TypeTable::MakeTypeScalar(PrimitiveType type, uint8 sign) {
	String name("");

	// If sign == 2 it will be set the default sign value
	if (type != PrimitiveType::Float) {
		if (sign == 0) {
			name += "unsigned ";
		} else if (sign == 1) {
			name += "signed ";
		}
	}

	TypeScalar* tmp = nullptr;

	switch (type) {
		case PrimitiveType::Byte:
			tmp = new TypeScalar("char", TypeScalar::Int, 8, sign);
			break;
		case PrimitiveType::Short:
			tmp = new TypeScalar("short", TypeScalar::Int, 16, sign);
			break;
		case PrimitiveType::Int:
			tmp = new TypeScalar("int", TypeScalar::Int, 32, sign);
			break;
		case PrimitiveType::Float:
			tmp = new TypeScalar("float", TypeScalar::Float, 32, sign);
			break;
	}

	for (Type* type : types) {
		if (*type == tmp) {
			delete tmp;
			return (TypeScalar*)type;
		}
	}

	types.PushBack(tmp);

	return tmp;
}

TypeVec* TypeTable::MakeTypeVec(PrimitiveType type) {
	TypeVec* tmp = nullptr;

	switch (type) {
		case PrimitiveType::Vec2:
			tmp = new TypeVec("vec2", MakeTypeScalar(PrimitiveType::Float, 0), 2);
			break;
		case PrimitiveType::Vec3:
			tmp = new TypeVec("vec3", MakeTypeScalar(PrimitiveType::Float, 0), 3);
			break;
		case PrimitiveType::Vec4:
			tmp = new TypeVec("vec4", MakeTypeScalar(PrimitiveType::Float, 0), 4);
			break;
	}

	for (Type* type : types) {
		if (*type == tmp) {
			delete tmp;
			return (TypeVec*)type;
		}
	}

	types.PushBack(tmp);

	return tmp;
}

TypeMat* TypeTable::MakeTypeMat(PrimitiveType type) {
	TypeMat* tmp = nullptr;

	switch (type) {
		case PrimitiveType::Mat4:
			tmp = new TypeMat("mat4", MakeTypeScalar(PrimitiveType::Float, 0), 4, 4);
			break;
	}

	for (Type* type : types) {
		if (*type == tmp) {
			delete tmp;
			return (TypeMat*)type;
		}
	}

	types.PushBack(tmp);

	return tmp;
}