#include<iostream>
#include<fstream>
#include"huffman.h"

int main(int argc, char *argv[]) {
	if(!argc==3) return 23;
    HCadapt a;
	if(argv[1][0] == 'e') {
		a.encode(argv[2], argv[3]);
	}
	else {
		if(argv[1][0] == 'd') a.decode(argv[2], argv[3]);
	}
	return 0;
}
