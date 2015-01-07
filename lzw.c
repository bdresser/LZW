// Bobby Dresser
// HW2: LZW Compression
// Due 11/07/2014
// CPSC 323


#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>
#include <string.h>
#include "trie.h"
#include "/c/cs323/Hwk2/code.h"

#define AUTH1 17
#define AUTH2 29

void encode(int empty, int prune, int maxbits){

    Node *trie = NULL;          // our beloved trie / string table 
    Node *newTrie = NULL;

    int cur_codes = 257;        // current number of codes in table
    int max_codes = 511;        // maximum # of codes supported by cur_bits
    int cur_bits = 9;           // current number of bits being used
    int target = 0;
    int triecount = 0;

    if(empty){
        cur_codes = 3;          // no codes in table if empty flag specified
    }

    int c = 0, k = 0;           // trusty c and k (current and next bit)

    putchar(AUTH1);   // validate file with decode
    putchar(AUTH2);
    putchar(maxbits);
    putchar(empty);
    putchar(prune);

    createTrie(&trie, cur_bits);  // make our trie!
    initTrie(trie, empty); 

    while ((k = getchar()) != EOF) {
            
        if(prune && (cur_codes > max_codes)){
           
            int keep = 0;
            int new_codes = 0;

            putBits(cur_bits, 0);
            putBits(cur_bits + 1, 0);

            int i = 257;
            while(i < cur_codes){               // determine how many codes to keep
                if(trie[i].seen > 1)            // not counting ASCII chars
                    keep += 1;
                i++;
            }

            newTrie = (Node *) malloc (sizeof (Node) * (257 + keep));
            initTrie(newTrie, empty);
            if(triecount > 1)
                triecount ++;
            new_codes = pruneTrie(trie, newTrie, cur_codes, empty);
            destroyTrie(trie, cur_codes);
            cur_codes = new_codes;
            trie = newTrie;

            i = 1;
            while(i <= cur_bits){
               if(new_codes < (1 << i)) {
                    cur_bits = i;
                    max_codes = (1 << i) - 1;
                    reallocTrie(&trie, cur_bits);
                    break;
                }
                i++;
            } 

        triecount ++;
        }

        target = binarySearch(trie[c].kids, k, 0, trie[c].numKids - 1);
                    
        if (target <= -1){                      // (C, K) not in the table 

            // -e flag specified, k doesn't exist in entire trie 
            if(empty && (binarySearch(trie[0].kids, k, 0, trie[0].numKids - 1) <= -1)){
                if( c != 0){
                    putBits (cur_bits, c);
                    trie[c].seen += 1;
                }
                putBits (cur_bits, 1);
                putBits (cur_bits, k);
                trieAdd(trie, 0, k, cur_codes);
                cur_codes ++;
                c = 0;
            }

           else{                                    // (C, K) is in table already
 
                putBits (cur_bits, c);              // add it!
                trie[c].seen += 1;                  // update seen value for pruning
            
                if (max_codes >= cur_codes){             // add a new node to trie!
                    trieAdd(trie, c, k, cur_codes);      // if there's room
                    cur_codes ++;
                }
      
                  // if the code can't be added to the tree....
                if (cur_codes > max_codes && maxbits > cur_bits){
                      putBits(cur_bits, 0);           // escape sequence 1
                      cur_bits = cur_bits + 1;        // start using another bit
                      reallocTrie(&trie, cur_bits);   // expand trie to make room 
                      max_codes = (1 << cur_bits) - 1;
                }
                if(empty){
                    c = searchTrie(trie, trie[k].pref, k);
                    if(c == -1){ 
                        c = 0;
                    }
                }
                else
                    c = k + 1;                          // move along
              }  
        }

        else if (target >= -1){                 // target is in table
            if (c != 0)                         // update seen value for purning
                trie[c].seen += 1;
            c = trie[c].kids[target].code;      // get appropriate code
        } 
    }

    if(c != 0){ 
        putBits(cur_bits, c);
        trie[c].seen = trie[c].seen + 1;
        destroyTrie(trie, cur_codes);
        flushBits();
    }	

    return;
}

