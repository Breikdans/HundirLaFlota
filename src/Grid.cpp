/*
 * Grid.cpp
 */
#include <stdexcept>
#include "Grid.h"
#include "stdio.h"

Grid::Grid(usint16 C) : _CasillasVivas(C)
{
	for (int i = 0; i < MAX_ROWS_GRID ; i++ )
	{
		for (int j = 0; j < MAX_COLS_GRID ; j++ )
			_tbl_Grid [i][j] = AGUA;
	}
}

/* Para acceder a una posicion y ASIGNAR un valor */
usint16& Grid::operator() (usint16 F, usint16 C)
{
	//	LANZAMOS EXCEPCION
	if (F == 0 || C == 0 || F >= MAX_ROWS_GRID || C >= MAX_COLS_GRID)
		throw std::out_of_range("Indice fuera de limites");

	return _tbl_Grid[F][C];
}

/* para acceder a una posicion y DEVOLVER su valor */
usint16 Grid::operator() (usint16 F, usint16 C) const
{
	//	LANZAMOS EXCEPCION
	if (F == 0 || C == 0 || F >= MAX_ROWS_GRID || C >= MAX_COLS_GRID)
		throw std::out_of_range("Indice fuera de limites");

	return _tbl_Grid[F][C];
}
