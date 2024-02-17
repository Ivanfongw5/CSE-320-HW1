/*
Haifeng Wu
SBU ID: 114375272
CSE 320
*/

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "huff.h"
#include "debug.h"



#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

/*
 * You may modify this file and/or move the functions contained here
 * to other source files (except for main.c) as you wish.
 *
 * IMPORTANT: You MAY NOT use any array brackets (i.e. [ and ]) and
 * you MAY NOT declare any arrays or allocate any storage with malloc().
 * The purpose of this restriction is to force you to use pointers.
 * Variables to hold the nodes of the Huffman tree and other data have
 * been pre-declared for you in const.h.
 * You must use those variables, rather than declaring your own.
 * IF YOU VIOLATE THIS RESTRICTION, YOU WILL GET A ZERO!
 *
 * IMPORTANT: You MAY NOT use floating point arithmetic or declare
 * any "float" or "double" variables.  IF YOU VIOLATE THIS RESTRICTION,
 * YOU WILL GET A ZERO!
 */
int find_min(int nodes_sum) {
    int minIndex = -1; // Initialize to an invalid index
    int minWeight = INT_MAX; // Initialize to maximum possible value

    NODE *current_node = nodes; // Pointer to the first node
for (int i = 0; i < nodes_sum; i++, current_node++) {
    if (current_node && current_node->weight < minWeight) {
        minWeight = current_node->weight;
        minIndex = i;
    }
}


    return minIndex; // Return the index of the node with minimum weight
}



void emit_huffman_tree_rec(NODE *node) {
    if (!node) return;

    // Check if it's a leaf node (no children)
    if (node->left == NULL && node->right == NULL) {
        fputc('1', stdout); // Mark the beginning of a leaf node
        fwrite(&(node->symbol), sizeof(node->symbol), 1, stdout); // Emit the symbol
    } else {
        fputc('0', stdout); // Mark the beginning of an internal node
        emit_huffman_tree_rec(node->left);
        emit_huffman_tree_rec(node->right);
    }
}


/**
 * @brief Emits a description of the Huffman tree used to compress the current block.
 * @details This function emits, to the standard output, a description of the
 * Huffman tree used to compress the current block.  Refer to the assignment handout
 * for a detailed specification of the format of this description.
 */
void emit_huffman_tree() {
    // To be implemented.
    
    //abort();
    emit_huffman_tree_rec(root);
    fflush(stdout); // Make sure all output has been written to stdout
}


/**
 * @brief Reads a description of a Huffman tree and reconstructs the tree from
 * the description.
 * @details  This function reads, from the standard input, the description of a
 * Huffman tree in the format produced by emit_huffman_tree(), and it reconstructs
 * the tree from the description.  Refer to the assignment handout for a specification
 * of the format for this description, and a discussion of how the tree can be
 * reconstructed from it.
 *
 * @return 0 if the tree is read and reconstructed without error, 1 if EOF is
 * encountered at the start of a block, otherwise -1 if an error occurs.
 */
int read_huffman_tree() {
    // To be implemented.
    abort();
}

void remove_node(NODE **nodes, int index, int *nodes_sum, int *node_symbol_count, int *endpoint) {
    NODE *nodeToRemove = *(nodes + index);
    if (nodeToRemove->left) nodeToRemove->left->parent = NULL;
    if (nodeToRemove->right) nodeToRemove->right->parent = NULL;

    if (nodeToRemove->parent) {// If the node has a parent, adjust its child pointers
        if (nodeToRemove->parent->left == nodeToRemove) {
            nodeToRemove->parent->left = NULL;
        } else if (nodeToRemove->parent->right == nodeToRemove) {
            nodeToRemove->parent->right = NULL;
        }
    }

    
   for (NODE **current = nodes + index; current < nodes + *nodes_sum - 1; current++) {
    *current = *(current + 1);
}

    (*nodes_sum)--; // Decrease total node count
}


/**
 * @brief Reads one block of data from standard input and emits corresponding
 * compressed data to standard output.
 * @details This function reads raw binary data bytes from the standard input
 * until the specified block size has been read or until EOF is reached.
 * It then applies a data compression algorithm to the block and outputs the
 * compressed block to the standard output.  The block size parameter is
 * obtained from the global_options variable.
 *
 * @return 0 if compression completes without error, -1 if an error occurs.
 */
