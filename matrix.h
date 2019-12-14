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

class Flyweight {

};

template<typename T>
class ValueFlyweight : public Flyweight {

public:
	T value;
};

// (extracted from Matrix for to define operator<< with it)
template<typename T, int defval>
class MatrixValueFlyweight: public ValueFlyweight<T> {
	unsigned row;
	unsigned col;
	Matrix<T, defval>* parent;
public:
	MatrixValueFlyweight(Matrix<T, defval>* parent, unsigned row, unsigned col) :
		parent{ parent }, row{ row }, col{ col } {}

	T operator*() const {
		auto it = parent->data.find(tuple{ row, col });
		return it != parent->data.end() ? it->second : defval;
	}
	MatrixValueFlyweight<T, defval>& operator=(const T& val) {
		parent->add(row, col, val);
		return *this;
	}
	MatrixValueFlyweight<T, defval>& operator=(MatrixValueFlyweight<T, defval>& iter) {
		return this->operator=(*iter);
	}

	bool operator==(const T& right) {
		return parent->get(row, col) == right;
	}
};



template<typename Key, typename Flyweight>
class FlyweightFactory {
	std::function<Flyweight(Key)> fnDoNotExist;
protected:
	map<Key, Flyweight> data;
public:
	// TODO using references into function (Flyweight&, Key&)
	FlyweightFactory(std::function<tuple<Flyweight, bool>(Key)> fnDoNotExist) : fnDoNotExist{ fnDoNotExist } {}
	T get(Key& key, bool* pis_find = nullptr) {
		auto it = data.find(key);
		if (it != data.end()) {
			if (pis_find != nullptr) *pis_find = true;
			return it->second;
		}
		if (pis_find != nullptr) *pis_find = false;
		auto [val, isadd] = val_isadd = fnDoNotExist(key);
		if (isadd) {
			data[key] = val;
		}
		return val;
	}
};

template<typename Key, typename Flyweight, int defval>
class MatrixFlyweightFactory: FlyweightFactory<key, Flyweight> {
private:
	size_t length;
	ValueFlyweight<int> def_value{ defval };
public:
	// TODO use references
	MatrixFlyweightFactory() : FlyweightFactory{ [](Key key) {return tuple{def_value, false}; } }
	{}
	void add(Key& key, T& elt) {
		bool is_finded;
		auto it = data.find(key);
		if (it != data.end()) {
			if (it->second == defval) {
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

template<typename T, int defval>
std::ostream& operator<<(std::ostream& out, const MatrixValueFlyweight<T, defval>& iter) {
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
	friend class MatrixValueFlyweight<T, defval>;
	using CurIterator = MatrixIterator<T, defval>;

	// For using matrix[x][y]
	template<typename T>
	class MatrixPartValue {
		unsigned row;
		Matrix<T, defval>* parent;
	public:
		MatrixPartValue(Matrix<T, defval>* parent, unsigned row) :
			parent{ parent }, row{ row } {}
		MatrixValueFlyweight<T, defval> operator[](unsigned col) {
			return MatrixValueFlyweight<T, defval>{ parent, row, col, };
		}
	};
	//// end Helper classes

	constexpr static decltype(defval) def_value = defval;

	size_t size() {
		return length;
	}

	MatrixValueFlyweight<T, defval> operator()(unsigned row_idx, unsigned col_idx) {
		return MatrixValueFlyweight<T, defval>{ this, row_idx, col_idx };
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

