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

#define BG_RED     "\x1b[41m"
#define BG_GREEN   "\x1b[42m"
#define BG_YELLOW  "\x1b[43m"
#define BG_BLUE    "\x1b[44m"
#define BG_MAGENTA "\x1b[45m"
#define BG_CYAN    "\x1b[46m"
#define BG_WHITE   "\x1b[47m"

//todo file path
#define FILENAME "/home/fakhoury/Public/todo.txt"

int quantity; //quantity of todos

/** Struct of a todo */
typedef struct _t {
	bool category;
	int label;
	char msg[256];
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

/** Just create a empty file with a Default category */
void emptyFile() {
	FILE* todo = fopen(FILENAME, "w+");
	fprintf(todo, "%%DEFAULT\n");
	fclose(todo);
}

/** Print a todo according to its label */
void printTODO(char* str, int label, bool category, int pos) {
	if (category) {
		printf(COLOR_BRWHITE "%s:\n" COLOR_RESET, str);
		return;
	}

	if (pos == -1) printf(" >");
	else printf(" [%03d] ", pos);

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
TODO* listTODO(bool usePosition, bool print) {
	FILE* todo = fopen(FILENAME, "r+");
	if (todo == NULL) {
		fclose(todo);
		emptyFile();
		printf(COLOR_YELLOW "-- Nothing to do --\n" COLOR_RESET);
		return NULL;
	}

	// if (print)
	// 	printf(COLOR_GREEN "Here is your list:\n" COLOR_RESET);

	char msg[256], category[256], opt;
	int label, pos = 0;

	TODO* list = NULL;

	// while(fscanf(todo, "%[^\n\r] ", msg) != EOF) {
	// 	if (msg[0] != '%') fscanf(todo, "%d ", &label);

	while(fscanf(todo, "%c ", &opt) != EOF) {
		if (opt == '%') fscanf(todo, "%[^\n\r] ", msg);
		else fscanf(todo, "%[^[][%d] ", msg, &label);

		// printf("__ %c __ %s %d\n", opt, msg, label);
		// print = false;
		
		pos++;

		if (usePosition) {
			list = realloc(list, pos * sizeof(TODO));
			strcpy(list[pos-1].msg, msg);
			list[pos-1].category = opt == '%';
			list[pos-1].label = label;
		}

		if (print)
			printTODO(msg, label, opt == '%', usePosition ? pos : -1);
	}

	fclose(todo);

	if (pos == 0 || pos == 1) {
		printf(COLOR_YELLOW "-- Nothing to do --\n" COLOR_RESET);
		emptyFile();
	}

	quantity = pos;
	return list;
}

/** Creates the TODO file according to a todo list */
void todoFromList(TODO* list, int size) {
	FILE* todo = fopen(FILENAME, "w");
	for (int i = 0; i < size; i++) {
		if (list[i].category)
			fprintf(todo, "%%%s\n", list[i].msg);
		else
			fprintf(todo, "-%s[%d]\n", list[i].msg, list[i].label);
	}
	fclose(todo);
}

/** Insert a TODO for the list */
void insertTODO(char* str, int label, char* category) {
	TODO* list = listTODO(true, false);
	if (list == NULL) return;

	int qtt = quantity;
	bool flag = false;

	for (int i = 0; i < qtt; i++) {
		if (list[i].category && !strcmp(list[i].msg, category)) {
			list = realloc(list, (qtt+1) * sizeof(TODO));
			for (int j = qtt; j > i+1; j--)
				list[j] = list[j-1];
			strcpy(list[i+1].msg, str);
			list[i+1].label = label;
			list[i+1].category = false;
			flag = true;
			break;
		}
	}

	if (!flag) {
		printf(COLOR_RED "Error on inserting...\n" COLOR_RESET);
		return;
	}

	todoFromList(list, qtt+1);
	printTODO(str, label, false, -1);
	printf("Todo succesfully added!\n");
}

/** Update a TODO from the list */
void updateTODO() {
	FILE* todo;
	TODO* list;
	int pos, qtt;

	list = listTODO(true, true);
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
			printTODO(list[pos-1].msg, list[pos-1].label, false, -1);

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

			todoFromList(list, qtt);

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

	list = listTODO(true, true);
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
			printTODO(list[pos-1].msg, list[pos-1].label, false, -1);
			printf(COLOR_CYAN "Confirm? [y/n]" COLOR_RESET);
			char confirm;
			scanf(" %c", &confirm);

			if (confirm == 'y' || confirm == 'Y') {
				for (int i = 0; i < qtt; i++) {
					if (i == pos-1) {
						for (int j = i; j < qtt-1; j++)
							list[j] = list[j+1];
						list = realloc(list, (qtt-1) * sizeof(TODO));
					}
				}

				todoFromList(list, qtt-1);

				printf(COLOR_GREEN "Todo succesfully removed.\n" COLOR_RESET);
			} else {
				printf("Okay, nothing will be done.\n");
			}
		}

	} while (pos > qtt || pos < 1);

	free(list);
}

