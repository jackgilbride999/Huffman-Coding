// C code for a file ADT where we can read a single bit at a
// time, or write a single bit at a time

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "bitfile.h"

// open a bit file in "r" (read) mode or "w" (write) mode
struct bitfile * bitfile_open(char * filename, char * mode)
{
	struct bitfile * this;
	this = malloc(sizeof(struct bitfile));
	if(mode[0]=='r'){
		this->is_read_mode=1;
	}
	else if(mode[0]=='w'){
		this->is_read_mode=0;
	}
	this->buffer=0;
	this->index=0;
	this->is_EOF=0;
	this->file = fopen(filename, mode);
	return this;
}

// write a bit to a file; the file must have been opened in write mode
void bitfile_write_bit(struct bitfile * this, int bit)
{
	this->buffer= this->buffer | (bit<<(this->index));
	this->index = this->index+1;
	assert(this->index<=8);
	if(this->index==8){
		fputc(this->buffer, this->file);
		this->index=0;
		this->buffer=0;
	}
}

// read a bit from a file; the file must have been opened in read mode
int bitfile_read_bit(struct bitfile * this)
{
	int bit = this->buffer & (1<<this->index);
	bit = bit >> this->index;
	this->index = this->index + 1;
	assert(this->index<=8);
	if(this->index==8){
		this->buffer = fgetc(this->file);
		this->index = 0;
	}
	return bit;
}

// close a bitfile; flush any partially-filled buffer if file is open
// in write mode
void bitfile_close(struct bitfile * this) {
	if(!this->is_read_mode && this->index>0)
	{
			fputc(this->buffer, this->file);
	}
	fclose(this->file);
}

// check for end of file
int bitfile_end_of_file(struct bitfile * this)
{
	return this->is_EOF;
}
