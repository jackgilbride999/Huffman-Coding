// code for a huffman coder


#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "huff.h"
#include "bitfile.h"


// create a new huffcoder structure
struct huffcoder *  huffcoder_new()
{
	// initialize the huffcoder
	struct huffcoder * this = malloc(sizeof(struct huffcoder));
	// set freqs to all 0s
	memset(this->freqs, 0, NUM_CHARS*sizeof(int));
	// set code_lengths to all 0s
  memset(this->code_lengths, 0, NUM_CHARS*sizeof(int));
	// set codes to all 0s
  memset(this->codes, 0, NUM_CHARS*sizeof(unsigned long long));

	// return the huffcoder structure
	return this;
}

// count the frequency of characters in a file; set chars with zero
// frequency to one
void huffcoder_count(struct huffcoder * this, char * filename)
{

 unsigned char c;  // we need the character to be
                    // unsigned to use it as an index
  FILE * file;
  file = fopen(filename, "r");
  assert( file != NULL );
  c = fgetc(file);	// attempt to read a byte
  while( !feof(file) ) {
		unsigned int character = (unsigned)c;
		this->freqs[character] = (this->freqs[character])+1;
    c = fgetc(file);
  }
  fclose(file);

	// loop through the frequencies. Where a frequency is 0 set it to 1
	// so that they get some sort of sensible encoding later.
	int j;
	for(j=0; j<NUM_CHARS; j++){
		if(this->freqs[j]==0){
			this->freqs[j]=1;
		}
	}
}

struct huffchar * remove_smallest(struct huffchar ** char_list)
// pass an array of pointers to huffchars
{
	struct huffchar * smallest;
	int i=0;
	while(char_list[i]==NULL){
		i++;
	}
	smallest = char_list[i];
	int smallestIndex = i;	
	for(i=i+1; i<NUM_CHARS; i++){
		struct huffchar * current = char_list[i];
		if((current!=NULL) && // cannot be smaller character if it is null
			((current->freq < smallest->freq) || // smaller character if its frequency is less
			((current->freq == smallest->freq) && (current->seqno < smallest->seqno))))
		{		
				smallest = current;
				smallestIndex = i;
		}
	}
	char_list[smallestIndex] = NULL;
	return smallest;
}

// insert a huffchar into the first null space in an array
int insert_huffchar(struct huffchar ** char_list, struct huffchar * character){
int inserted = 0;
int i = 0;
while(!inserted){
	if(char_list[i]==NULL){
		char_list[i] = character;
		inserted = 1;
	}
	i++;
}
return inserted;
}

struct huffchar * make_compound(struct huffchar * char1, struct huffchar * char2){
	// initialise compound character
	struct huffchar * compound	= malloc(sizeof(struct huffchar));
	// set the character to compound
	compound->is_compound=1;
	// combine the frequencies to a compound frequency
	int freq1 = char1 -> freq;
	int freq2 = char2 -> freq;
	compound->freq = freq1 + freq2; 
	// set the branches for the tree
	compound->u.compound.left=char1;
	compound->u.compound.right=char2;

	// return the compound char
	return compound;
}

// using the character frequencies build the tree of compound
// and simple characters that are used to compute the Huffman codes
void huffcoder_build_tree(struct huffcoder * this)
{
	// initializse array of pointers to huffchars
	struct huffchar * char_list[NUM_CHARS];

	// add each char to the huffchar list
	int i;
	for(i=0; i<NUM_CHARS; i++){
		// initialize the huffchar at index i
		char_list[i] = malloc(sizeof(struct huffchar));
		// the frequency of the character is its frequency in the huffcoder
		char_list[i]->freq = this->freqs[i];
		// set the character as not compound
		char_list[i]->is_compound = 0;
		// set the character c
		char_list[i]->u.c = i;
		// set the sequence number, will be needed later for characters
		// of the same frequency
		char_list[i]->seqno=i;
	}
	int next_seqno = NUM_CHARS;
	int j;
	for(j=0; j<NUM_CHARS-1; j++){
		// get the two least frequent characters
		struct huffchar * smallest = remove_smallest(char_list);
		struct huffchar * second_smallest = remove_smallest(char_list);
		// get the compound character with branches to the two small chars
		struct huffchar * compound = make_compound(smallest, second_smallest);
		compound->seqno=next_seqno;
		next_seqno = next_seqno + 1;
		insert_huffchar(char_list, compound);
	}

	// there is only one compound character now in the list this is the
	// root of the tree
	this->tree=char_list[0];
}

