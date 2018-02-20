#pragma once
#include<stdio.h>

//class of bit2bit input-output
class HCifstream  {
private:
	FILE *in;
	unsigned char buf, buf2;
	//bool buf2empty;
	unsigned int used_bits;
	char char_size;
	unsigned int last_bit_solve; 
	bool isfine;
	bool isopened;
public:
	//my constructor. It opens files
	HCifstream(const char *filename);
	~HCifstream();
	
	void close();
	//reads new bit
	//return -1 if the stream is exausted
	unsigned char readBit();
	#define HC_EXAUSTED_STREAM 60
	//reading integers array function
	//ASSUMPTION:
	//this function will be called befor readBit
	void read_array_int(unsigned int *arr, int size);
	//integer reader. ASSUMPTION is the same
	void read_int(unsigned int &a);
	bool success() {return isfine;}; //whether the file is opened successful
	void init(); //initialize bit-by-bit reading
};

class HCofstream  {
private:
	unsigned char buf;
	//unsigned char buf2;
	//bool buf2empty;
	unsigned int used_bits;
	unsigned int char_size;
	FILE *out;
	bool isfine;
	bool isopened;

public:
	//constructor which opens a file
	HCofstream(const char *filename);
	//desctructor which closes file
	~HCofstream();
	
	void close();
	//writeBit function
	void writeBit(unsigned char s);
	
	//write_int_functions
	//ASSUMPTION: we write integers brfore our codes
	void write_int(unsigned int a);
	void write_array_int(unsigned int *arr, int size);
	bool success() {return isfine; };
};
