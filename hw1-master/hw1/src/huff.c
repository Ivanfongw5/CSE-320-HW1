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



void emit_huffman_tree_rec(NODE *node) {
    if (!node) 
        return;

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

    if (nodes->left == NULL && nodes->right == NULL) {
        fputc('1', stdout); // Mark the beginning of a leaf node
        fwrite(&(nodes->symbol), sizeof(nodes->symbol), 1, stdout); // Emit the symbol
    } else {
        fputc('0', stdout); // Mark the beginning of an internal node
        emit_huffman_tree_rec(nodes->left);
        emit_huffman_tree_rec(nodes->right);
    }
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

void assign_code(int a){

}

int find_min(int nodes_sum) {
    int minIndex = -1; // Initialize to an invalid index
    int minWeight = 999999; // Initialize to maximum possible value

    NODE *current_node = nodes; // Pointer to the first node
    for (int i = 0; i < nodes_sum; i++, current_node++) {
        if (current_node && current_node->weight < minWeight) {
            minWeight = current_node->weight;
            minIndex = i;
        }
    }
    return minIndex; // Return the index of the node with minimum weight
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
    NODE *current = nodes + index;
   for (int i = index; i < *nodes_sum - 1; current++) {
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
    //abort();
    if ((global_options & 0x2) == 0){
        return -1;              // check if the compression mode is choose "-c"
    }

    while (!feof(stdin) && !ferror(stdin)){
        int result = compress_block();  // check each block
        if (result != 0){
            return -1;
        }
        emit_huffman_tree();
    }
    if (ferror(stdin)){
        return -1;           //if there is an error reading the stdin, return fail
    }
    fflush(stdout);
    return 0;
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
int string_to_int(char *s1){
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

int validargs(int argc, char **argv)
{
    // To be implemented.
    //abort();
    int b_flag = 0;
    if (argc == 1){
        return EXIT_FAILURE;          //if no flags provided, exit with an EXIT_FAILURE
    }

    char **arg = argv + 1;     //take the second argv
    for (; argc > 1; arg++, argc--) {
    if (new_strcmp(*arg, "-h") == 0 && *(*arg + 2) == '\0') { 
        if (arg != argv + 1) { // If -h is not the first argument
            return EXIT_FAILURE;
        }
        global_options = 1; // Set least significant bit
        return 0;
    } else if (new_strcmp(*arg, "-c") == 0&& *(*arg + 2) == '\0') { // Compression flag
        global_options |= 2; // Set second bit
    } else if (new_strcmp(*arg, "-d")==0 && *(*arg + 2) == '\0') { // Decompression flag
        global_options |= 4; // Set third bit
    } else if (new_strcmp(*arg, "-b")==0) { // Block size flag
        if (argc < 3) return -1; // Ensure a number follows -b
        if (global_options & 0x4) {
            return -1;
        }
        b_flag = 1;
        arg++; // Move to the block size argument
        argc--;
        int blockSize = string_to_int(*arg);
        if (blockSize < 1024 || blockSize > 65536) return -1; // Check valid range
        global_options |= (blockSize - 1) << 16; // Store block size in the 16 most significant bits
    } else { 
        return -1;
    }
}
    
    if ((global_options & 0x2) && (global_options & 0x4)) return -1; // Both -c and -d are print together
    if (!(global_options & 0x6)) return -1; // neither input -c nor -d
    if (!b_flag) {
        global_options |= 0xFFFF0000;
    }

    return EXIT_SUCCESS;
}

