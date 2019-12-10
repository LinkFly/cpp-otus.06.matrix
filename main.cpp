#include "share.h"

#include <iostream>
#include <cassert>
#include <tuple>

#include "matrix.h"

using std::cout;
using std::endl;

template<typename T, int defval>
std::ostream& operator<<(std::ostream& out, const MatrixIterator<T, defval>& iter) {
	out << std::get<1>(*iter);
	return out;
}

int main() {

	//Matrix<int, 0> mm;
	//MatrixIterator<int, 0> myit = mm(0, 0);
	//auto it = myit.find(34);
	///**myit;*/
	//cout << myit << endl;

	//Matrix<int, 0> mtx;
	//auto it = mtx(0, 0);
	//cout << (*it);
	//return 0;

	/*Trivial checking code*/
	{
		Matrix<int, -1> matrix; // бесконечная матрица int заполнена значениями -1
		assert(matrix.size() == 0); // все ячейки свободны
		auto a = matrix(0, 0);
		cout << a << "!!!" << endl;
		assert(a == -1);
		assert(matrix.size() == 0);
		matrix(100, 100) = 314;
		assert(matrix(100, 100) == 314);
		assert(matrix.size() == 1);
	}
	///////////////////

	/* При запуске программы необходимо создать матрицу с пустым значением 0 */
	Matrix<int, 0> m;

	const int N = 10;
	auto half_side = N / 2;
	auto defval = decltype(m)::defval;

	/* заполнить диагонали матрицы
	выражением m[i][i] = m[N - 1 - i][i] = i; // N = 10, i = [0, N) */
	for (int i = 0; i < N; i++) {
		/*int j = ;*/
		m[i][i] = m[N - 1 - i][i] = i;
		/*m[i][j] = m[j][j] = i;*/
	}

	/* Начиная с ячейки [0, 0] в шахматном порядке заполнить матрицу 10x10 значением по умолчанию.
	Заполнение в шахматном порядке заняло 25 итераций */
	/* (ниже альтернативное, оптимизированное решение) */
	//for (int i = 0; i < half_side; i++) {
	//	auto cur_row = i * 2;
	//	for (int j = 0; j < half_side; j++) {
	//		auto cur_col = j * 2;
	//		m[cur_row][cur_col] = defval;
	//		m[cur_row + 1][cur_col + 1] = defval;
	//	}
	//}

	/* !!! Альтернативное решение, так как мы по факту "стираем" только одну диагоняль (записываем значения по-умолчанию),
	то мы вполне можем просто стереть эту диагоняль, т.е. решить задачу за 10 итераций!!! */
	for (int i = 0; i < N; i++) {
		int j = N - 1 - i;
		m[j][j] = defval;
	}


	/* Необходимо вывести фрагмент матрицы от [1,1] до [8,8]. Между столбцами пробел. Каждая строка матрицы
	на новой строке. */
	cout << "Matrix [1,1]:[8,8]\n";
	for (int i = 1; i <= 8; i++) {
		cout << m[i][1];
		for (int j = 2; j <= 8; j++) {
			cout << " " << m[i][j];
		}
		cout << endl;
	}

	/* Вывести количество занятых ячеек. */
	cout << endl << "Matrix size: " << m.size() << endl;

	/* Вывести все занятые ячейки вместе со своими позициями. */
	cout << endl << "Matrix cells: \n";
	for (auto elt_spec : m) {
		auto [x, y] = std::get<0>(elt_spec);
		auto elt = std::get<1>(elt_spec);
		cout << x << ":" << y << " - " << elt << endl;
	}

	return 0;
}
