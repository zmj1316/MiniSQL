#include "ra.tab.h"
#include <iostream>
#include <stdio.h>
using namespace std;
extern FILE *yyin;
extern int yyparse();
int main(int argc, char **argv){
	if (argc > 1) {
    	yyin = fopen(argv[1], "r");
    } 
    yyparse();
    // #if _WIN32
    // freopen( "CON", "w", stdout );
    // // #else
    // // freopen( "/dev/tty", "w", stdout );
    // // #endif
	getchar();
}
