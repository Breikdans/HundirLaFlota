/*
 * Grid.h
 */

#ifndef GRID_H_
#define GRID_H_

const int MAX_ROWS_GRID	=		10;
const int MAX_COLS_GRID	=		10;

const int CELL_WIDTH	= 		4;

const int HORIZONTAL 	= 0;
const int VERTICAL		= 1;

#define SEA_BACKGROUND	1 << 0		// = 1. 32 bits de mascara.
#define PLAYER_CELLS 	1 << 1		// = 2. 32 bits de mascara.
#define CPU_CELLS 		1 << 2		// = 4. 32 bits de mascara.
#define SHIP_CELL		1 << 3		// = 8. 32 bits de mascara.

const std::string MATERIAL_CELL 			= "celda";
const std::string MATERIAL_CELL_SELECTED 	= "celda_light";

typedef unsigned int uint32;
typedef unsigned short int usint16;

typedef enum{
	 AGUA 		= 0					// 0
	,DISPARADO						// 1

	// el barco esta "intacto"
	,PROA_H							// 2
	,CUERPO1_H						// 3
	,CUERPO2_H						// 4
	,POPA_H							// 5

	,PROA_V							// 6
	,CUERPO1_V						// 7
	,CUERPO2_V						// 8
	,POPA_V							// 9

	// el barco esta "ardiendo"
	,PROA_H_T						// 10
	,CUERPO1_H_T					// 11
	,CUERPO2_H_T					// 12
	,POPA_H_T						// 13

	,PROA_V_T						// 14
	,CUERPO1_V_T					// 15
	,CUERPO2_V_T					// 16
	,POPA_V_T						// 17

	// el barco esta "quemado"
	,PROA_H_Q						// 18
	,CUERPO1_H_Q					// 19
	,CUERPO2_H_Q					// 20
	,POPA_H_Q						// 21

	,PROA_V_Q						// 22
	,CUERPO1_V_Q					// 23
	,CUERPO2_V_Q					// 24
	,POPA_V_Q						// 25
}EN_CELDAS;

typedef enum{
	 PLAYER
	,CPU
}EN_TURNO;

int rangeRandomNumber (int min, int max);		// retorna un numero aleatorio entre el minimo y maximo indicados

class Grid
{
	private:
		usint16 _CasillasVida;
		usint16 _tbl_Grid[MAX_ROWS_GRID][MAX_COLS_GRID];			// Asi se inicializa el array a 0 (AGUA)
		void ColocaBarco(usint16 casillas);
	public:
		Grid(usint16 C = 5+4+3+3+2);								// constructor
		usint16& operator() (usint16 F, usint16 C);					// para escribir en posicion del grid
		usint16 operator() (usint16 F, usint16 C) const;			// para leer de posicion del grid
		void IniciaBarcosAleatorio();								// inicia barcos en posiciones aleatorias
		bool ActualizaDisparo(usint16 F, usint16 C);				// recibe unas coordenadas de disparo y actualiza la tabla si ha sido tocado y comprueba si es hundido para actualizarlas
		usint16 getCasillasVida() const;
		void setCasillasVida(usint16 c);
		void clearGrid();
		void restaCasillas(void);
		void DebugGrid();
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
