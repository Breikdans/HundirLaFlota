/*
 * Grid.cpp
 */
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <String.h>
#include "Grid.h"

int rangeRandomNumber (int min, int max)
{
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do
    {
        x = rand();
    }while (x >= RAND_MAX - remainder);
    return min + x % n;
}

// ========================================================================
// =========================== METODOS PUBLICOS ===========================
// ========================================================================
Grid::Grid(usint16 C) : _CasillasVida(C)
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
	if (F >= MAX_ROWS_GRID || C >= MAX_COLS_GRID)
		throw std::out_of_range("Indice fuera de limites");

	return _tbl_Grid[F][C];
}

/* para acceder a una posicion y DEVOLVER su valor */
usint16 Grid::operator() (usint16 F, usint16 C) const
{
	//	LANZAMOS EXCEPCION
	if (F >= MAX_ROWS_GRID || C >= MAX_COLS_GRID)
		throw std::out_of_range("Indice fuera de limites");

	return _tbl_Grid[F][C];
}

void Grid::IniciaBarcosAleatorio()
{
	usint16 barcos[] = {5, 4, 3, 3, 2};

	int max_barcos = sizeof(barcos) / sizeof(barcos[0]);
	for(int i = 0; i < max_barcos; i++)
	{
		ColocaBarco(barcos[i]);
	}
#ifdef _DEBUG
DebugGrid();
#endif
}

usint16 Grid::getCasillasVida() const
{
	return _CasillasVida;
}

void Grid::setCasillasVida(usint16 c)
{
	_CasillasVida = c;
}

void Grid::restaCasillas(void)
{
	_CasillasVida--;
}

// ========================================================================
// =========================== METODOS PRIVADOS ===========================
// ========================================================================
void Grid::ColocaBarco(usint16 casillas)
{
	// por claridad...
	const int MAX_X			= MAX_COLS_GRID - 1;
	const int MAX_Y			= MAX_ROWS_GRID - 1;

	bool sw_OK = true;		// indica si ya ha sido colocado correctamente
	bool sw_orientacion = static_cast<bool>(rangeRandomNumber(HORIZONTAL, VERTICAL));	// Orientacion vertical u horizontal?

// --- acotamos rango maximo para la casilla inicial...
	// Ej: si el barco es de 5 y la orientacion es horizontal, la casilla inicial no podra ser superior a la 5 porque no cabe
	int MaxPosXInicial = 0;
	int MaxPosYInicial = 0;

	int PosXInicial = 0;
	int PosYInicial = 0;

	if (sw_orientacion == HORIZONTAL)
	{
		MaxPosXInicial = (MAX_X - casillas) + 1;
		MaxPosYInicial = MAX_Y;
	}
	else if(sw_orientacion == VERTICAL)
	{
		MaxPosXInicial = MAX_X;
		MaxPosYInicial = (MAX_Y - casillas) + 1;
	}

	do{
		sw_OK = true;

		// Cogemos coordenadas aleatorias de casilla
		PosXInicial = rangeRandomNumber(0, MaxPosXInicial);
		PosYInicial = rangeRandomNumber(0, MaxPosYInicial);

// --- Calculamos el rango de casillas a validar...
		// hay que validar si esta vacia, desde una casilla antes de la inicial... a una casilla despues de la final... en X y en Y
		int IniXValidar = 0, FinXValidar = 0;
		int IniYValidar = 0, FinYValidar = 0;

		// calculamos casilla de inicio validacion...
		if(PosXInicial > 0)	IniXValidar = PosXInicial - 1;
		if(PosYInicial > 0)	IniYValidar = PosYInicial - 1;

		// calculamos casilla fin validacion...
		if(sw_orientacion == HORIZONTAL)
		{
			if(PosXInicial + (casillas - 1) + 1 > MAX_X)
				FinXValidar = PosXInicial + (casillas - 1);
			else
				FinXValidar = PosXInicial + (casillas - 1) + 1;

			if (PosYInicial < MAX_Y)
				FinYValidar = PosYInicial + 1;
			else
				FinYValidar = PosYInicial;
		}
		else if(sw_orientacion == VERTICAL)
		{
			if(PosYInicial + (casillas - 1) + 1 > MAX_Y)
				FinYValidar = PosYInicial + (casillas - 1);
			else
				FinYValidar = PosYInicial + (casillas - 1) + 1;

			if (PosXInicial < MAX_X)
				FinXValidar = PosXInicial + 1;
			else
				FinXValidar = PosXInicial;
		}

// --- Validacion de casillas vacias
		for (int x = IniXValidar; x <= FinXValidar && sw_OK == true; x++)
		{
			for (int y = IniYValidar; y <= FinYValidar && sw_OK == true; y++)
			{
				if (_tbl_Grid[x][y] != AGUA)
					sw_OK = false;
			}
		}
	}while(sw_OK == false);

// --- colocamos el barco en la rejilla

	if (sw_orientacion == HORIZONTAL)
	{
		for (int i = 0; i < casillas; i++)
		{
			if(i == 0)
				_tbl_Grid[PosXInicial+i][PosYInicial] = PROA_H;
			else if(i == casillas-1)
				_tbl_Grid[PosXInicial+i][PosYInicial] = POPA_H;
			else
				_tbl_Grid[PosXInicial+i][PosYInicial] = static_cast<usint16>(rangeRandomNumber(CUERPO1_H,CUERPO2_H));
		}
	}
	else if(sw_orientacion == VERTICAL)
	{
		for (int i = 0; i < casillas; i++)
		{
			if(i == 0)
				_tbl_Grid[PosXInicial][PosYInicial+i] = PROA_V;
			else if(i == casillas-1)
				_tbl_Grid[PosXInicial][PosYInicial+i] = POPA_V;
			else
				_tbl_Grid[PosXInicial][PosYInicial+i] = static_cast<usint16>(rangeRandomNumber(CUERPO1_V,CUERPO2_V));
		}
	}
}

void Grid::clearGrid()
{
	memset(_tbl_Grid, 0x00, sizeof(usint16) * (MAX_ROWS_GRID*MAX_COLS_GRID));
}

void Grid::DebugGrid()
{
	std::cout << std::endl;
	for(int y = 0; y < MAX_ROWS_GRID; y++)
	{
		for(int x = 0; x < MAX_COLS_GRID; x++)
		{
			std::cout << " " << _tbl_Grid[x][y];
		}
		std::cout << std::endl;
	}
}

// ============================================================================
// =========================== FIN METODOS PUBLICOS ===========================
// ============================================================================
