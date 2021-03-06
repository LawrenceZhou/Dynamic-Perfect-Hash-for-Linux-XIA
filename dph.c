//
//  dph.c
//  DPH
//
//This the implementation of Dynamic Perfect Hashing in C, referring to the paper of
//Martin Dietzfelbinger et. al, Dynamic Perfect Hashing: Upper and Lower Bounds(DPHULB).
//Hash is the main hash table, declared as a global variable. Efficiency of this code
//changes as parameters in functions as parameters c, SM, etc. change.
//
//  Created by Yijun Zhou on 1/29/16.
//  Copyright © 2016 Yijun Zhou. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "dph.h"

//If an operation(insert, delete, check if condition ** is satisfied) succeeds,
//it will return SUCCESS; otherwise UNSUCCESS.
#define SUCCESS 1
#define UNSUCCESS 0

//If position contains nothing or element is deleted, position will be set
//to DELETED, which is 0.
#define DELETED 0

//If a Lookup operation finds the element it is looking for, it will return FOUND;
//otherwise NOT_FOUND.
#define FOUND 1
#define NOT_FOUND 0

//If a hash function is injective on the list, it will set the flag to NO_COLLISION;
//otherwise COLLISION.
#define NO_COLLISION 1
#define COLLISION 0


uint32_t PRIME = 200003;//PRIME is a parameter used in the hash function.
uint32_t count = 0;     //A variable records the number of operations(Insert, Delete).
uint32_t c = 2;         //A parameter used to resize M
uint32_t SM = 1;        //SM is a parameter to generate a s(M) function.
uint32_t M = 0;         //The main hash table is to accomodate up to M elements.
HashTable Hash;         //The main hash table
list * new_list;        //I have to declare the lists to store the elements temporarily
//as a global variable. As the test set grows larger, there can be
//issues of memories with the local one.


//s(M)function is chosen to be O(n), and this makes the total space used is linear in the
//number of elements currently stored in the table. s(M) returns the new number of sub-hashtables.
uint32_t s(uint32_t size){
    return (uint32_t)(SM * size);
}

//Hash functions generator ( According to the paper DPHULB)
uint32_t HashFunction(uint32_t key, uint32_t k_random, uint32_t p_prime, uint32_t size){
    uint32_t result = ((k_random * key) % p_prime) % size;
    return result;
}

