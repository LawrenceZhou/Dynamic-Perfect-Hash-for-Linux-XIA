//
//  test.c
//  DPH
//
//This is a test.
//
//  Created by  Yijun Zhou on 1/29/16.
//  Copyright © 2016 Yijun Zhou. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "dph.h"

extern uint32_t count;
extern HashTable Hash;
extern uint32_t PRIME;

int main(int argc, const char * argv[]) {
    uint32_t i;
    Initialize();
    
    //This is a test set of 10,000 elements.
    
    printf("Test of Insert\n");
    for (i = 3593593201 ; i < 3593593201+10000; i++) {
        Insert(i);
    }
    for (i = 3593593201 ; i < 3593593201+10000; i++) {
        uint32_t b =HashFunction(i, Hash.k_parameter, PRIME, Hash.size);
        uint32_t c =HashFunction(i, Hash.sub_table[b].k_parameter, PRIME, Hash.sub_table[b].space);
        printf("%4u,%4u,%4u,%4u,%4u\n",i,b, c, Hash.sub_table[b].element[c],Lookup(i));
    }
 
    for (i = 3593595201 ; i < 3593593201+3000; i++) {
        Delete(i);
    }
    printf("Test of Delete\n");
    for (i = 3593593201 ; i < 3593593201+10000; i++) {
        uint32_t b =HashFunction(i, Hash.k_parameter, PRIME, Hash.size);
        uint32_t c =HashFunction(i, Hash.sub_table[b].k_parameter, PRIME, Hash.sub_table[b].space);
        printf("%4u,%4u,%4u,%4u,%4u\n",i,b, c, Hash.sub_table[b].element[c],Lookup(i));
    }
    
    return 0;
}