int compress_block() {
    // To be implemented.
    //abort();
    
    int block_bits = (global_options & 0xffff0000) >> 16; //get the upper 16 bits
    int block_size = block_bits + 1;        
    int nodes_sum = 0;
    int node_symbol_count = 0;
    int next = 0;    //initial the bit
    for(int count = 0; count < block_size; count++){
        if (ferror(stdin)){
            return -1;
        }

        next = fgetc(stdin);    //get the next bit of input

        if(next != EOF){     //
            *(current_block + count) = next;
        } else return -1;
    }

    int current_block_number = 0;
    while (current_block_number < block_size){
        short symbol = *(current_block + current_block_number);
        int flag = 0;
        int node_count = 0;
        while(node_count < nodes_sum){
            NODE node = *(nodes+node_count);
            if (node.symbol == symbol){
                (*(nodes + node_count)).weight++;
                flag = 1;
            }
            node_count++;
        }

        if (flag == 0){
            NODE node;
            node.weight = 1;
            node.left = NULL;
            node.right = NULL;
            node.parent = NULL;
            node.symbol = symbol;
            *(nodes + nodes_sum) = node;
            nodes_sum++;
        }
        current_block_number++;
    }

    NODE node;
    node.left = NULL;
    node.right = NULL;
    node.parent = NULL;
    node.symbol = 0xff00;
    *(nodes + (nodes_sum++)) = node; num_nodes = nodes_sum;
    int endpoint = (2 * num_nodes - 1) - 1;
    while (nodes_sum > 1){
        int mini_index = find_min(nodes_sum);
        remove_node(mini_index, &nodes_sum, &node_symbol_count, &endpoint);
        NODE *min1 = &(*(nodes+endpoint + 1));

        int min2_index = find_min(nodes_sum);
        remove_node(min2_index, &nodes_sum, &node_symbol_count, &endpoint);
        NODE *min2 = &(*(nodes + endpoint + 1));

        NODE newNode;
        newNode.left = min1;
        newNode.right = min2;

        newNode.weight = (*min1).weight + (*min2).weight;
        newNode.symbol = ' ';
        newNode.parent = NULL;

        *(nodes + nodes_sum) = newNode;
        (*min1).parent = nodes + nodes_sum;
        (*min2).parent = nodes + nodes_sum;
        nodes_sum++;

        num_nodes++;
    }

    assign_code(node_symbol_count);
    emit_huffman_tree();
    emit_encoded_message(stdout. block_size, node_symbol_count);

    return 0;
        
}

/**
 * @brief Reads one block of compressed data from standard input and writes
 * the corresponding uncompressed data to standard output.
 * @details This function reads one block of compressed data from the standard
 * input, it decompresses the block, and it outputs the uncompressed data to
 * the standard output.  The input data blocks are assumed to be in the format
 * produced by compress().  If EOF is encountered before a complete block has
 * been read, it is an error.
 *
 * @return 0 if decompression completes without error, -1 if an error occurs.
 */
int decompress_block() {
    // To be implemented.
    abort();
}

/**
 * @brief Reads raw data from standard input, writes compressed data to
 * standard output.
 * @details This function reads raw binary data bytes from the standard input in
 * blocks of up to a specified maximum number of bytes or until EOF is reached,
 * it applies a data compression algorithm to each block, and it outputs the
 * compressed blocks to standard output.  The block size parameter is obtained
 * from the global_options variable.
 *
 * @return 0 if compression completes without error, -1 if an error occurs.
 */
int compress() {
    // To be implemented.
    abort();
}

/**
 * @brief Reads compressed data from standard input, writes uncompressed
 * data to standard output.
 * @details This function reads blocks of compressed data from the standard
 * input until EOF is reached, it decompresses each block, and it outputs
 * the uncompressed data to the standard output.  The input data blocks
 * are assumed to be in the format produced by compress().
 *
 * @return 0 if decompression completes without error, -1 if an error occurs.
 */
int decompress() {
    // To be implemented.
    abort();
}

/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the
 * program, returning 0 if validation succeeds and -1 if validation fails.
 * Upon successful return, the selected program options will be set in the
 * global variable "global_options", where they will be accessible
 * elsewhere in the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @return 0 if validation succeeds and -1 if validation fails.
 * Refer to the homework document for the effects of this function on
 * global variables.
 * @modifies global variable "global_options" to contain a bitmap representing
 * the selected options.
 */