//Insert the key into the main hash table, Hash.
Status Insert(uint32_t key){
    count = count + 1;
    
    if (count > M){
        RehashAll(key);
    }
    else{
        uint32_t bucket_number = HashFunction(key, Hash.k_parameter, PRIME, Hash.size);
        uint32_t location = HashFunction(key, Hash.sub_table[bucket_number].k_parameter, PRIME,
                                         Hash.sub_table[bucket_number].space);
        if (key == Hash.sub_table[bucket_number].element[location]){
            //If position contains key, then do nothing.
        }
        else{
            Hash.sub_table[bucket_number].number =  Hash.sub_table[bucket_number].number + 1;
            if ( Hash.sub_table[bucket_number].number <= Hash.sub_table[bucket_number].length){
                //size of sub-hashtable sufficient
                if (Hash.sub_table[bucket_number].element[location] == DELETED){
                    //Store the key in the position, if it is deleted or the position is empty.
                    Hash.sub_table[bucket_number].element[location] = key;
                }
                else{
                    //A ccollision occurs so that the sub-hashtable has to be rehashed
                    uint32_t i;
                    list temp_list;
                    temp_list.number = 0;
                    temp_list.head = (nodep)malloc(sizeof(node));
                    nodep node_pointer = temp_list.head;
                    
                    //Append all elements in the sub-hashtable to temp_list
                    for (i = 0; i < Hash.sub_table[bucket_number].space; i++){
                        if (Hash.sub_table[bucket_number].element[i] != DELETED) {
                            nodep temp = (nodep)malloc(sizeof(node));
                            temp->element = Hash.sub_table[bucket_number].element[i];
                            node_pointer->next = temp;
                            node_pointer = temp;
                            temp_list.number = temp_list.number + 1;
                            Hash.sub_table[bucket_number].element[i] = DELETED;
                        }
                    }
                    
                    //Append key to temp_list
                    nodep temp = (nodep)malloc(sizeof(node));
                    temp->element = key;
                    temp->next = NULL;
                    node_pointer->next = temp;
                    temp_list.number = temp_list.number + 1;
                    
                    Hash.sub_table[bucket_number].number = temp_list.number;
                    int flag = 1;
                    int collision = NO_COLLISION;
                    uint32_t new_location;
                    
                    //Randomly choose hash function, until  it is injective on sub-hashtable, that is no collision
                    while (flag) {
                        node_pointer = temp_list.head->next;
                        Hash.sub_table[bucket_number].k_parameter = rand() % (PRIME - 1) + 1;
                        
                        for (i = 0; i < temp_list.number; i++) {
                            new_location = HashFunction(node_pointer->element,
                                                        Hash.sub_table[bucket_number].k_parameter,
                                                        PRIME, Hash.sub_table[bucket_number].space);
                            if (Hash.sub_table[bucket_number].element[new_location] != DELETED &&
                                Hash.sub_table[bucket_number].element[new_location] != node_pointer->element) {
                                collision = COLLISION;
                                break;
                            }
                            Hash.sub_table[bucket_number].element[new_location] = node_pointer->element;
                            node_pointer = node_pointer->next;
                        }
                        
                        
                        if (collision == NO_COLLISION)
                            flag = 0;
                        else{
                            collision = NO_COLLISION;
                            
                            for (i = 0; i < Hash.sub_table[bucket_number].space; i++)
                                Hash.sub_table[bucket_number].element[i] = DELETED;
                            
                        }
                        
                    }
                }
            }
            else{
                //Sub-hashtable is too small
                uint32_t i;
                list temp_list;
                temp_list.number = 0;
                temp_list.head = (nodep)malloc(sizeof(node));
                nodep node_pointer = temp_list.head;
                
                for (i = 0; i < Hash.sub_table[bucket_number].space; i++){
                    if (Hash.sub_table[bucket_number].element[i] != DELETED) {
                        nodep temp = (nodep)malloc(sizeof(node));
                        temp->element = Hash.sub_table[bucket_number].element[i];
                        node_pointer->next = temp;
                        node_pointer = temp;
                        temp_list.number = temp_list.number + 1;
                    }
                }
                
                nodep temp = (nodep)malloc(sizeof(node));
                temp->element = key;
                temp->next = NULL;
                node_pointer->next = temp;
                temp_list.number = temp_list.number + 1;
                
                //Double capacity of the sub-hashtable
                if (Hash.sub_table[bucket_number].length > 1)
                    Hash.sub_table[bucket_number].length = 2 * Hash.sub_table[bucket_number].length;
                else
                    Hash.sub_table[bucket_number].length = 2;
                Hash.sub_table[bucket_number].space = 2 * Hash.sub_table[bucket_number].length * \
                (Hash.sub_table[bucket_number].length - 1);
                
                //If condition ** is still satisfied, rehash the sub-hashtable
                if (isSatisfied()) {
                    uint32_t new_size = Hash.sub_table[bucket_number].space;
                    Hash.sub_table[bucket_number].element = realloc(Hash.sub_table[bucket_number].element, \
                                                                    new_size * sizeof(uint32_t));
                    
                    for (i = 0; i < Hash.sub_table[bucket_number].space; i++){
                        Hash.sub_table[bucket_number].element[i] = DELETED;
                    }
                    
                    Hash.sub_table[bucket_number].number = temp_list.number;
                    
                    int flag = 1;
                    int collision = NO_COLLISION;
                    uint32_t new_location;
                    while (flag) {
                        node_pointer = temp_list.head->next;
                        Hash.sub_table[bucket_number].k_parameter = rand() % (PRIME - 1) + 1;
                        
                        for (i = 0; i < temp_list.number; i++) {
                            new_location = HashFunction(node_pointer->element, \
                                                        Hash.sub_table[bucket_number].k_parameter, PRIME, \
                                                        Hash.sub_table[bucket_number].space);
                            if (Hash.sub_table[bucket_number].element[new_location] != DELETED &&
                                Hash.sub_table[bucket_number].element[new_location] != node_pointer->element) {
                                collision = COLLISION;
                                break;
                            }
                            Hash.sub_table[bucket_number].element[new_location] = node_pointer->element;
                            node_pointer = node_pointer->next;
                        }
                        
                        if (collision == NO_COLLISION)
                            flag = 0;
                        else{
                            collision = NO_COLLISION;
                            for (i = 0; i < Hash.sub_table[bucket_number].space; i++)
                                Hash.sub_table[bucket_number].element[i] = DELETED;
                        }
                    }
                    
                }
                else{
                    //Level-1 hash function is "bad"
                    RehashAll(key);
                }
            }
        }
    }
    return SUCCESS;
}

