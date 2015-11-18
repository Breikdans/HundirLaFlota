/*
 * Grid.cpp
 */


/* Para acceder a una posicion y ASIGNAR un valor */
usint16& Grid::operator() (usint16 F, usint16 C)
{
	//	LANZAMOS EXCEPCION
	if (F == 0 || C == 0 || F > MAX_ROWS_GRID || C > MAX_COLS_GRID)
		throw std::out_of_range("Indice fuera de limites");

	return _tbl_Grid[MAX_COLS_GRID*F + C];
}

/* para acceder a una posicion y DEVOLVER su valor */
usint16 CMatriz::operator() (usint16 F, usint16 C) const
{
	//	LANZAMOS EXCEPCION
	if (F == 0 || C == 0 || F > MAX_ROWS_GRID || C > MAX_COLS_GRID)
		throw std::out_of_range("Indice fuera de limites");

	return _tbl_Grid[MAX_COLS_GRID*F + C];
}
