#include "share.h"

#include <iostream>
#include <cassert>
#include <tuple>

//tmp
#include <map>
using std::map;

#include "matrix.h"

using std::cout;
using std::endl;

int main() {
	/* (trivial checking code see in tests) */

	/* При запуске программы необходимо создать матрицу с пустым значением 0 */
	Matrix<int, 0> m;

	const int N = 10;
	auto half_side = N / 2;
	auto def_value = decltype(m)::def_value;

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
	//		m[cur_row][cur_col] = def_value;
	//		m[cur_row + 1][cur_col + 1] = def_value;
	//	}
	//}

	/* !!! Альтернативное решение, так как мы по факту "стираем" только одну диагоняль (записываем значения по-умолчанию),
	то мы вполне можем просто стереть эту диагоняль, т.е. решить задачу за 10 итераций!!! */
	for (int i = 0; i < N; i++) {
		int j = N - 1 - i;
		m[j][j] = def_value;
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