int validargs(int argc, char **argv)
{
    // To be implemented.
    //abort();
    if (argc == 1){
        printf("Usage: bin/huff [-h] [-c|-d] [-b BLOCKSIZE]\n");     //print the usage
        printf("    -h       Help: displays this help menu\n");
        printf("    -c       Compress: read raw data, output compressed data\n");
        printf("    -d       Decompress: read compressed data, output raw data\n");
        printf("    -b       For compression, specify blocksize in bytes (range [1024, 65536])\n");
        return EXIT_FAILURE;          //if no flags provided, exit with an EXIT_FAILURE
    }

    char **arg = argv + 1;     //take the second argv
    if (new_strcmp(*arg, "-h") == 0){
        printf("Usage: bin/huff [-h] [-c|-d] [-b BLOCKSIZE]\n");    //print the usage
        printf("    -h       Help: displays this help menu\n");
        printf("    -c       Compress: read raw data, output compressed data\n");
        printf("    -d       Decompress: read compressed data, output raw data\n");
        printf("    -b       For compression, specify blocksize in bytes (range [1024, 65536])\n");
        return EXIT_SUCCESS;
    }

    for (int i = 1; i < argc; i++){   //continue to read the argv
        if (new_strcmp(*arg, "-c") == 0){  
            global_options |= 2;              //if "-c" then                                
        } else if (new_strcmp(*arg, "-d") == 0){
            global_options |= 4;                                           
        } else if (new_strcmp(*arg, "-b")== 0){
            if ((global_options & 2) == 0 || (i >= (argc - 1))){
                return EXIT_FAILURE;
            } else {
                i++;
                arg++;
                int a = new_atoi(*arg);
                if (a < 1024 || a > 65536){
                    return EXIT_FAILURE;
                } 
                a--;
                global_options |= a << 16;
            } 
        } else return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}



/*
This is a function I created to replace the strcmp function since we cannot use the string.h
*/
int new_strcmp(const char *s1, const char *s2) {
    for (; *s1 != '\0' && *s1 == *s2; ++s1, ++s2) {
    }
    return (*(unsigned char*)s1 - *(unsigned char*)s2);
}

/*
This is a function I created to replace the atoi function to convert the string to binary
*/
int new_atoi(const char *s1){
    int value = 0;
    if (*s1 == ' '){
        s1++;                           // jump the blank
    } else if (*s1 < '0' || *s1 > '9'){
        return 0;                //If the first char is not a number, return 0
    }
    while (*s1 >= '0' && *s1 <= '9' && *s1 != '\0'){
        value = value *10 + (*s1 - '0');
        s1++;
    }
    return value;
    
}


void remove_node(int index, int *nodes_sum, int *node_symbol_count, int *endpoint) {
    NODE *nodeToRemove = (nodes + index);
    if (nodeToRemove->left) {
        nodeToRemove->left->parent = NULL;
    }
    if (nodeToRemove->right) {
        nodeToRemove->right->parent = NULL;
    }
    
    if (nodeToRemove->parent) {// If the node has a parent, adjust its child pointers
        if (nodeToRemove->parent->left == nodeToRemove) {
            nodeToRemove->parent->left = NULL;
        } else if (nodeToRemove->parent->right == nodeToRemove) {
            nodeToRemove->parent->right = NULL;
        }
    }
    
   for (NODE **current = nodes + index; current < nodes + *nodes_sum - 1; current++) {
    *current = *(current + 1);
}

    (*nodes_sum)--; // Decrease total node count
}
 make
cc -Wall -Werror -Wno-unused-variable -Wno-unused-function -MMD -fcommon -std=gnu11 -I include -c -o build/huff.o src/huff.c
src/huff.c: In function ‘emit_huffman_tree’:
src/huff.c:75:27: error: passing argument 1 of ‘emit_huffman_tree_rec’ from incompatible pointer type [-Werror=incompatible-pointer-types]
   75 |     emit_huffman_tree_rec(&nodes);
      |                           ^~~~~~
      |                           |
      |                           NODE (*)[513] {aka struct node (*)[513]}
src/huff.c:47:34: note: expected ‘NODE *’ {aka ‘struct node *’} but argument is of type ‘NODE (*)[513]’ {aka ‘struct node (*)[513]’}
   47 | void emit_huffman_tree_rec(NODE *node) {
      |                            ~~~~~~^~~~
src/huff.c: In function ‘remove_node’:
src/huff.c:132:26: error: initialization of ‘NODE **’ {aka ‘struct node **’} from incompatible pointer type ‘NODE *’ {aka ‘struct node *’} [-Werror=incompatible-pointer-types]
  132 |    for (NODE **current = nodes + index; current < nodes + *nodes_sum - 1; current++) {
      |                          ^~~~~
src/huff.c:132:49: error: comparison of distinct pointer types lacks a cast [-Werror]
  132 |    for (NODE **current = nodes + index; current < nodes + *nodes_sum - 1; current++) {
      |                                                 ^
cc1: all warnings being treated as errors

