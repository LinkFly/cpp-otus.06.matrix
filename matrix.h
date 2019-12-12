#pragma once

#include "share.h"

#include <map>
#include <tuple>
#include <iostream>

using std::map;
using std::tuple;

template<typename T, int defval>
class Matrix;

// (extracted from Matrix for to define operator<< with it)
template<typename T, int defval>
class MatrixValue {
	unsigned row;
	unsigned col;
	Matrix<T, defval>* parent;
public:
	MatrixValue(Matrix<T, defval>* parent, unsigned row, unsigned col) :
		parent{ parent }, row{ row }, col{ col } {}

	T operator*() const {
		auto it = parent->data.find(tuple{ row, col });
		return it != parent->data.end() ? it->second : defval;
	}
	MatrixValue<T, defval>& operator=(const T& val) {
		parent->add(row, col, val);
		return *this;
	}
	MatrixValue<T, defval>& operator=(MatrixValue<T, defval>& iter) {
		return this->operator=(*iter);
	}

	bool operator==(const T& right) {
		return parent->get(row, col) == right;
	}
};

template<typename T, int defval>
std::ostream& operator<<(std::ostream& out, const MatrixValue<T, defval>& iter) {
	out << (*iter);
	return out;
}

template<typename T, int defval>
class Matrix {
	map<tuple<unsigned, unsigned>, T> data;
	size_t length = 0;

public:
	//// Helper classes
	template<typename T, int defval>
	class MatrixIterator {
		friend MatrixIterator<T, defval>;

		Matrix<T, defval>* parent;
		using LowIterator = typename decltype(parent->data)::iterator;
		LowIterator it;
	public:
		MatrixIterator(Matrix<T, defval>* parent) : 
			parent{ parent } {
			if (parent != nullptr) {
				it = parent->data.begin();
				if (it == parent->data.end())
					parent = nullptr;
			}
		}

		static MatrixIterator begin(Matrix<T, defval>* parent) {
			return MatrixIterator{ parent };
		}

		static MatrixIterator end() {
			return MatrixIterator(nullptr);
		}

		std::tuple<std::tuple<unsigned, unsigned>, T> operator*() const {
			return *it;
		}

		bool operator==(const T& right) {
			return *it == right;
		}

		MatrixIterator& operator=(const T& val) {
			if (it == end()) {
				// TODO test throw
				throw "Out of bound";
			}
			*it = val;
			return *this;
		}

		MatrixIterator& operator=(const MatrixIterator& iter) {
			return this->operator*(*iter);
		}

		MatrixIterator& operator++() {
			if (parent != nullptr) {
				++it;
				if (it == parent->data.end()) {
					parent = nullptr;
				}
			}
			return *this;
		}

		bool operator==(MatrixIterator& right) {
			if (parent == right.parent) {
				return parent == nullptr || it == right.it;
			}
			return false;
		}

		bool operator!=(MatrixIterator& right) {
			return !operator==(right);
		}
	};

	friend class MatrixIterator<T, defval>;
	friend class MatrixValue<T, defval>;
	using CurIterator = MatrixIterator<T, defval>;

	// For using matrix[x][y]
	template<typename T>
	class MatrixPartValue {
		unsigned row;
		Matrix<T, defval>* parent;
	public:
		MatrixPartValue(Matrix<T, defval>* parent, unsigned row) :
			parent{ parent }, row{ row } {}
		MatrixValue<T, defval> operator[](unsigned col) {
			return MatrixValue<T, defval>{ parent, row, col, };
		}
	};
	//// end Helper classes

	constexpr static decltype(defval) def_value = defval;

	size_t size() {
		return length;
	}

	MatrixValue<T, defval> operator()(unsigned row_idx, unsigned col_idx) {
		return MatrixValue<T, defval>{ this, row_idx, col_idx };
	}

	MatrixPartValue<T> operator[](unsigned row_idx) {
		return MatrixPartValue<T>{ this, row_idx };
	}

	CurIterator begin() {
		return CurIterator::begin(this);
	}
	
	CurIterator end() {
		return CurIterator::end();
	}

private:
	T get(unsigned row_idx, unsigned col_idx) {
		auto it = data.find(tuple{ row_idx, col_idx });
		if (it != data.end()) {
			return it->second;
		}
		return defval;
	}
	void add(unsigned row_idx, unsigned col_idx, const T& elt) {
		auto it = data.find(tuple{ row_idx, col_idx });
		if (it != data.end()) {
			if (elt == defval) {
				data.erase(it);
				length--;
			}
			else {
				it->second = elt;
			}
		}
		else if (it == data.end()) {
			if (elt == defval) {
				return;
			}
			data[tuple{ row_idx, col_idx }] = elt;
			length++;
		}
	}
};