//Rehashall is called by Insert, Delete or Initialize. It rehashes the whole hash table.
Status RehashAll(uint32_t key){
    uint32_t i, k;
    uint32_t j = 0;
    list all_list;
    all_list.number = 0;
    all_list.head = (nodep)malloc(sizeof(node));
    nodep pointer = all_list.head;
    
    //Append all the elements in the hash table to all_list
    for(i = 0; i < Hash.size; i++){
        for (j = 0; j < Hash.sub_table[i].space; j++){
            if (Hash.sub_table[i].element[j] != DELETED) {
                nodep temp = (nodep)malloc(sizeof(node));
                temp->element = Hash.sub_table[i].element[j];
                pointer->next = temp;
                pointer = temp;
                all_list.number = all_list.number + 1;
            }
        }
    }
    
    free(Hash.sub_table);
    Hash.sub_table = NULL;
    
    if (key >= 1) {
        //If key is valid, append it to all_list
        nodep temp = (nodep)malloc(sizeof(node));
        temp->element = key;
        temp->next = NULL;
        pointer->next = temp;
        all_list.number = all_list.number + 1;
    }
    
    count =  all_list.number;
    //Resize the whole hash table
    if (count > 4) {
        M = (1 + c) * count;
    }
    else{
        M = (1 + c) * 4;
    }
    
    new_list = (list *)malloc((s(M)) * sizeof(list));
    int flag = 1, m = 1;
    nodep node_pointer[s(M)];
    //Randomly choose level-1 hash function until the condition ** is satisfied.
    while (flag){
        for(i = 0; i < s(M); i++){
            new_list[i].number = 0;
            if (m == 1) {
                new_list[i].head = (nodep)malloc(sizeof(node));
            }
        }
        
        Hash.k_parameter = rand() % (PRIME - 1) + 1;
        Hash.size = s(M);
        
        for (i = 0; i < Hash.size; i++){
            node_pointer[i] = new_list[i].head;
        }
        
        nodep pointer = all_list.head->next;
        
        //Form every list for sub-table
        for (i = 0; i < all_list.number; i++){
            uint32_t list_num = HashFunction(pointer->element, Hash.k_parameter, PRIME, Hash.size);
            nodep temp = (nodep)malloc(sizeof(node));
            temp->element = pointer->element;
            new_list[list_num].number = new_list[list_num].number + 1;
            node_pointer[list_num]->next = temp;
            node_pointer[list_num] = temp;
            pointer = pointer->next;
        }
        
        Hash.sub_table = (Bucket *)realloc(Hash.sub_table, Hash.size * sizeof(Bucket));
        
        for (i = 0; i < Hash.size; i++){
            Hash.sub_table[i].number = new_list[i].number;
            if (new_list[i].number > 1){
                Hash.sub_table[i].length = 2 * Hash.sub_table[i].number;
                Hash.sub_table[i].space = 2 * Hash.sub_table[i].length * \
                (Hash.sub_table[i].length - 1);
            }
            else{
                Hash.sub_table[i].length = 2;
                Hash.sub_table[i].space = 4;
            }
        }
        
        m++;
        if (isSatisfied()) {
            flag = 0;
        }
    }
    
    //Form every sub-hashtable
    for (i = 0; i < Hash.size; i++){
        int flag = 1;
        int collision = NO_COLLISION;
        uint32_t new_location;
        nodep pointer;
        Hash.sub_table[i].element = (uint32_t *)malloc(Hash.sub_table[i].space * sizeof(uint32_t));
        
        //Randomly choose hash function, until  it is injective on sub-hashtable, that is no collision
        while (flag) {
            Hash.sub_table[i].k_parameter = rand() % (PRIME - 1) + 1;
            pointer = (new_list[i].head->next);
            
            for (j = 0; j < Hash.sub_table[i].space; j++)
                Hash.sub_table[i].element[j] = DELETED;
            
            for (j = 0; j < new_list[i].number; j++) {
                new_location = HashFunction(pointer -> element, Hash.sub_table[i].k_parameter,
                                            PRIME, Hash.sub_table[i].space);
                if (Hash.sub_table[i].element[new_location] != DELETED) {
                    collision = COLLISION;
                    break;
                }
                Hash.sub_table[i].element[new_location] = pointer -> element;
                pointer = pointer -> next;
            }
            
            if (collision == NO_COLLISION )
                flag = 0;
            else{
                collision = NO_COLLISION;
                for (k = 0; k < Hash.sub_table[i].space; k++)
                    Hash.sub_table[i].element[k] = DELETED;
            }
        }
    }
    return SUCCESS;
}

//Delete key from the hash table
Status Delete(uint32_t key){
    count = count + 1;
    uint32_t bucket_number = HashFunction(key, Hash.k_parameter, PRIME, Hash.size);
    uint32_t location = HashFunction(key, Hash.sub_table[bucket_number].k_parameter,
                                     PRIME, Hash.sub_table[bucket_number].space);
    if (key == Hash.sub_table[bucket_number].element[location])
        Hash.sub_table[bucket_number].element[location] = DELETED;
    else
        return UNSUCCESS;
    
    if (count > M){
        //Start a new phase
        RehashAll(0);
    }
    
    return SUCCESS;
}

//Lookup key in hash table
Status Lookup(uint32_t key){
    uint32_t bucket_number = HashFunction(key, Hash.k_parameter, PRIME, Hash.size);
    uint32_t location = HashFunction(key, Hash.sub_table[bucket_number].k_parameter,
                                     PRIME, Hash.sub_table[bucket_number].space);
    if (key == Hash.sub_table[bucket_number].element[location]){
        return FOUND;
    }
    return NOT_FOUND;
}

//Initialize the hash table
Status Initialize(){
    Hash.k_parameter = rand() % (PRIME - 1) + 1;
    Hash.size = 0;
    Hash.sub_table = NULL;
    RehashAll(0);
    return SUCCESS;
}

//Check if condition ** is satisfied
Status isSatisfied(){
    uint32_t i, sum = 0;
    uint32_t threshold = 32 * M * M / Hash.size + 4 * M;
    
    for (i = 0; i < Hash.size; i++) {
        sum += Hash.sub_table[i].space;
        if (sum > threshold) {
            //If sum already exceeds threshold, terminate the loop.
            return UNSUCCESS;
        }
    }
    
    return SUCCESS;
}
