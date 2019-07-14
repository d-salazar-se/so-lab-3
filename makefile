all:
	gcc ./src/main.c ./src/lab.c ./src/call.c -o lab3 -Wall

clean:
	-rm -f lab3

run: 
	./lab3 -u subida.csv -d bajada.csv -o random.csv -n 30 -e 2
runB:
	./lab3 -u subida.csv -d bajada.csv -o random.csv -n 30 -e 2 -b