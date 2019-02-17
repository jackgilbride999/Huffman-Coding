# Huffman-Coding
C program which uses Huffman coding to encode a text file, can then decode this file back into a text file. Takes a training file of text to compute the frequency of characters and how to encode them. The training file may or may not be the file to be encoded.

The program uses the ADT in bitfile.c to write encoded files and read from decoded files. The read bit functionality takes a byte from the file and puts it into a buffer, so when the function is called it may return the next bit in the buffer. This allows the encoded file to be read bit by bit, which is useful as the huffman decoder must follow the huffman tree bit-by-bit. Likewise the write bit functionality allows encoded characters of any bit length to be written to the bitfile bit-by-bit, so that encoded characters may cross byte boundaries without the huffman coder needing to know exacty how.

Usage:
- With all files in the same repository, compile the program; "gcc -Wall -g -o huff main.c bitfile.c huff.c -I. -lm"
- To encode a file, run the program passing the word encode, name of the training file, input file and output file as arguments, e.g. "./huff encode sampletrainfile.txt sampleinput.txt sampleencoding". The encoded file should now be in your directory.
- To decode a file, run the program passing the word decode, the name of the training file, encoded file and the output file as arguments, e.g. "./huff decode sampletrainfile.txt sampleencoding sampledecoding". The decoded file should now be in your directory.
- To show the huffman encoding of characters based on a training file, run the program passing the word showcodes followed by the training file name as arguments, e.g. "./huff showcodes sampletrainfile.txt". The huffman codes should appear in the terminal window.

Known limitations:
- The program assumes that the files passed are of the right type. This means that the file to be encoded and the training file should be made up of ASCII characters. The file to be decoded should be a file previously encoded by the program using the same training file.