/** Lists all the categories */
void listCategory() {
	TODO* list = listTODO(true, false);
	int qtt = quantity;

	for (int i = 0; i < qtt; i++)
		if (list[i].category)
			printf(COLOR_BLUE "%s\n" COLOR_RESET, list[i].msg);
	free(list);
}

/** Create a category with a name */
void createCategory(char* name) {
	FILE* todo = fopen(FILENAME, "a+");
	fprintf(todo, "%%%s\n", name);
	fclose(todo);

	printf(COLOR_GREEN "Category successfully created.\n" COLOR_RESET);
}

/** Remove a category and all the todos it contains */
void removeCategory(char* name) {
	TODO* list = listTODO(true, false);
	int qtt = quantity;

	printf(COLOR_GREEN "Really? Remove category %s?\n[y/n]" COLOR_RESET, name);
	char c; scanf(" %c", &c);
	if (c != 'Y' && c != 'y') return;

	int removed = 0;
	for (int i = 0; i < qtt; i++) {
		if (list[i].category && !strcmp(list[i].msg, name)) {
			removed = 1;
			for (int j = i; j < qtt-1; j++) {
				if (list[j].msg[0] == '%') break;
				list[j] = list[j+1];
				removed++;
			}
			list = realloc(list, (qtt-removed) * sizeof(TODO));
			todoFromList(list, qtt-removed);
			break;
		}
	}

	if (removed == 0) {
		printf(COLOR_RED "Error on inserting...\n" COLOR_RESET);
	} else {
		printf(COLOR_GREEN "Category successfully removed.\n" COLOR_RESET);
	}
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
	printf(COLOR_GREEN "todo add <msg> <label> <category>\n");

	printf(COLOR_CYAN  "update: ");
	printf(COLOR_WHITE "update a todo in the list. Usage: ");
	printf(COLOR_GREEN "todo update\n");

	printf(COLOR_CYAN  "remove: ");
	printf(COLOR_WHITE "remove a todo from the list. Usage: ");
	printf(COLOR_GREEN "todo remove\n");

	printf(COLOR_CYAN  "edit: ");
	printf(COLOR_WHITE "open vim to directly edit the todo file. Usage: ");
	printf(COLOR_GREEN "todo edit\n");

	printf(COLOR_CYAN  "subl: ");
	printf(COLOR_WHITE "open sublime to directly edit the todo file. Usage: ");
	printf(COLOR_GREEN "todo subl\n");

	printf(COLOR_CYAN  "category: ");
	printf(COLOR_WHITE "lists all categories. Usage: ");
	printf(COLOR_GREEN "todo category\n");

	printf(COLOR_CYAN  "category add: ");
	printf(COLOR_WHITE "add a category. Usage: ");
	printf(COLOR_GREEN "todo category add <name>\n");

	printf(COLOR_CYAN  "category remove: ");
	printf(COLOR_WHITE "remove a category. Usage: ");
	printf(COLOR_GREEN "todo category remove <name>\n");

	printf(COLOR_CYAN  "help: ");
	printf(COLOR_WHITE "shows this beautiful message. Usage: ");
	printf(COLOR_GREEN "todo help\n");

	printf("\n");
	printf(COLOR_RESET);
}

/** The well-known main function */
int main(int argc, char* argv[]) {

	if (argc == 1) {
		listTODO(false, true);
	} else {
		char* arg = strlwr(argv[1]);

		if 		  (!strcmp(arg, "list")) listTODO(false, true);
		else if   (!strcmp(arg, "add" )) {
			if (argc == 3) insertTODO(argv[2], 3, "DEFAULT");
			else if (argc == 4) insertTODO(argv[2], toint(argv[3]), "DEFAULT");
			else if (argc == 5) insertTODO(argv[2], toint(argv[3]), argv[4]);
			else printf(COLOR_RED "Invalid entry! Use " COLOR_BRRED "todo add <msg> <label> <category>\n" COLOR_RESET);
		} else if (!strcmp(arg, "update")) updateTODO();
		else if   (!strcmp(arg, "remove")) removeTODO(argc >= 3 && !strcmp(argv[2], "all"));
		else if   (!strcmp(arg, "help")) helpTODO();
		else if   (!strcmp(arg, "edit")) system("vim " FILENAME);
		else if   (!strcmp(arg, "vim")) system("vim " FILENAME);
		else if   (!strcmp(arg, "subl")) system("subl " FILENAME);
		else if   (!strcmp(arg, "category") || !strcmp(arg, "tag")) {
			if (argc == 2) listCategory();
			else if (argc == 3 && !strcmp(argv[2], "list")) listCategory();
			else if (argc == 4 && !strcmp(argv[2], "add")) createCategory(argv[3]);
			else if (argc == 4 && !strcmp(argv[2], "remove")) removeCategory(argv[3]);
		}
		else {
			printf(COLOR_RED "Invalid entry! Type " COLOR_BRRED "todo help" COLOR_RED " for more info.\n" COLOR_RESET);
		}
	}

	printf(COLOR_RESET);
	return 0;
}