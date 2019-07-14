#include "lab.h"

param * getParams(int argc, char * argv[]) {
	param * params = (param*)calloc(1, sizeof(param));

	params->numberOfFloors = 1;
	params->numberOfElevators = 1;
	params->showResultsFlag = 0;

	int option;
	opterr = 0;
	while((option = getopt(argc, argv, "u:d:o:n:e:b")) != -1) {
		switch (option) {
			case 'u':
				params->filenameUp = (char*)calloc(1, sizeof(char) * (strlen(optarg) + 1));
				strcpy(params->filenameUp, optarg);
				params->filenameUp[strlen(params->filenameUp)] = '\0';
				break;
			case 'd':
				params->filenameDown = (char*)calloc(1, sizeof(char) * (strlen(optarg) + 1));
				strcpy(params->filenameDown, optarg);
				params->filenameDown[strlen(params->filenameDown)] = '\0';
				break;
			case 'o':
				params->filenameOrdinary = (char*)calloc(1, sizeof(char) * (strlen(optarg) + 1));
				strcpy(params->filenameOrdinary, optarg);
				params->filenameOrdinary[strlen(params->filenameOrdinary)] = '\0';
				break;
			case 'n':
				params->numberOfFloors = atoi(optarg);
				break;
			case 'e':
				params->numberOfElevators = atoi(optarg);
				break;
			case 'b':
				params->showResultsFlag = 1;
				break;
			default:
				abort();
		}
	}

	// revisar parametros obligatorios
	if ( ! params->filenameUp
		|| ! params->filenameDown
		|| ! params->filenameOrdinary
		|| params->numberOfFloors <= 0
		|| params->numberOfElevators <= 0) {
		return (param*)NULL;
	}

	return params;
}

file * readFile(const char * filename) {
	
	FILE * fp = fopen(filename, "r");

	if ( ! fp) {
		perror(filename);
		return (file*)NULL;
	}

	file * f = (file*)calloc(1, sizeof(file));
	f->lines = (fileLine**)calloc(1, sizeof(fileLine*));
	fileLine * line = (fileLine*)calloc(1, sizeof(fileLine));

	while ( ! feof(fp) && fscanf(fp, "%d,%d,%d,%c,%d\n", &(line->time), &(line->originFloor), &(line->destinationFloor), &(line->direction), &(line->numberOfPassengers))) {
		f->lines = (fileLine**)realloc(f->lines, sizeof(fileLine*) * (f->numberOfLines + 1));
		f->lines[f->numberOfLines] = (fileLine*)calloc(1, sizeof(fileLine));
		
		f->lines[f->numberOfLines]->time = line->time;
		f->lines[f->numberOfLines]->originFloor = line->originFloor;
		f->lines[f->numberOfLines]->destinationFloor = line->destinationFloor;
		f->lines[f->numberOfLines]->direction = line->direction;
		f->lines[f->numberOfLines]->numberOfPassengers = line->numberOfPassengers;

		f->numberOfLines++;
	}

	fclose(fp);

	return f;
}

elevator ** generateElevators(int numberOfElevators) {
	elevator ** elevators = (elevator**)calloc(numberOfElevators, sizeof(elevator*));

	int i;
	for (i = 0; i < numberOfElevators; ++i) {
		elevators[i] = (elevator*)calloc(1, sizeof(elevator));
		
		elevators[i]->totalFloors = 0;
		elevators[i]->totalTime = 0;

		elevators[i]->passengers = (int*)calloc(1, sizeof(int));
		// elevators[i]->calls = (fileLine**)calloc(1, sizeof(fileLine*));
		elevators[i]->calls = createList();
		elevators[i]->pendingCalls = 0;

		elevators[i]->currentNumberOfPassengers = 0;
		elevators[i]->currentFloor = 1;
		elevators[i]->destinationFloor = 1;
		elevators[i]->currentDirection = DESTINATION_INACTIVE;
	}

	return elevators;
}

int boardPassengers(elevator * e, int numberOfPassengers, int destination) {
	if (e->currentNumberOfPassengers + numberOfPassengers > MAX_CAPACITY) {
		return 0;
	}

	e->passengers = (int*)realloc(e->passengers, sizeof(int) * (e->currentNumberOfPassengers + numberOfPassengers));
	for (int i = 0; i < numberOfPassengers; ++i) {
		e->passengers[e->currentNumberOfPassengers + i] = destination;
	}
	
	e->currentNumberOfPassengers += numberOfPassengers;
	e->totalTime += TIME_OPEN_DOORS + TIME_BOARD_PASSENGER * numberOfPassengers;
	
	return numberOfPassengers;
}

