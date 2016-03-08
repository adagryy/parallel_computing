#include <stdio.h>
#include <stdlib.h>
#define BUFFOR_SIZE 80

double sum(double* vector, int n) {
	int i;
	double sum = 0.0f;
	for(i=0; i<n; i++) {
		sum += vector[i];
	}
	return sum;
}

int main(int argc, char **argv) { 
	FILE* f = fopen("vector.dat", "r");
    char buffor[BUFFOR_SIZE+1];
	double* vector;
	int n;
	int i;
	fgets(buffor, BUFFOR_SIZE, f);
 	n = atoi(buffor);
	vector = malloc(sizeof(double) * n);
	printf("Vector has %d elements\n", n);
	for(i=0; i<n; i++) {
		fgets(buffor, BUFFOR_SIZE, f);
		vector[i] = atof(buffor);
		printf("%s\n", buffor);
	}
	fclose(f);
	printf("v = [ ");
	for(i=0; i<n; i++) {
		printf("%f ", vector[i]);
	}
	printf("]\n");

	//printf("Suma elementów w wektorze = %f\n", sum(vector, n));
	return 0;
}



// Należy napisać program służacy do sumowania elementów w wektorze zapisanym w pliku.
// Sumowanie ma przebiegać współbieżnie i ma być wykonywane przez N procesów potomnych.
// Procesy potomne korzystają z dwóch przestrzeni pamięci współdzielonej zawierających odpowiednio:
// zakresy indeksów, które dany proces ma sumować i tablicę z wynikami cząstkowymi.

// range[i] -- range[i+1] -> zakres indeksów, które ma sumować proces-dziecko numer i,
// result[i] -> miejsce na wynik sumowania procesu-dziecka numer i.
// Proces główny tworzy procesy potomne, wczytuje plik z wektorem, inicjalizuje dane w pamięci, 
// a następnie, gdy dane są gotowe, wysyła wszystkim dzieciom sygnał SIGUSR1, informując je przy tym, 
// że dane są już dostępne. Proszę obejrzeć załączony plik add.c pokazujący prawidłowy sposób wczytywania wektorów.