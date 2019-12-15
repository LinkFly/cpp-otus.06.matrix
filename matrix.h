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
	unsigned row;
	unsigned col;
	Matrix<T, defval>* parent;
	friend class MatrixValueProxy;
public:
	MatrixValueProxy(Matrix<T, defval>* parent, unsigned row, unsigned col, T elt = defval) :
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

template<typename Key, typename Flyweight>
class FlyweightFactory {
	std::function<tuple<Flyweight, bool>(Key)> fnDoNotExist;
protected:
	map<Key, Flyweight> data;
public:
	using iterator = typename map<Key, Flyweight>::iterator;
	// TODO using references into function (Flyweight&, Key&)
	FlyweightFactory(std::function<tuple<Flyweight, bool>(Key)> fnDoNotExist) : fnDoNotExist{ fnDoNotExist } {}
	Flyweight get(Key& key, bool* pis_find = nullptr) {
		auto it = data.find(key);
		if (it != data.end()) {
			if (pis_find != nullptr) *pis_find = true;
			return it->second;
		}
		if (pis_find != nullptr) *pis_find = false;
		auto [val, isadd] = fnDoNotExist(key);
		if (isadd) {
			data[key] = val;
		}
		return val;
	}
	//FlyweightFactory~() {
	//	for (auto it : data) {
	//		delete it->second;
	//	}
	//}
};

template<typename Key, typename Flyweight>
class FlyweightFactoryEx: public FlyweightFactory<Key, Flyweight> {
private:
	size_t length = 0;
	Flyweight* pdef_fly_val;
public:
	/*map<Key, Flyweight> data;*/
	/*FlyweightFactoryEx() : FlyweightFactory{ [&defval](Key key) {return tuple{defval, false}; } }
	{}*/
	// TODO use references
	FlyweightFactoryEx(Flyweight& defval) :
		FlyweightFactory{ [&defval](Key key) {return tuple{defval, true}; } },
		pdef_fly_val{ &defval }
	{}
	/*void add(Key& key, Flyweight& elt) {
		auto it = data.find(key);
		if (it != data.end()) {
			if (it->second == *pdef_fly_val) {
				data.erase(it);
				length--;
			}
			else {
				it->second = elt;
			}
		}
		else if (it == data.end()) {
			if (elt == *pdef_fly_val) {
				return;
			}
			data[key] = elt;
			length++;
		}
	}*/
};

using Key = tuple<unsigned, unsigned>;

template<typename T, int defval>
struct MatrixFlyweightFactory {
	Matrix<T, defval>* parent;
	using Key = tuple<unsigned, unsigned>;
	using MapProxies = map<Key, MatrixValueProxy<T, defval>>;
	MapProxies data;
public:
	MatrixFlyweightFactory(Matrix<T, defval>* parent) : parent{ parent } {}
	using iterator = typename MapProxies::iterator;
	// TODO using references into function (Flyweight&, Key&)
	MatrixValueProxy<T, defval>& get(Key& key) {
		auto it = data.find(key);
		if (it != data.end()) {
			return it->second;
		}
		auto new_proxy = new MatrixValueProxy{ parent, std::get<0>(key), std::get<1>(key) };
		data[key] = *new_proxy;
		return *new_proxy;
	}
	~MatrixFlyweightFactory() {
		for (auto it : data) {
			delete &it.second;
		}
	}
	auto begin() { return data.begin(); }
	auto end() { return data.end(); }
};

template<typename T, int defval>
class MatrixIterator {
	friend MatrixIterator<T, defval>;

	Matrix<T, defval>* parent;
	using LowIterator = typename Matrix<T, defval>::iterator;
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

template<typename T, int defval>
class Matrix {
	map<tuple<unsigned, unsigned>, T> data;
	size_t length = 0;
	MatrixFlyweightFactory<T, defval> mtx_fly_factory;	
public:
	using iterator = typename map<tuple<unsigned, unsigned>, T>::iterator;
	//// Helper classes
	

	friend class MatrixIterator<T, defval>;
	friend class MatrixValueProxy<T, defval>;
	using CurIterator = MatrixIterator<T, defval>;
	using ValueProxyType = MatrixValueProxy<T, defval>;

	// For using matrix[x][y]
	template<typename T>
	class MatrixPartValue {
		unsigned row;
		Matrix<T, defval>* parent;
	public:
		MatrixPartValue(Matrix<T, defval>* parent, unsigned row) :
			parent{ parent }, row{ row } {}
		MatrixValueProxy<T, defval> operator[](unsigned col) {
			return MatrixValueProxy<T, defval>{ parent, row, col, };
		}
	};
	//// end Helper classes

	constexpr static decltype(defval) def_value = defval;

	Matrix() : mtx_fly_factory{ this } {}

	size_t size() {
		return length;
	}

	MatrixValueProxy<T, defval> operator()(unsigned row_idx, unsigned col_idx) {
		return MatrixValueProxy<T, defval>{ this, row_idx, col_idx };
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
	//mtx_fly_factory
	ValueProxyType get_val_proxy(unsigned row_idx, unsigned col_idx) {
		return mtx_fly_factory.get(tuple{ row_idx, col_idx });
	}
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

