#pragma once
#ifndef LAB_H
#define LAB_H

// #include "call.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

//===================================================+
// DEFINICION DE CONSTANTES
//===================================================+

/**
 * Modo de uso del programa, especificando parametros obligatorios y opcionales
 */
#define USAGE "USAGE:\n\
./lab1.o -i <filename> -o <filename> -n <amount of disks> -d <disk width> [-b]\n\
\t-u\tNombre de archivo de subidas.\n\
\t-d\tNombre de archivo de bajadas.\n\
\t-o\tNombre de archivo de randoms.\n\
\t-n\tCantidad de pisos.\n\
\t-e\tCantidad de elevadores.\n\
OPTIONS:\n\
\t-b\tMuestra los realizados por archivo."

/**
 * Maximo tamano de buffer para transmision de
 * datos en pipes
 */
#define MAX_STR_BUFF 256


/**
 *
 */
#define TIME_PER_FLOOR 1
#define TIME_BOARD_PASSENGER 1
#define TIME_OPEN_DOORS 1
#define MAX_CAPACITY 20
#define DESTINATION_INACTIVE '-'

//===================================================+
// DEFINICION DE TIPOS DE DATOS
//===================================================+

/**
 * Representa parametros de entrada del programa
 */
typedef struct {
	char * filenameUp;					//< -u: Nombre del archivo subidas
	char * filenameDown;				//< -d: Nombre del archivo bajadas
	char * filenameOrdinary;		//< -o: Nombre del archivo randoms
	int numberOfFloors;					//< -n: Numero de pisos
	int numberOfElevators;			//< -e: Numero de elevadores
	int showResultsFlag;				//< -b: Mostrar resultados
} param;

/**
 * Representa 1 registro de visibilidad,
 * es decir, una linea del archivo de entrada.
 */
typedef struct {
	int time;
	int originFloor;
	int destinationFloor;
	char direction;
	int numberOfPassengers;
} fileLine;

/**
 * Representa el archivo completo de entrada.
 */
typedef struct {
	int numberOfLines;
	fileLine ** lines;
} file;

typedef struct _node {
  fileLine * call;
  struct _node * next;
} node;

typedef struct {
  node * head;
} list;

/**
 * Calls
 */
typedef struct {
	int totalFloors;	// pisos recorridos
	int totalTime;

	// current status
	list * calls;	// call stops
	int pendingCalls;
	int * passengers;	// passengers destinations
	
	int currentNumberOfPassengers;
	int currentFloor;
	int destinationFloor;
	char currentDirection;		//< ['U'|'D']
} elevator;

list * createList();
void addCall(list * l, fileLine * call);
int removeCallsByFloor(list * l, int floorNumber);
//===================================================+
// DEFINICION DE FIRMAS DE FUNCIONES
//===================================================+

/**
 * Obtiene los parametros desde argc y argv de la funcion main().
 *
 * @param 	int 		argc  	Número de variables en array argv.
 * @param 	char** 		argv  	Parametros como arreglo de strings.
 * @return 	param*				Estructura con los parametros asignados.
 */
param * getParams(int argc, char * argv[]);

/**
 * Lee el archivo de visibilidades y los almacena en un array.
 *
 * @param 	const char* 		filename 	Nombre del archivo con visibilidades.
 * @return 	file*						Estructura con los registros del archivo.
 */
file * readFile(const char * filename);

elevator ** generateElevators(int numberOfElevators);

int boardPassengers(elevator * e, int numberOfPassengers, int destination);

int leavePassengers(elevator * e, int currentFloor);

float nearestCar(file * f, elevator ** elevators, int numberOfElevators, int numberOfFloors);
float fso(file * f, elevator ** elevators, int numberOfElevators, int numberOfFloors);

void moveElevators(elevator ** elevators, int numberOfElevators);

/**
 * Escribe los resultados en el archivo de salida.
 *
 * @param 	char* 			filename 				Nombre del archivo de salida.
 * @return 	void
 */
void printResultsFile(char * filename, float up, float down, float ordinary, elevator ** elevators, int numberOfElevators);

void printResultsConsole(elevator ** elevatorsNearestCar, elevator ** elevatorsFSO, int numberOfElevators);

/**
 * Libera la memoria utilizada por las estructuras.
 *
 * @param 	param* 				p 				Parametros de entrada del programa
 * @param 	visibilityArray* 	v 				Arreglo de visibilidades, representa el archivo de entrada..
 * @param 	disk** 				d 				Arreglo con informacion de los procesos hijos/discos.
 * @return 	void
 */
void freeParams(param * p);

void freeFile(file * f);

void freeElevators(elevator ** elevators, int numberOfElevators);
// ------------------------------
// TEMP
// ------------------------------
void printFile(file * f);
void printElevator(elevator * e);
void printElevators(elevator ** elevators, int numberOfElevators);
void printList(list * l);
#endif