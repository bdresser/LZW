/* trie.c */

typedef struct kidnode{
	int code;
	int myChar;
} kidNode;

// these nodes will make up our trie. Each contains an array of
// kidNodes that keep track of the given node's children.
typedef struct node{
    int pref;
    int newChar;
    int seen;
    int numKids;
    kidNode *kids;
} Node;

// Add a node to the tree. Takes the trie to add to, 
// a (p, c) pair that specifies the represented code, and
// an int that shows us to the next code in the array. It should
// also increment numKids in the parent node.
void trieAdd(Node *root, int p, int c, int index);

// adds new node to the long array of seen nodes.
// does not care about the children array. This is used
// by decode().
void arrayAdd(Node *root, int p, int c, int index);

// Allocate memory for a new tree
void createTrie(Node **root, int bits);

// initializes all 256 trie values, or none if e flag is specified.
void initTrie(Node *trie, int eFlag);

// reallocs the trie when we need to change number of
// bits being used 
void reallocTrie(Node **trie, int bits);

// Removes a node from the trie, decrements numKids in parent 
// node
void trieRemove(Node *root, int index);

// Searches through the tree for the pair (p, c) and returns
// the code for the pair if found or -1 if not found.
int searchTrie(Node *root, int p, int c);

// Frees trie. No memory leaks allowed!
void destroyTrie(Node *root, int size);

int binarySearch(kidNode *node, int find, int start, int last);

int topSearch(kidNode *node, int find, int last);

// Adds string to decode array

void printTrie(Node *root, int x);
void printArray(kidNode *array, int c);

// Used by decode to lookup a code in the table
// and print the code, starting at leaf node and
// tracing all the way up to the root.
int putString(Node* root, int *fK, int code);
int pruneTrie(Node *trie, Node *newTrie, int index, int empty);
int seen(Node *trie, int code);
void printChildren(kidNode *node, int count);
