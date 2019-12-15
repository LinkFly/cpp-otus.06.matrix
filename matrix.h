#pragma once

#include "share.h"

#include <map>
#include <tuple>
#include <iostream>
#include <functional>

using std::map;
using std::tuple;

template<typename T, int defval>
class Matrix;

template<typename T, T defval>
class MatrixValueProxy {
	Matrix<T, defval>* parent;
	unsigned row;
	unsigned col;
public:
	MatrixValueProxy(Matrix<T, defval>* parent, unsigned row, unsigned col) :
		parent{ parent }, row{ row }, col{ col } {}
	T operator*() const {
		return parent->get(row, col);
	}
	MatrixValueProxy<T, defval>& operator=(const T& val) {
		parent->add(row, col, val);
		return *this;
	}
	MatrixValueProxy<T, defval>& operator=(MatrixValueProxy<T, defval>& right) {
		return operator=(*right);
	}

	bool operator==(const T& right) {
		return parent->get(row, col) == right;
	}

	bool operator==(const MatrixValueProxy& right) {
		return operator==(*right);
	}
};

template<typename T, int defval>
std::ostream& operator<<(std::ostream& out, const MatrixValueProxy<T, defval>& iter) {
	out << (*iter);
	return out;
}

template<typename T, int defval>
struct MatrixFlyweightFactory {
	Matrix<T, defval>* parent;
	using Key = tuple<unsigned, unsigned>;
	using MapProxies = map<Key, MatrixValueProxy<T, defval>*>;
	MapProxies data;
public:
	MatrixFlyweightFactory(Matrix<T, defval>* parent) : parent{ parent } {}
	MatrixValueProxy<T, defval>& get(const Key& key) {
		auto it = data.find(key);
		if (it != data.end()) {
			return *it->second;
		}
		auto new_proxy = new MatrixValueProxy{ parent, std::get<0>(key), std::get<1>(key) };
		data.insert(it, { key, new_proxy });
		return *new_proxy;
	}
	~MatrixFlyweightFactory() {
		for (auto it : data) {
			delete it.second;
		}
	}
};

template<typename T, int defval>
class MatrixIterator {
	Matrix<T, defval>* parent;
	using LowIterator = typename Matrix<T, defval>::iterator;
	LowIterator it;
public:
	MatrixIterator(Matrix<T, defval>* parent) : parent{ parent } {
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
		auto [key, val] = *it;
		return tuple{ key, val };
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
		return this->operator=(*iter);
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

template<typename T, int defval>
class Matrix {
	map<tuple<unsigned, unsigned>, T> data;
	size_t length = 0;
	MatrixFlyweightFactory<T, defval> mtx_fly_factory;	
public:
	friend class MatrixIterator<T, defval>;
	friend class MatrixValueProxy<T, defval>;
	using iterator = typename map<tuple<unsigned, unsigned>, T>::iterator;
	using CurIterator = MatrixIterator<T, defval>;
	using ValueProxyType = MatrixValueProxy<T, defval>;

	// For using matrix[x][y]
	class MatrixPartValue {
		Matrix<T, defval>* parent;
		unsigned row;
	public:
		MatrixPartValue(Matrix<T, defval>* parent, unsigned row) :
			parent{ parent }, row{ row } {}
		MatrixValueProxy<T, defval> operator[](unsigned col) {
			/*return MatrixValueProxy<T, defval>{ parent, row, col, };*/
			return parent->get_val_proxy(row, col);
		}
	};

	Matrix() : mtx_fly_factory{ this } {}

	size_t size() {
		return length;
	}

	MatrixValueProxy<T, defval> operator()(unsigned row_idx, unsigned col_idx) {
		/*return MatrixValueProxy<T, defval>{ this, row_idx, col_idx };*/
		return get_val_proxy(row_idx, col_idx);
	}

	MatrixPartValue operator[](unsigned row_idx) {
		return MatrixPartValue{ this, row_idx };
	}

	CurIterator begin() {
		return CurIterator::begin(this);
	}
	
	CurIterator end() {
		return CurIterator::end();
	}

private:
	//mtx_fly_factory
	ValueProxyType get_val_proxy(unsigned row_idx, unsigned col_idx) {
		return mtx_fly_factory.get(tuple{ row_idx, col_idx });
	}
	// O(n) = log(size())
	T get(unsigned row_idx, unsigned col_idx) {
		auto it = data.find(tuple{ row_idx, col_idx });
		if (it != data.end()) {
			return it->second;
		}
		return defval;
	}
	// O(n) = log(size())
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
			data.insert(it, { tuple{row_idx, col_idx}, elt });
			length++;
		}
	}
};