// reverse the order of a huffman code
unsigned long long reverse(unsigned long long code, int code_length){
	unsigned long long reverse = 0;	
	int i, j;
	j=0;
	for(i=code_length-1; i>=0; i--){
		// get a bit from the code, starting index is the first bit
		unsigned long long copy_bit = code & (1<<i);
		if(copy_bit!=0)	// i.e. if a bit was set
		{
			// set a bit in the reverse, starting at the last index
			reverse = reverse | (1<<j);
		}
		j++;
	}
	return reverse;
}

// recursive function to convert the Huffman tree into a table of
// Huffman codes
void tree2table_recursive(struct huffcoder * this, struct huffchar * node,
		 unsigned long long * path, int depth)
{
	if(node->is_compound){
		// get the child nodes of the compound node
		struct huffchar * leftnode = node->u.compound.left;
		struct huffchar * rightnode = node->u.compound.right;
		// get the paths for the children, append 0 on the end for a
		// left path and append 1 on the end for a right path
		unsigned long long newPath = *path;
		unsigned long long leftPath = newPath<<1;
		unsigned long long rightPath = leftPath + 1;
		// call recursive function for the left node
		tree2table_recursive(this, leftnode, &leftPath, depth+1);
		// call recursive function for the right node
		tree2table_recursive(this, rightnode, &rightPath, depth+1);
	}
	else{
		// this is not a compound node
		int i = node->u.c;
		// store the path of the leaf as a code
		this->codes[i]=reverse(*path, depth);
		// store the amount of digits, ie the depth
		this->code_lengths[i]=depth;
	}
}

// using the Huffman tree, build a table of the Huffman codes
// with the huffcoder object
void huffcoder_tree2table(struct huffcoder * this)
{	
	unsigned long long path = 0;
	tree2table_recursive(this, this->tree, &path, 0);
}


// print the Huffman codes for each character in order;
// you should not modify this function
void huffcoder_print_codes(struct huffcoder * this)
{
  int i, j;
  char buffer[NUM_CHARS];

  for ( i = 0; i < NUM_CHARS; i++ ) {
    // put the code into a string
    assert(this->code_lengths[i] < NUM_CHARS);
    for ( j = this->code_lengths[i]-1; j >= 0; j--) {
      buffer[j] = ((this->codes[i] >> j) & 1) + '0';
    }
    // don't forget to add a zero to end of string
    buffer[this->code_lengths[i]] = '\0';

    // print the code
    printf("char: %d, freq: %d, code: %s\n", i, this->freqs[i], buffer);;
  }
}

void write_code(struct bitfile * this, unsigned long long code, int code_length){
	int i;
	for(i=0; i<code_length; i++){
		int index_true = code & (1<<i);
		int bit = 0;
		if(index_true!=0){
			bit = 1;
		}
		bitfile_write_bit(this, bit);
	}
}

// encode the input file and write the encoding to the output file
void huffcoder_encode(struct huffcoder * this, char * input_filename,
		      char * output_filename)
{
	// open the input file
  FILE * file;
  file = fopen(input_filename, "r");
  assert( file != NULL );

	// open the output file
	struct bitfile * output_file = bitfile_open(output_filename, "w");	// Open a bitfile to write out to
  unsigned char c;  // we need the character to be unsigned to use it as an index 
	c = fgetc(file);	// read a character from the input 	
	while(!feof(file) && c!=255) {
		unsigned long long code = this->codes[c]; // get the huffman code of c
		int code_length = this->code_lengths[c]; // get the length of the code of c
		write_code(output_file, code, code_length);
		c = fgetc(file);	// read a character from the input
  }
	write_code(output_file, this->codes[4], this->code_lengths[4]); // write the encoded EOF character to the file
  fclose(file);
	bitfile_close(output_file);
}


// decode the input file and write the decoding to the output file
void huffcoder_decode(struct huffcoder * this, char * input_filename,
		      char * output_filename)
{
	// open the output file
	FILE * output;
	output = fopen(output_filename, "w");

	// open the input file
	struct bitfile * input_file = bitfile_open(input_filename, "r");
	struct huffchar * node = this->tree;
	input_file->buffer=fgetc(input_file->file);
	while(!input_file->is_EOF){
		int bit = bitfile_read_bit(input_file);
		if(bit==0){
			node = node->u.compound.left;
		} else if(bit==1) {
			node = node->u.compound.right;
		}
		if(!node->is_compound){
			if(node->u.c==4){
				// if the decoded character is EOFs
				input_file->is_EOF = 1;			
			}
			else{
				fputc(node->u.c, output);
				node = this->tree;
			}
		}
	}
	bitfile_close(input_file);
	fclose(output);
}
  
