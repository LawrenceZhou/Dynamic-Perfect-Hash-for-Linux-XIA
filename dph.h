//
//  dph.h
//  DPH
//
//  Created by Yijun Zhou on 1/29/16.
//  Copyright © 2016 Yijun Zhou. All rights reserved.
//

#ifndef dph_h

typedef int Status;

//Data structure to store sub-hashtable
typedef struct{
    uint32_t *element;
    uint32_t length;
    uint32_t number;
    uint32_t space;
    uint32_t k_parameter;
}Bucket;

//Data structure to store the hash table
typedef struct{
    Bucket *sub_table;
    uint32_t size;//s(M)
    uint32_t k_parameter;
}HashTable;

//Data structure to store node in List
typedef struct Node{
    uint32_t element;
    struct Node *next;
}node;

//Data structure to store a list
typedef struct{
    node *head;
    uint32_t number;
}list;

typedef node * nodep;

uint32_t s(uint32_t size);

uint32_t HashFunction(uint32_t key, uint32_t k_random, uint32_t p_prime, uint32_t size);

Status Insert(uint32_t key);

Status RehashAll(uint32_t key);

Status Delete(uint32_t key);

Status Lookup(uint32_t key);

Status Initialize();

Status isSatisfied();

#define dph_h
#endif /* dph_h */
