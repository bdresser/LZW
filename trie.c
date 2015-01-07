/* trie.c */

#include <stdio.h>
#include <stdlib.h>
#include "trie.h"
#define die(msg) exit (fprintf (stderr, "%s", msg))

void createTrie (Node **root, int bits)
{
    *root = (Node *) malloc (sizeof (Node) * (1 << bits));
    return; 
}

void reallocTrie(Node **node, int bits){
    *node = (Node *) realloc (*node, sizeof (Node) * (1 << bits));
    return;
}

void initTrie (Node *trie, int eFlag){
    
    // init root node, taking -e flag into account
    trie[0].newChar = 0;
    trie[0].seen = 0;
    trie[0].pref = 0;
    if (eFlag == 0){
        trie[0].numKids = 256;
        trie[0].kids = (kidNode*) malloc (sizeof (kidNode) * 256);
        for(int i = 0; i < 256; i++){

            // initialize node's array
            trie[0].kids[i].myChar = i;
            trie[0].kids[i].code = i + 1;

            // initialize all other nodes
            int j = i + 1;
            trie[j].pref = 0;
            trie[j].numKids = 0;
            trie[j].seen = 0;
            trie[j].kids = NULL;
            trie[j].newChar = i;
        }
    }
    
    // if -e flag is specified
    else{
        trie[0].numKids = 0;
        trie[0].kids = NULL; 
    }
}

void arrayAdd(Node *root, int p, int c, int index){

    // add new (p, c) pair to trie as its own node. 
    // new node is added sequentially at the end with
    // proper ties through its kid array
    root[index].kids = NULL;
    root[index].numKids = 0;
    root[index].seen = 1;
    root[index].newChar = c;
    root[index].pref = p;
    return;
}


void trieAdd(Node *root, int p, int c, int index){

    // add new (p, c) pair to trie as its own node. 
    // new node is added sequentially at the end with
    // proper ties through its kid array
    arrayAdd(root, p, c, index);

    // then, add the code to the list of children 
    // in its true parent node
    
    // if this is the first child in the trie
    if(root[p].kids == NULL) {
        root[p].kids = (kidNode *) malloc (sizeof (kidNode) * 2);
        root[p].numKids = 1;
        root[p].kids[0].code = index;
        root[p].kids[0].myChar = c;
    }

    else{

        int i = 0;

        // determine where to put the new node
        // these chars are sorted by increasing value for binary search
        // while(i >= 0 && j <= root[p].numKids){
        for(i = 0; i >= 0; i++){
            if(i == root[p].numKids){
                break;
            }
            if(c > root[p].kids[i].myChar){
//                i ++; j ++;
                continue;
            }
            if(c < root[p].kids[i].myChar)
                break;
        }
        
         // realloc memory for addition of new child
        root[p].kids = (kidNode *) realloc (root[p].kids, 
            (sizeof (kidNode) * (root[p].numKids + 1)));

        // if the new kid is not last in the array, then 
        // shift over all elements in the array that are 
        // after the new kid
        if(root[p].numKids != i){
            int j = root[p].numKids - 1;
            while(j >= i){
                root[p].kids[j+1].code = root[p].kids[j].code;
                root[p].kids[j+1].myChar = root[p].kids[j].myChar; 
                j--;
            }

            root[p].kids[i].code = index;
            root[p].numKids = root[p].numKids + 1;
            root[p].kids[i].myChar = c;
 
        }

        // add new kid!
        else if(root[p].numKids == i){
            root[p].kids[i].code = index;
            root[p].numKids = root[p].numKids + 1;
            root[p].kids[i].myChar = c;
        }
    }

}

int searchTrie(Node *root, int c, int k){
    
    if(root[c].kids == NULL){
        printf("null root");
        return -1;
    }

    int index = binarySearch(root[c].kids, k, 0, root[c].numKids - 1);
    
    if(index == -1){
        return -1;
    }
    else{
        if(c != 0)
            root[c].seen = root[c].seen + 1;
        return root[c].kids[index].code;
    }
}

void destroyTrie(Node *root, int index){
    for(int i = 0; i < index; i++){
        if (root[i].kids != NULL)
            free(root[i].kids);
        }

    free(root);
}

int seen(Node *root, int code){

    if(code < 0)
        die("Invalid code passed to trie.c");
    
    if(root[code].pref == 0){
        root[code].seen = root[code].seen + 1;
        return 1;
    }

    root[code].seen = root[code].seen + 1;
    seen(root, root[code].pref);
    return 1;
}

int pruneTrie(Node *old, Node *new, int cur_codes, int empty){
  
    int i = 0; 
    int seen = 0;
    int pref = 0;
    int c = 0;
    int new_codes = 257;
    int pastChar = 0;
    int pastPref = 0;
    int parent = 0;

    for(i = 257; i < cur_codes; i++){
        
        if(old[i].seen > 1){
            c = old[i].newChar;
            pref = old[i].pref;

            if(pref <= 255){
                trieAdd(new, pref, c, new_codes);
                new[new_codes].seen = old[i].seen / 2;
                ++new_codes; 
            }

            else{
                pastChar = old[pref].newChar;
                pastPref = old[pref].pref;

                parent = binarySearch(new[pastPref].kids, pastChar, 0, new[pastPref].numKids - 1);
                old[i].pref = new[pastPref].kids[parent].code;
                trieAdd(new, old[i].pref, c, new_codes);
                new[new_codes].seen = old[i].seen / 2;
                ++new_codes;
            }
        }
    }
    return new_codes;
}
                

int putString(Node *root, int *fK, int code){

    if(code < 0)
        die ("ERROR: Code not in table");

    if(root[code].pref == 0){
        *fK = root[code].newChar;
        putchar (root[code].newChar);
        return 1;
    }
    
    putString (root, fK, root[code].pref);
    putchar (root[code].newChar);
    return 1;
}

void printTrie(Node *root, int c){

    printf("i,pref[i],char[i],used[i]\n");
    for(int i = 2; i < c; i++){
        printf("%d,%d,%d,%d\n", i, root[i].pref, root[i].newChar, root[i].seen);
    }
}

void printChildren (kidNode *array, int count)
{
    int i;

    if (array == NULL)
        printf ("ERROR: children array is empty\n");

    for (i = 0; i < count; i++) {
        printf ("%d\t%d\n", array[i].myChar, array[i].code);
    }
}

void trieRemove(Node *root, int index){

}

int binarySearch(kidNode *node, int find, int start, int end){

    if(end < start)
        return -1;

    else{
        int middle = (start + end) / 2;
        if(node[middle].myChar > find)
            return binarySearch(node, find, start, middle-1); 
        else if(node[middle].myChar < find)
            return binarySearch(node, find, middle + 1, end);
        else if(node[middle].myChar == find)
            return middle;
        else
            return -1;
    }
}


