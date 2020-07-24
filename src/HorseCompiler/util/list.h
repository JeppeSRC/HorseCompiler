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

#pragma once

#include <core/def.h>
#include <vector>
#include <tuple>
#include <core/error/error.h>

template<typename T>
class List {
private:
	std::vector<T> items;

public:
	List() = default;
	List(uint64 reserve) {
		Reserve(reserve);
	}

	List(const List& other) = default;
	List(List&& other) = default;
	
	List& operator=(const List& other) = default;
	List& operator=(List&& other) = default;

	T& operator[](uint64 index) { 
		HC_ASSERT(index >= 0 && index < GetSize());
		return items[index]; 
	}
	const T& operator[](uint64 index) const { 
		HC_ASSERT(index >= 0 && index < GetSize());
		return items[index]; 
	}

	void Reserve(uint64 reserve) {
		HC_ASSERT(reserve != 0);
		items.reserve(reserve);
	}

	void PushBack(const T& item) {
		items.push_back(item);
	}

	T PopBack() {
		return std::move(items.pop_back());
	}

	std::tuple<uint64, T&> Find(const T& item, uint64 offset = 0) {
		HC_ASSERT(offset >= 0 && offset < GetSize());
		for (uint64 i = offset; i < items.size(); i++) {
			T& curr = items[i];
			if (curr == item) return { i, curr };
		}

		return { (uint64)~0, T() };
	}

	template<typename K>
	std::tuple<uint64, T&> Find(const K& item, bool(*CmpFunc)(const T&, const K&), uint64 offset = 0) {
		HC_ASSERT(offset >= 0 && offset < GetSize());
		for (uint64 i = offset; i < items.size(); i++) {
			T& curr = items[i];
			if (CmpFunc(curr, item)) return { i, curr };
		}

		return { (uint64)~0, T() };
	}

	uint64 GetSize() const { return items.size(); }

	auto begin() { return items.begin(); }
	auto end() { return items.end(); }

};