int leavePassengers(elevator * e, int currentFloor) {
	int currentPassenger = 0;
	int numberOfPassengers = 0;	

	while	(currentPassenger < e->currentNumberOfPassengers) {
		if (e->passengers[currentPassenger] == currentFloor) {
			for (int i = currentPassenger; i < e->currentNumberOfPassengers; ++i) {
				e->passengers[i] = e->passengers[i + 1];
			}
			numberOfPassengers++;
			e->currentNumberOfPassengers--;
		} else {
			currentPassenger++;
		}
	}

	if (numberOfPassengers > 0) {
		e->passengers = (int*)realloc(e->passengers, sizeof(int) * e->currentNumberOfPassengers);
		e->totalTime += TIME_OPEN_DOORS + TIME_BOARD_PASSENGER * numberOfPassengers;
	}

	return numberOfPassengers;
}

void addStop(elevator * e, fileLine * call) {
	// e->calls = (fileLine**)realloc(e->calls, sizeof(fileLine*) * (e->pendingCalls + 1));
	// e->calls[e->pendingCalls] = call;
	addCall(e->calls, call);
	e->pendingCalls++;

	if (e->currentDirection == DESTINATION_INACTIVE) {
		e->currentDirection = call->direction;
		e->destinationFloor = call->destinationFloor;
	} else if (e->currentDirection == 'U' && e->destinationFloor < call->destinationFloor) {
		e->destinationFloor = call->destinationFloor;
	} else if (e->currentDirection == 'D' && e->destinationFloor > call->destinationFloor) {
		e->destinationFloor = call->destinationFloor;
	}
}

void arriveOnStop(elevator * e) {
	node * callNode = e->calls->head;
	while	(callNode) {
		/* calls to board on this stop */
		if (callNode->call->originFloor == e->currentFloor) {
			boardPassengers(e, callNode->call->numberOfPassengers, callNode->call->destinationFloor);
			// printf("\t[Floor:%2d] BOARDING: %d\n", e->currentFloor, x);
		}
		/* calls to leave on this stop */
		if (callNode->call->destinationFloor == e->currentFloor) {
			leavePassengers(e, e->currentFloor);
		}
		callNode = callNode->next;
	}

	e->pendingCalls = removeCallsByFloor(e->calls, e->currentFloor);
	if (e->pendingCalls == 0) {
		e->currentDirection = DESTINATION_INACTIVE;
	}
}

void moveElevators(elevator ** elevators, int numberOfElevators) {
	int i;
	for (i = 0; i < numberOfElevators; ++i) {
		if (elevators[i]->currentFloor != elevators[i]->destinationFloor) {
			elevators[i]->totalFloors += 1;
			elevators[i]->totalTime += TIME_PER_FLOOR;

			if (elevators[i]->currentDirection == 'U') {
				elevators[i]->currentFloor++;
			} else if (elevators[i]->currentDirection == 'D') {
				elevators[i]->currentFloor--;
			}
		}
		else {
			arriveOnStop(elevators[i]);
		}
	}
}

int figureOfSuitability(elevator ** elevators, int numberOfElevators, int numberOfFloors, fileLine * call) {
	int i;
	int figureOfSuitability = 0, currentFigureOfSuitability = 0;
	int distance;
	int elevatorIndex = 0;
	for (i = 0; i < numberOfElevators; ++i) {
		distance = elevators[i]->currentFloor - call->destinationFloor;

		// Ascensor se mueve hacia el piso de la llamada y direccion de llamada es igual 
		// ej: va bajando y la llamada pide bajar desde un piso inferior
		if (((elevators[i]->currentDirection == 'D' && elevators[i]->currentFloor > call->originFloor)
				|| (elevators[i]->currentDirection == 'U' && elevators[i]->currentFloor < call->originFloor))
			&& elevators[i]->currentDirection == call->direction) {
			currentFigureOfSuitability = numberOfFloors + 2 - distance;
		}
		// Ascensor se mueve hacia el piso de la llamada, pero la direccion de la llamada es diferente
		// ej: va bajando, pasa por el piso de la llamada, pero la llamada quiere subir
		else if (((elevators[i]->currentDirection == 'D' && elevators[i]->currentFloor > call->originFloor)
							|| (elevators[i]->currentDirection == 'U' && elevators[i]->currentFloor < call->originFloor))
						&& elevators[i]->currentDirection != call->direction) {
			currentFigureOfSuitability = numberOfFloors + 1 - distance;
		}
		// Ascensor se mueve en direccion contraria al piso solicitado
		else if (elevators[i]->currentDirection != call->direction) {
			currentFigureOfSuitability = 1;
		}
		// Ascensor inactivo
		else if (elevators[i]->currentDirection == DESTINATION_INACTIVE) {
			currentFigureOfSuitability = numberOfFloors + 1 - distance;
		}

		if (currentFigureOfSuitability > figureOfSuitability) {
			figureOfSuitability = currentFigureOfSuitability;
			elevatorIndex = i;
		}
	}

	return elevatorIndex;
}

