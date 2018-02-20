#include"HCbitstream.h"
#include<iostream>
#include"stdio.h"

//TODO more isopened checks

HCifstream::HCifstream(const char *filename) {
	buf = 0;
    buf2 = 0;
    //buf2empty = true;

	used_bits = 0; 
	char_size = 8;
	last_bit_solve = 27;
        
	in = fopen(filename, "rb");
	isfine = (in!=NULL);
	if(isfine) read_int(last_bit_solve);
   	isopened = isfine;
	//std::cout<<"lastbit = "<<last_bit_solve<<"\n";
}

HCifstream::~HCifstream() {
    if(isopened) close();
	
}

void HCifstream::init() {
    buf2 = getc(in);
}

unsigned char HCifstream::readBit() {
	//if buf empty, I read new chunk
        char ans;
        if(!used_bits) {
             if(!feof(in)) {
                buf = buf2;
                buf2=getc(in);
                //std::cout<<"read\n";
                used_bits = (feof(in)) ? last_bit_solve : char_size;
                //if(feof(in)) std::cout<<"\n============BINGO============\n";
             }
             else {
                 return HC_EXAUSTED_STREAM;
             }
             //std::cout<<"new chunk readed. The chunk size is "<<used_bits<<"\n";
             //std::cout<<(int)buf;
             
              
        }
        
        ans = buf >> (char_size - 1);
        buf = buf<<1;
        used_bits--;
        return ans;
	

}

void HCifstream::read_int(unsigned int &a) {
	fread(&a, sizeof(int), 1, in);
}

void HCifstream::read_array_int(unsigned int *a, int size) {
        fread(a, sizeof(int), size,  in);
        // for(int i = 0; i<size; i++) std::cout<<"read: "<<a[i]<<"\n";
        if(ferror(in)) std::cout<<"NNNNNNNN\n";
}


void HCifstream::close() {
        fclose(in);
        //std::cout<<"Current buffer = "<<(int)buf<<" used_bits = "<<last_bit_solve<<"\n";
        isopened = false;
}
HCofstream::HCofstream(const char *filename) {
        buf = 0;
       // buf2 =0;
       // buf2empty = false;
        used_bits = 0;
        char_size = 8;
        out = fopen(filename, "wb");
        isfine = (out!=NULL);
        isopened = isfine;
        //I write down zeros to designate a needed space
        unsigned int needed_space = 0;
        write_int(needed_space);
}

HCofstream::~HCofstream() {
    if(isopened) close();

}

void HCofstream::close() {
    if(used_bits) putc(buf, out);
    
    rewind(out);
    write_int(used_bits);
    fclose(out);
    isopened = false;
   // std::cout<<"used "<<used_bits<<"\n";

}

void HCofstream::writeBit(unsigned char s) {
	if((s!=0)&&(s!=1)) throw(100500);
        //if the buffer is overflown, I write it down 
	if(used_bits==char_size) {
                 //std::cout<<"pushed"<<(int)buf<<"\n";
		putc(buf, out);
		buf = 0;
		used_bits = 0;
               
	}
	buf = buf | (s<<(char_size - used_bits - 1));
        used_bits++;
	
}

void HCofstream::write_int(unsigned int a) {
        //if(!isopened) throw(100500);
	fwrite(&a, sizeof(int), 1, out);
}

void HCofstream::write_array_int(unsigned int *arr, int size) {
	fwrite(arr, sizeof(int) ,  size, out);
        //for(int i = 0; i<size; i++) std::cout<<"written: "<<arr[i]<<"\n";
        if(ferror(out)) std::cout<<"hhhhhhhhhhhhh\n";
}

