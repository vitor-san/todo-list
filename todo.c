#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//macros for colors
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_WHITE   "\x1b[37m"
#define COLOR_BRRED   "\x1b[91m"
#define COLOR_BRWHITE "\x1b[97m"
#define COLOR_RESET   "\x1b[0m"

#define FILENAME "todo.txt"

int quantity; //quantity of todos

/** Struct of a todo */
typedef struct _t {
	char msg[256];
	int label;
} TODO;

/** Converts a string to lower case */
char* strlwr(char* str) {
	int len = strlen(str);
	for (int i = 0; i < len; i++)
		str[i] += (str[i] >= 65 && str[i] <= 90) ? 32 : 0;
	return str;
}

/** Returns an integer from a string */
int toint(char* str) {
	int a;
	return sscanf(str, "%d", &a) == EOF ? -1 : a;
}

/** Print a todo according to its label */
void printTODO(char* str, int label, int pos) {
	if (pos == -1) printf("> ");
	else printf("[%03d] ", pos);

	switch(label) {
		case 1: printf(COLOR_GREEN);
				break;
		case 2: printf(COLOR_BLUE);
				break;
		case 3: printf(COLOR_BRWHITE);
				break;
		default: printf(COLOR_YELLOW);
	}

	printf("%s\n" COLOR_RESET, str);
}

/** List all the todos from the list */
TODO* listTODO(bool usePosition) {
	FILE* todo = fopen(FILENAME, "r+");
	if (todo == NULL) {
		todo = fopen(FILENAME, "w+");
		printf(COLOR_RED "-- Nothing to do --\n" COLOR_RESET);
		fclose(todo);
		return NULL;
	}

	char msg[256];
	int label;
	int pos = 0;

	TODO* list = NULL;

	while(fscanf(todo, "%[^\n\r] %d ", msg, &label) != EOF) {
		pos++;

		if (usePosition) {
			list = realloc(list, pos * sizeof(TODO));
			strcpy(list[pos-1].msg, msg);
			list[pos-1].label = label;
		}

		printTODO(msg, label, usePosition ? pos : -1);
	}

	fclose(todo);

	if (pos == 0)
		printf(COLOR_GREEN "-- Nothing to do --\n" COLOR_RESET);

	quantity = pos;
	return list;
}

/** Insert a TODO for the list */
void insertTODO(char* str, int label) {
	FILE* todo = fopen(FILENAME, "a+");
	if (todo == NULL) return;

	fprintf(todo, "%s\n%d\n", str, label);

	printTODO(str, label, -1);
	printf("Todo succesfully added!\n");

	fclose(todo);
}

/** Update a TODO from the list */
void updateTODO() {
	FILE* todo;
	TODO* list;
	int pos, qtt;

	list = listTODO(true);
	qtt = quantity;

	do {
		if (qtt == 0) {
			printf(COLOR_RED "Nothing to update.\n" COLOR_RESET);
			return;
		}

		printf("Todo you want to update: ");
		scanf("%d", &pos);

		if (pos > qtt || pos < 1) printf("Invalid position.\n");
		else {
			printf(COLOR_CYAN "The following todo will be updated:\n");
			printTODO(list[pos-1].msg, list[pos-1].label, -1);

			int option;
			do {
				printf(COLOR_CYAN "Update (1)-Message or (2)-Label? " COLOR_RESET);
				scanf("%d", &option);
			} while(option != 1 && option != 2);

			if (option == 1) {
				printf(COLOR_GREEN "New message: " COLOR_RESET);
				char msg[256];
				scanf(" %[^\n\r]", msg);
				strcpy(list[pos-1].msg, msg);
			} else {
				printf(COLOR_GREEN "New label: " COLOR_RESET);
				int label;
				scanf("%d", &label);
				list[pos-1].label = label;
			}
			
			todo = fopen(FILENAME, "w");

			for (int i = 0; i < qtt; i++)
				fprintf(todo, "%s\n%d\n", list[i].msg, list[i].label);

			printf(COLOR_GREEN "Todo succesfully updated.\n" COLOR_RESET);
		}

	} while (pos > qtt || pos < 1);

	free(list);
}

/** Remove a TODO from the list */
void removeTODO(bool all) {
	FILE* todo;
	TODO* list;
	int pos, qtt;

	if (all) {
		todo = fopen(FILENAME, "w");
		fclose(todo);

		printf("Everything was removed.\n");

		return;
	}

	list = listTODO(true);
	qtt = quantity;

	do {
		if (qtt == 0) {
			printf(COLOR_RED "Nothing to remove.\n" COLOR_RESET);
			return;
		}

		printf("Todo you want to remove: ");
		scanf("%d", &pos);

		if (pos > qtt || pos < 1) printf("Invalid position.\n");
		else {
			printf(COLOR_CYAN "The following todo will be removed:\n");
			printTODO(list[pos-1].msg, list[pos-1].label, -1);
			printf(COLOR_CYAN "Confirm? [y/n]" COLOR_RESET);
			char confirm;
			scanf(" %c", &confirm);

			if (confirm == 'y' || confirm == 'Y') {
				todo = fopen(FILENAME, "w");

				for (int i = 0; i < qtt; i++) {
					if (i == pos-1) continue;

					fprintf(todo, "%s\n%d\n", list[i].msg, list[i].label);
				}

				printf(COLOR_GREEN "Todo succesfully removed.\n" COLOR_RESET);
			} else {
				printf("Okay, nothing will be done.\n");
			}
		}

	} while (pos > qtt || pos < 1);

	free(list);
}

/** Shows the help message */
void helpTODO() {
	printf(COLOR_YELLOW "Welcome to todo! Take a look to the possible commands:\n\n");

	printf(COLOR_CYAN  "list: ");
	printf(COLOR_WHITE "lists all todos. Usage: ");
	printf(COLOR_GREEN "todo list\n");

	printf(COLOR_CYAN  "add: ");
	printf(COLOR_WHITE "add a todo to the list. Usage: ");
	printf(COLOR_GREEN "todo add <msg> <label>\n");

	printf(COLOR_CYAN  "update: ");
	printf(COLOR_WHITE "update a todo in the list. Usage: ");
	printf(COLOR_GREEN "todo update\n");

	printf(COLOR_CYAN  "remove: ");
	printf(COLOR_WHITE "remove a todo from the list. Usage: ");
	printf(COLOR_GREEN "todo remove\n");

	printf(COLOR_CYAN  "help: ");
	printf(COLOR_WHITE "shows this beautiful message. Usage: ");
	printf(COLOR_GREEN "todo help\n");

	printf("\n");
	printf(COLOR_RESET);
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		listTODO(false);
	} else {
		char* arg = strlwr(argv[1]);

		if 		  (!strcmp(arg, "list")) listTODO(false);
		else if   (!strcmp(arg, "add" )) {
			if (argc == 3) insertTODO(argv[2], 3);
			else if (argc == 4) insertTODO(argv[2], toint(argv[3]));
			else printf(COLOR_RED "Invalid entry! Use " COLOR_BRRED "todo add <msg> <label>\n" COLOR_RESET);
		} else if (!strcmp(arg, "update")) updateTODO();
		else if   (!strcmp(arg, "remove")) removeTODO(argc >= 3 && !strcmp(argv[2], "all"));
		else if   (!strcmp(arg, "help")) helpTODO();
		else {
			printf(COLOR_RED "Invalid entry! Type " COLOR_BRRED "todo help" COLOR_RED " for more info.\n" COLOR_RESET);
		}
	}

	printf(COLOR_RESET);
	return 0;
}