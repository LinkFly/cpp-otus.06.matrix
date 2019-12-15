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

template<typename T>
struct Flyweight {
	T value;
};

template<typename T>
class ValueFlyweight : public Flyweight<T> {

public:
	bool operator==(const ValueFlyweight& right) {
		return this->value == right.value;
	}
};

// (extracted from Matrix for to define operator<< with it)
template<typename T, int defval>
class MatrixValueFlyweight: public ValueFlyweight<T> {
	//unsigned row;
	//unsigned col;
	Matrix<T, defval>* parent;
public:
	MatrixValueFlyweight() : MatrixValueFlyweight{ defval } {}
	MatrixValueFlyweight(const T val, Matrix<T, defval>* parent = nullptr) : parent{ parent } {
		value = val;
	}
	//MatrixValueFlyweight(Matrix<T, defval>* parent, unsigned row, unsigned col) :
	//	parent{ parent }, row{ row }, col{ col } {}

	T operator*() const {
		return value;
		/*auto it = parent->data.find(tuple{ row, col });
		return it != parent->data.end() ? it->second : defval;*/
	}
	MatrixValueFlyweight<T, defval>& operator=(const T& val) {
		/*parent->add(row, col, val);*/
		value = val;
		return *this;
	}
	MatrixValueFlyweight<T, defval>& operator=(MatrixValueFlyweight<T, defval>& right) {
		/*return this->operator=(*iter)*/;

		return operator=(right.value);
	}

	bool operator==(const T& right) {
		/*return parent->get(row, col) == right;*/
		return value == right.value;
	}

	//MatrixValueFlyweight<T, defval> operator T(T& val) {
	//	return MatrixValueFlyweight{ val };
	//}
	bool operator==(const MatrixValueFlyweight& right) {
		return this->value == right.value;
	}
};



template<typename Key, typename Flyweight>
class FlyweightFactory {
	std::function<tuple<Flyweight, bool>(Key)> fnDoNotExist;
protected:
	map<Key, Flyweight> data;
public:
	// TODO using references into function (Flyweight&, Key&)
	FlyweightFactory(std::function<tuple<Flyweight, bool>(Key)> fnDoNotExist) : fnDoNotExist{ fnDoNotExist } {}
	Flyweight get(Key& key, bool* pis_find = nullptr) {
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

template<typename Key, typename Flyweight>
class FlyweightFactoryEx: public FlyweightFactory<Key, Flyweight> {
private:
	size_t length;
	Flyweight* pdef_fly_val;
public:
	// TODO use references
	FlyweightFactoryEx(Flyweight& defval) :
		FlyweightFactory{ [&defval](Key key) {return tuple{defval, false}; } },
		pdef_fly_val{ &defval }
	{}
	void add(Key& key, Flyweight& elt) {
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
	}
};

template<typename T, int defval>
struct MatrixFlyweightFactory : public FlyweightFactoryEx<
	tuple<unsigned, unsigned>,
	MatrixValueFlyweight<T, defval>
> {
	using iterator = typename decltype(data)::iterator;
	MatrixFlyweightFactory() : FlyweightFactoryEx{ MatrixValueFlyweight<T, defval>{defval} }
	{}
	iterator begin() { return data.begin(); }
	iterator end() { return data.end(); }
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
	MatrixFlyweightFactory<T, defval> mtx_fly_factory;
	
public:
	using iterator = typename decltype(mtx_fly_factory)::iterator;
	//// Helper classes
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
				it = parent->mtx_fly_factory.begin();
				if (it == parent->mtx_fly_factory.end())
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
			return tuple{ key, val.value };
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
				if (it == parent->mtx_fly_factory.end()) {
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
	using ValueType = MatrixValueFlyweight<T, defval>;

	// For using matrix[x][y]
	template<typename T>
	class MatrixPartValue {
		unsigned row;
		Matrix<T, defval>* parent;
	public:
		MatrixPartValue(Matrix<T, defval>* parent, unsigned row) :
			parent{ parent }, row{ row } {}
		//MatrixValueFlyweight<T, defval> operator[](unsigned col) {
		//	return MatrixValueFlyweight<T, defval>{ parent, row, col, };
		//}
		MatrixValueFlyweight<T, defval> operator[](unsigned col) {
			return MatrixValueFlyweight<T, defval>{};
		}
	};
	//// end Helper classes

	constexpr static decltype(defval) def_value = defval;

	Matrix() {}

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
	//mtx_fly_factory
	ValueType get(unsigned row_idx, unsigned col_idx) {
		return mtx_fly_factory.get(tuple{ row_idx, col_idx });
	}
	void add(unsigned row_idx, unsigned col_idx, const T& elt) {
		mtx_fly_factory.add(tuple{ row_idx, col_idx }, ValueType( elt ));
	}
	/*T get(unsigned row_idx, unsigned col_idx) {
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
	}*/
};

