#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"
#include "util.h"

struct Node {
    node_t* next;
    node_t* prev;
    void* data;
};

void append(list_t* list, void* data) {
    m_check_pointer(list);

    node_t* new_node = (node_t*)malloc(sizeof(node_t));

    m_check_pointer(new_node);

    new_node->data = data;

    if(list->head == NULL){
        new_node->next = new_node;
        new_node->prev = new_node;
        list->head = new_node;
    } else {
        list->head->prev->next = new_node;
        new_node->prev = list->head->prev;
        list->head->prev = new_node;
        new_node->next = list->head;
    }

    list->length++;
}

void push(list_t* list, void* data) {
    m_check_pointers(list);

    append(list, data);

    list->head = list->head->prev;
}

void* pop(list_t* list, node_t** node) {
    m_check_pointers(list, node, *node);
    
    bool is_last_node = list->length == 1; 

    if(list->head == (*node)) {
        list->head = (*node)->next;
    }

    if(!is_last_node && (*node)->next != NULL) {
        (*node)->next->prev = (*node)->prev;
    }

    if(!is_last_node && (*node)->prev != NULL) {
        (*node)->prev->next = (*node)->next;
    }

    list->length--;

    node_t* temp_n = is_last_node ? NULL : (*node)->next;
    void* temp_d = (*node)->data;
    
    free(*node);
    if(is_last_node) list->head = NULL;
 
    *node = temp_n;  
    return temp_d;
}

void pop_free(list_t* list, node_t** node) {
    m_check_pointers(list, node, *node);

    bool is_last_node = list->length == 1; 

    if(list->head == (*node)) {
        list->head = is_last_node ? NULL : (*node)->next;
    }

    if((*node)->next != NULL) {
        (*node)->next->prev = (*node)->prev;
    }

    if((*node)->prev != NULL) {
        (*node)->prev->next = (*node)->next;
    }

    list->length--;

    node_t* temp_n = is_last_node ? NULL : (*node)->next;
    void* temp_d = (*node)->data;
    
    free(*node);
    free(temp_d);

    *node = temp_n;    
    return;
}

void freeConstantData(list_t* list) {
    m_check_pointer(list);

    node_t* n = list->head;
    
    while(n != NULL) {
        // as it is constant data we are not interested in it and we don't need 
        // to free it
        (void)pop(list, &n);
    }
}

node_t* iter(list_t* list, node_t** node) {
    m_check_pointers(list, node);

    if(*node == NULL) {
        *node = list->head;
    } else {
        *node = (*node)->next;

        if(*node == list->head) return NULL;
    }

    return *node;
}

node_t* riter(list_t* list, node_t** node) {
    m_check_pointers(list, node);

    if(*node == NULL) {
        *node = list->head->prev;
    } else {
        *node = (*node)->prev;

        if(*node == list->head->prev) return NULL;
    }

    return *node;
}

node_t* getHead(list_t* list) {
    m_check_pointers(list);
    
    return list->head;
}

node_t* getTail(list_t* list) {
    m_check_pointers(list);
    
    if(list->head != NULL) return list->head->prev;

    return NULL;
}

node_t* getNodeAt(list_t* list, int index) {
    m_check_pointers(list);
    
    if(index >= list->length || index < -list->length) return NULL;

    node_t* n = NULL;

    if(index >= 0) {
        int i = 0;

        while(iter(list, &n) != NULL && i < index) {
            i++;
        }
    } else {
        int i = index + 1;

        while(riter(list, &n) != NULL && i < 0) {
            i++;
        }
    }

    return n;
}

void* getNodeData(node_t* node) {
    if(node == NULL) return NULL;

    return node->data;
}

node_t* search(list_t* list, bool (*criterion)(void*)) {
    m_check_pointers(list, criterion);

    node_t* n = NULL;

    while(iter(list, &n)) {
        void* data = getNodeData(n);

        if(criterion(data)) return n;
    }

    return n;
}

void printList(list_t* list, void (*printData)(void*)) {
    m_check_pointers(list, printData);

    node_t* n = NULL;

    while(iter(list, &n)) {
        void* data = getNodeData(n);
        
        printData(data);
    }
}