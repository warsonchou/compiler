%{
	#include <string.h>
	#include <stdio.h>

	int isOnlyOneChar = 1;
	char ip[12];
	char getClass(char *);
	
%}

NUMBER [0-9]{1,3}
NEWLINE [\n]
DOT [\.]
LEGALIP {NUMBER}{DOT}{NUMBER}{DOT}{NUMBER}{DOT}{NUMBER}
OTHERS .*

%%
{LEGALIP} {
	char output = getClass(yytext);
	if (output == 'i') {
		printf("Invalid\n");
	} else {
		printf("%c\n", output);
	}
	isOnlyOneChar = 0;
}

{OTHERS} {
	printf("Invalid\n");
	isOnlyOneChar = 0;
}

{NEWLINE} {
	// printf("%d\n", isOnlyOneChar);
	if (isOnlyOneChar) {
		printf("Invalid\n");
	}
	isOnlyOneChar = 1;
}
%%

char getClass(char *str) {
	int interval[4], i = 0;
	char *p, *end;
	p = strtok(str, ".");
	while (p != NULL) {
		interval[i] = strtol(p, &end, 10);
		i++;
		p = strtok(NULL, ".");

		// if (strlen(p) > 3) {
		// 	return 'i';
		// }
	}

	if (interval[0] >= 0 && interval[0] <= 127 && interval[1] >= 0 &&
		interval[1] <= 255 && interval[2] >= 0 && interval[2] <= 255 && interval[3] >= 0 && interval[3] <= 255) {
		return 'A';
	} else if (interval[0] >= 128 && interval[0] <= 191 && interval[1] >= 0 &&
		interval[1] <= 255 && interval[2] >= 0 && interval[2] <= 255 && interval[3] >= 0 && interval[3] <= 255) {
		return 'B';
	} else if (interval[0] >= 192 && interval[0] <= 233 && interval[1] >= 0 &&
		interval[1] <= 255 && interval[2] >= 0 && interval[2] <= 255 && interval[3] >= 0 && interval[3] <= 255) {
		return 'C';
	} else if (interval[0] >=224 && interval[0] <= 239 && interval[1] >= 0 &&
		interval[1] <= 255 && interval[2] >= 0 && interval[2] <= 255 && interval[3] >= 0 && interval[3] <= 255) {
		return 'D';
	} else if (interval[0] >=225 && interval[0] <= 255 && interval[1] >= 0 &&
		interval[1] <= 255 && interval[2] >= 0 && interval[2] <= 255 && interval[3] >= 0 && interval[3] <= 255) {
		return 'E';
	} else {
		return 'i';
	}
}
