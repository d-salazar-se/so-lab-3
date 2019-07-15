#include "lab.h"

int main(int argc, char *argv[])
{
	// Obtención y validación de parametros
	param * params = getParams(argc, argv);

	if ( ! params) {
		fprintf(stderr, "%s\n", USAGE);
		exit(1);
	}
	file * fileUp = readFile(params->filenameUp);

	if ( ! fileUp) {
		exit(1);
	}

	file * fileDown = readFile(params->filenameDown);
	if ( ! fileDown) {
		exit(1);
	}

	file * fileOrdinary = readFile(params->filenameOrdinary);
	if ( ! fileOrdinary) {
		exit(1);
	}
	
	elevator ** elevatorsNearestCar = generateElevators(params->numberOfElevators);
	elevator ** elevatorsFSO = generateElevators(params->numberOfElevators);

	nearestCar(fileUp, elevatorsNearestCar, params->numberOfElevators, params->numberOfFloors);
	// float waitingTimeNearestCarDown = nearestCar(fileDown, elevatorsNearestCar, params->numberOfElevators, params->numberOfFloors);
	// float waitingTimeNearestCarOrdinary = nearestCar(fileOrdinary, elevatorsNearestCar, params->numberOfElevators, params->numberOfFloors);
	
	// float waitingTimeFSOUp = fso(fileUp, elevatorsFSO, params->numberOfElevators, params->numberOfFloors);
	// float waitingTimeFSODown = fso(fileDown, elevatorsFSO, params->numberOfElevators, params->numberOfFloors);
	// float waitingTimeFSOOrdinary = fso(fileOrdinary, elevatorsFSO, params->numberOfElevators, params->numberOfFloors);
	
	// // Generar archivo salida con resultados por hijo
	// if (params->showResultsFlag) {
	// 	printResultsConsole(elevatorsNearestCar, elevatorsNearestCar, params->numberOfElevators);
	// } else {
	// 	printResultsFile("Nearest Car.txt", waitingTimeNearestCarUp, waitingTimeNearestCarDown, waitingTimeNearestCarOrdinary, elevatorsNearestCar, params->numberOfElevators);
	// 	printResultsFile("FSO.txt", waitingTimeFSOUp, waitingTimeFSODown, waitingTimeFSOOrdinary, elevatorsFSO, params->numberOfElevators);
	// 	printf("[ ! ] Archivos 'Nearest Car.txt' y 'FSO.txt' generados!.\n");
	// }
	if(params->showResultsFlag){
		printResultsConsole(elevatorsNearestCar, elevatorsFSO, params->numberOfElevators);
	}
	else{
		printResultsFile("salida.txt", 10.0, 11.1, 12.3, elevatorsNearestCar, params->numberOfElevators);
	}
	// Finalmente liberar la memoria utilizada.
	freeParams(params);
	freeFile(fileUp);
	freeFile(fileDown);
	freeFile(fileOrdinary);
	freeElevators(elevatorsNearestCar, params->numberOfElevators);
	freeElevators(elevatorsFSO, params->numberOfElevators);

	return 0;
}