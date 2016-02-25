I define a struct shared in the regex.l and regex.y, so you should include symbol.h to compile the program to get the correct ./a.out. The steps to get the correct programm should be the same as the follows:
1. bison -d regex.y
2. lex regex.l
3. gcc symbol.h lex.yy.c regex.tab.c -ly -ll
4. ./a.out < file.input > file.output