float nearestCar(file * f, elevator ** elevators, int numberOfElevators, int numberOfFloors) {
	float averageWaitingTime = 0.0;
	int elevatorIndex;

	int tick = 0;
	int currentLine = 0;

	while (tick <= f->lines[f->numberOfLines - 1]->time) {
		// printf("[TICK: %d]\n", tick);
		if (tick == f->lines[currentLine]->time) {
			/* Obtener indice de elevador mas cercano */
			elevatorIndex = figureOfSuitability(elevators, numberOfElevators, numberOfFloors, f->lines[currentLine]);
			/* Agregar parada al elevador */
			addStop(elevators[elevatorIndex], f->lines[currentLine]);
			/* Mover el cursor a la siguiente linea. */
			currentLine++;
		}
		
		moveElevators(elevators, numberOfElevators);

		tick++;
	}
	
	return averageWaitingTime;
}

float fso(file * f, elevator ** elevators, int numberOfElevators, int numberOfFloors) {
	float averageWaitingTime = 0.0;

	return averageWaitingTime;
}

void printResultsFile(char * filename, float up, float down, float ordinary, elevator ** elevators, int numberOfElevators) {
	FILE * fp = fopen(filename, "w");

	if ( ! fp ){
		perror(filename);
		return;
	}

	float average = 0.0;

	fprintf(fp, "Tiempo promedio espera Subida: %.2f\n", up);
	fprintf(fp, "Tiempo promedio espera Bajada: %.2f\n", down);
	fprintf(fp, "Tiempo promedio espera Ordinario: %.2f\n", ordinary);
	fprintf(fp, "Tiempo promedio espera General: %.2f\n", average);

	int i;
	for (i = 0; i < numberOfElevators; ++i) {
		fprintf(fp, "Tiempo total ascensor %d: %d s.\n", i, elevators[i]->totalTime);
	}
	for (i = 0; i < numberOfElevators; ++i) {
		fprintf(fp, "Tiempo total ascensor %d: %d\n", i, elevators[i]->totalFloors);
	}

	fclose(fp);
}

void printResultsConsole(elevator ** elevatorsNearestCar, elevator ** elevatorsFSO, int numberOfElevators) {
	int i;

	// NC
	for (i = 0; i < numberOfElevators; ++i) {
		printf("Tiempo total ascensor %d NC: %d s.\n", i, elevatorsNearestCar[i]->totalTime);
	}

	for (i = 0; i < numberOfElevators; ++i) {
		printf("Total de pisos recorridos ascensor %d NC: %d.\n", i, elevatorsNearestCar[i]->totalFloors);
	}
	// FSO
	for (i = 0; i < numberOfElevators; ++i) {
		printf("Tiempo total ascensor %d FSO: %d s.\n", i, elevatorsFSO[i]->totalTime);
	}

	for (i = 0; i < numberOfElevators; ++i) {
		printf("Total de pisos recorridos ascensor %d FSO: %d.\n", i, elevatorsFSO[i]->totalFloors);
	}
}

void freeFile(file * f) {
	int i;
	for (i = 0; i < f->numberOfLines; ++i) {
		free(f->lines[i]);
	}
	free(f->lines);
}

void freeParams(param * p) {
	free(p->filenameUp);
	free(p->filenameDown);
	free(p->filenameOrdinary);
	free(p);
}

void freeElevators(elevator ** elevators, int numberOfElevators) {
	int i;

	for (i = 0; i < numberOfElevators; ++i) {
		free(elevators[i]);
	}

	free(elevators);
}

// ------------------------------
// TEMP
// ------------------------------
void printFile(file * f) {
	for (int i = 0; i < f->numberOfLines; ++i) {
		printf("%d,%d,%d,%c,%d\n", f->lines[i]->time, f->lines[i]->originFloor, f->lines[i]->destinationFloor, f->lines[i]->direction, f->lines[i]->numberOfPassengers);
	}
}

void printElevator(elevator * e) {
	printf("\n----------------------------------------------------\n");
	printf("totalFloors: %d\n", e->totalFloors);
	printf("totalTime: %d\n", e->totalTime);
	printf("calls: %2d\n", e->pendingCalls);
	printList(e->calls);
	printf("currentNumberOfPassengers: %d\n", e->currentNumberOfPassengers);
	for (int i = 0; i < e->currentNumberOfPassengers; ++i) {
		printf("%2d->", e->passengers[i]);
	}
	printf("\n");
	printf("currentFloor: %d\n", e->currentFloor);
	printf("destinationFloor: %d\n", e->destinationFloor);
	printf("currentDirection: %c\n", e->currentDirection);
}

void printElevators(elevator ** elevators, int numberOfElevators) {
	int i;
	for (i = 0; i < numberOfElevators; ++i) {
		printElevator(elevators[i]);
	}
}
