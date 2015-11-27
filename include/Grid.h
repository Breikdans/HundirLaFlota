/*
 * Grid.h
 */

#ifndef GRID_H_
#define GRID_H_

const int MAX_ROWS_GRID	=		10;
const int MAX_COLS_GRID	=		10;

const int CELL_WIDTH	= 		4;

typedef unsigned short int usint16;

typedef enum{
	 AGUA 		= 0
//	,ROCA
	,DISPARADO

	// el barco esta "intacto"
	,PROA
	,CUERPO
	,POPA

	// el barco esta "ardiendo"
	,PROA_T
	,CUERPO_T
	,POPA_T

	// el barco esta "quemado"
	,PROA_H
	,CUERPO_H
	,POPA_H
}EN_CELDAS;

class Grid
{
	private:
		usint16 _CasillasVivas;
		usint16 _tbl_Grid[MAX_ROWS_GRID][MAX_COLS_GRID];			// Asi se inicializa el array a 0 (AGUA)
	public:
		Grid(usint16 C = 5+4+3+3+2);								// constructor
		usint16& operator() (usint16 F, usint16 C);					// para escribir en posicion del grid
		usint16 operator() (usint16 F, usint16 C) const;			// para leer de posicion del grid
		void IniciaBarcosAleatorio();								// inicia barcos en posiciones aleatorias
		bool ActualizaDisparo(usint16 F, usint16 C);				// recibe unas coordenadas de disparo y actualiza la tabla si ha sido tocado y comprueba si es hundido para actualizarlas
};
//   1 2 3 4 5 6 7 8 9 10
//1 |-|-|-|-|-|-|-|-|-|-|
//2 |-|-|-|-|-|-|-|-|-|-|
//3 |-|-|-|-|-|-|-|-|-|-|
//4 |-|-|t|t|t|t|t|-|-|-|
//5 |-|-|-|-|-|-|-|-|-|-|
//6 |-|-|-|-|-|-|-|-|-|-|
//7 |-|-|-|-|-|-|-|-|-|-|
//8 |-|-|-|-|-|-|-|-|-|-|
//9 |-|-|-|-|-|-|-|-|-|-|
//10|-|-|-|-|-|-|-|-|-|-|

#endif /* GRID_H_ */
