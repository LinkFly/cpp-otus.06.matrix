#pragma once

#include "share.h"

#include <map>
#include <iostream>

using std::map;

template<typename T, int defval>
class Matrix;

template<typename T, int defval>
struct MatrixIterator {
	MatrixIterator(Matrix<T, defval>* parent, unsigned row_idx, unsigned col_idx) :
		parent{ parent }, cur_row{ row_idx }, cur_col{ col_idx } {}
	T operator*() const {
		return parent->get(cur_row, cur_col);
	}
	bool operator==(const T& right) {
		return parent->get(cur_row, cur_col) == right;
	}

	MatrixIterator& operator=(const T& val) {
		parent->add(cur_row, cur_col, val);
		return *this;
	}

	auto find(unsigned key) {
		return parent->data.find(key);
	}

private:
	Matrix<T, defval>* parent;
	unsigned cur_row;
	unsigned cur_col;
};

template<typename T, int defval>
class Matrix {
	using row = map<unsigned, T>;
	using cur_iterator = MatrixIterator<T, defval>;
	friend struct MatrixIterator<T, defval>;
	map<unsigned, row> data;
	size_t length = 0;

	bool getRow(unsigned row_idx, row*& row) {
		auto it = data.find(row_idx);
		if (it != data.end()) {
			row = &it->second;
			return true;
		}
		return false;
	}

public:
	constexpr static decltype(defval) defval = defval;
	struct part_iterator {
		part_iterator(Matrix<T, defval>* parent, unsigned row_idx) :
			parent{ parent }, cur_row{ row_idx } {}
		cur_iterator operator[](const unsigned& col_idx) {
			return MatrixIterator<T, defval>{ parent, cur_row, col_idx };
		}
	private:
		Matrix<T, defval>* parent;
		unsigned cur_row;
	};

	T get(unsigned row_idx, unsigned col_idx) {
		row* prow;
		if (getRow(row_idx, prow)) {
			auto it = prow->find(col_idx);
			if (it != prow->end()) {
				return it->second;
			}
		}
		return defval;
	}

	void add(unsigned row_idx, unsigned col_idx, const T& elt) {
		row* prow;
		if (getRow(row_idx, prow)) {
			auto it = prow->find(col_idx);
			if (it != prow->end()) {
				it->second = elt;
				elt == defval && length--;
			}
			else {
				if (elt == defval) return;
				length++;
				(*prow)[col_idx] = elt;
			}
		}
		else {
			if (elt == defval) return;
			length++;
			// data destructor will do to free memory
			auto new_row = new row{};
			(*new_row)[col_idx] = elt;
			data.emplace(row_idx, *new_row);
		}
	}

	size_t size() {
		return length;
	}

	cur_iterator operator()(unsigned row_idx, unsigned col_idx) {
		return cur_iterator{ this, row_idx, col_idx };
	}
	
	part_iterator operator[](unsigned row_idx) {
		return part_iterator{ this, row_idx };
	}

	/*MatrixIterator& begin() {
		return MatrixIterator{ this, 0, 0 };
	}*/
};