void decode(){
    Node *trie = NULL;          // our beloved trie / string table 
    Node *newTrie = NULL;

    int oldC = 0;
    int i = 0;
    int cur_codes = 257;        // current number of codes in table
    int new_codes = 0;
    int max_codes = 511;        // maximum # of codes supported by cur_bits
    int cur_bits = 9;           // current number of bits being used
    int code = 0;
    int finalK = 0;
    int keep = 0;
    int tempK = 0;
    int saveC = 0;
    int empty, prune, maxbits;
    int uFlag = 0;

    // validate that encode created this file
    if ((getchar() != AUTH1) || (getchar() != AUTH2)){
            fprintf(stderr, "Wrong file");
            exit(0);
    }

    maxbits = getchar();
    empty = getchar();
    prune = getchar();

    if(empty){
        cur_codes = 1;          // no codes in table if empty flag specified
    }

    createTrie(&trie, cur_bits);  // make our trie!
    initTrie(trie, empty); 

    while((code = getBits(cur_bits)) != EOF){
        saveC = code;           // save value of code just in case

        if(code == 0){          // expand trie
            
            code = getBits (cur_bits + 1);
            saveC = code;

            if(code == 0 && prune){     // LETS PRUNE!!!!!

                i = 257;
                while(i < cur_codes){
                    if(trie[i].seen > 1)
                        keep += 1;
                    i++;
                  }

                newTrie = (Node *) malloc (sizeof (Node) * (257 + keep));
                initTrie(newTrie, empty);
                new_codes = pruneTrie (trie, newTrie, cur_codes, empty);
                destroyTrie(trie, cur_codes);
                cur_codes = new_codes;
                trie = newTrie;

                i = 1;
                while(i <= cur_bits){
                    if(new_codes < (1 << i)) {
                        cur_bits = i;
                        max_codes = (1 << i) - 1;
                        reallocTrie(&trie, cur_bits);
                        break;
                    }
                    i ++;
                }

                code = 0;
                oldC = 0;
                saveC = 0;
                finalK = 0;
                tempK = 0;
                continue;
            }

            if(cur_bits < maxbits)
                cur_bits = cur_bits + 1;
            max_codes = (1 << cur_bits) - 1;
            reallocTrie(&trie, cur_bits);
        }

    
        // this is KwK, since cur_codes alwayas points to 
        // the right code. if it goes too far, it means
        // there is a KwK happening
            if (code >= cur_codes){
                uFlag = 1;
                code = oldC;
                putString(trie, &tempK, code);
                seen(trie, code);
                putchar(finalK);
            }
            else{
                seen(trie, code);
                putString(trie, &finalK, code);
            }
               
        // add code to the trie, update total number of codes 
            if(oldC != 0) {
                if(cur_codes <= max_codes) {
                 arrayAdd(trie, oldC, finalK, cur_codes);
                 cur_codes = cur_codes + 1;
                }
            }

            oldC = saveC;
            if(uFlag){
                trie[oldC].seen += 1;
                uFlag = 0;
            }
        }
    
    destroyTrie(trie, cur_codes);
    return;
}

int main(int argc, char **argv){

    int i;
    int emptyFlag = 0;
    int pruneFlag = 0;
    int maxbits = 12;

    if (!strcmp(argv[0], "./decode") || !strcmp(argv[0], "decode")){
        if(argc > 1)
            fprintf(stderr, "decode does not take any arguments"); 
        decode();
    }

    else{
        
        for (i = 1; i < argc; i++){
        
            if (!strcmp("-m", argv[i])){
                int newbits = atoi(argv[i+1]);
                if (newbits < 0)
                    fprintf(stderr, "-m can't take negative argument");

                if (newbits <= 20 && newbits >= 9)
                    maxbits = newbits;
            }

            else if (!strcmp("-e", argv[i])){
                emptyFlag = 1;
            }
            else if (!strcmp("-p", argv[i])){
                pruneFlag = 1;
            }
        }

        encode(emptyFlag, pruneFlag, maxbits);
    }
    return 0;
}


