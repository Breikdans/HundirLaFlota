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
	,DISPARADO

	// el barco esta "intacto"
	,PROA_H
	,CUERPO1_H
	,CUERPO2_H
	,POPA_H

	,PROA_V
	,CUERPO1_V
	,CUERPO2_V
	,POPA_V

	// el barco esta "ardiendo"
	,PROA_H_T
	,CUERPO1_H_T
	,CUERPO2_H_T
	,POPA_H_T

	,PROA_V_T
	,CUERPO1_V_T
	,CUERPO2_V_T
	,POPA_V_T

	// el barco esta "quemado"
	,PROA_H_Q
	,CUERPO1_H_Q
	,CUERPO2_H_Q
	,POPA_H_Q

	,PROA_V_Q
	,CUERPO1_V_Q
	,CUERPO2_V_Q
	,POPA_V_Q
}EN_CELDAS;

class Grid
{
	private:
		usint16 _CasillasVivas;
		usint16 _tbl_Grid[MAX_ROWS_GRID][MAX_COLS_GRID];			// Asi se inicializa el array a 0 (AGUA)
		int rangeRandomNumber (int min, int max) const;
		void ColocaBarco(usint16 casillas);
		void DebugGrid();
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
