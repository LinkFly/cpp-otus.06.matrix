#pragma once

#include "share.h"

#include <map>
#include <tuple>
#include <iostream>

using std::map;

template<typename T, int defval>
class Matrix;

template<typename T, int defval>
struct MatrixIterator {
	using RowIterator = typename Matrix<T, defval>::table::iterator;
	using ColumnIterator = typename Matrix<T, defval>::row::iterator;
	friend MatrixIterator<T, defval>;

	MatrixIterator(Matrix<T, defval>* parent, unsigned row_idx, unsigned col_idx) :
		parent{ parent }, cur_row{ row_idx }, cur_col{ col_idx } {
	}
	MatrixIterator(Matrix<T, defval>* parent): MatrixIterator(parent, 0, 0) {}
	MatrixIterator(Matrix<T, defval>* parent, RowIterator& row_iter, ColumnIterator& col_iter) : parent{ parent } {
		prow_iter = new RowIterator{};
		pcol_iter = new ColumnIterator{};
		*prow_iter = row_iter;
		*pcol_iter = col_iter;
	}

	std::tuple<std::tuple<unsigned, unsigned>, T> operator*() const {
		if ((prow_iter != nullptr) && (pcol_iter != nullptr)) {
			return std::make_tuple(
				std::make_tuple((*prow_iter)->first, (*pcol_iter)->first),
				(*pcol_iter)->second // use *->
			);
		}
		return std::make_tuple(
			std::make_tuple(cur_row, cur_col),
			parent->get(cur_row, cur_col)
		);
	}

	bool operator==(const T& right) {
		return parent->get(cur_row, cur_col) == right;
	}

	MatrixIterator& operator=(const T& val) {
		parent->add(cur_row, cur_col, val);
		return *this;
	}

	MatrixIterator& operator=(const MatrixIterator& iter) {
		parent->add(cur_row, cur_col, std::get<1>(*iter)); // use *->
		return *this;
	}

	auto find(unsigned key) {
		return parent->data.find(key);
	}
	~MatrixIterator() {
		delete prow_iter;
		delete pcol_iter;
	}

	static MatrixIterator begin(Matrix<T, defval>* parent) {
		assert(parent != nullptr);
		auto row_iter = parent->data.begin();
		if (row_iter != parent->data.end()) {
			auto& fst_row = (*row_iter).second;
			auto col_iter = fst_row.begin();
			if (col_iter != fst_row.end()) {
				return MatrixIterator { parent, row_iter, col_iter };
			}
		}
		return end();
	}

	static MatrixIterator end() {
		return MatrixIterator(nullptr);
	}

	MatrixIterator& operator++() {
		if (parent == nullptr || prow_iter == nullptr) {
			// is end iterator
			parent = nullptr;
		}
		if (pcol_iter != nullptr) {
			++(*pcol_iter);
			if ((*pcol_iter) == (*(*prow_iter)).second.end()) {
				delete pcol_iter;
				pcol_iter = nullptr;
			}
		}
		if (pcol_iter == nullptr) {
			++(*prow_iter);
			if (*prow_iter != parent->data.end()) {
				pcol_iter = new ColumnIterator{};
				(*pcol_iter) = (*(*prow_iter)).second.begin();
			}
			else {
				// is end iterator
				parent = nullptr;
			}
		}
		return *this;
	}

	bool operator==(MatrixIterator& right) {
		if (parent == right.parent) {
			return parent == nullptr || (
				(*prow_iter == *(right.prow_iter)) &&
				(*pcol_iter == *(right.pcol_iter))
			);
		}
		return false;
	}
	bool operator!=(MatrixIterator& right) {
		return !operator==(right);
	}

private:
	Matrix<T, defval>* parent;
	unsigned cur_row;
	unsigned cur_col;
	RowIterator* prow_iter = nullptr;
	ColumnIterator* pcol_iter = nullptr;
	/*typename decltype(parent-::data[0])::iterator ColIter;*/
};

template<typename T, int defval>
class Matrix {
	using row = map<unsigned, T>;
	using table = map<unsigned, row>;
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
				if (elt != defval) {
					it->second = elt;
				}
				else {
					length--;
					prow->erase(it);
				}
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

	cur_iterator begin() {
		return cur_iterator::begin(this);
	}
	
	cur_iterator end() {
		return cur_iterator::end();
	}
};
