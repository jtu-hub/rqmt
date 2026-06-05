#pragma once

#include <stdbool.h>

typedef struct Node node_t;

typedef struct List
{
    node_t* head;
    int length;
} list_t;

/**
 * \brief Adds a node storing data to the list at the end of the list, 
 *        i.e. the new node will be the tail of the list.
 * \param list      the list to which the data is added
 * \param data      the pointer to the data that is to be stored
 * \return void
 */
void append(list_t* list, void* data);

/**
 * \brief Adds a node storing data to the list at the beginning of the list, 
 *        i.e. the new node will be the head of the list.
 * \param list      the list to which the data is added
 * \param data      the pointer to the data that is to be stored
 * \return void
 */
void push(list_t* list, void* data);

/**
 * \brief Removes the node at the given address from the list, node will point 
 *        to the next node in the list (if none follow, node will be set to 
 *        NULL). If the last node is popped, and the list is circular the new 
 *        last element is returned instead of the head.Use for static data or 
 *        data than needs to be manipulated after the pop from the list.
 * \param list      the list from which the node is removed
 * \param node      the pointer to the node that is to be removed
 * \return the pointer to the data that was stored in the node (note that 
 *         dynamically allocated data will not be freed automatically by the pop
 *         method and must be taken care of after the pop)
 */
void* pop(list_t* list, node_t** node);

/**
 * \brief Removes the node at the given address from the list and frees the data 
 *        it stores, the node will point to the next node in the list (if none 
 *        follow, node will be set to NULL). Use for dynamic data, if the data 
 *        is to be deleted
 * \param list      the list from which the node is removed
 * \param node      the pointer to the node that is to be removed
 * \return the pointer to the data that was stored in the node
 */
void pop_free(list_t* list, node_t** node);

/**
 * \brief Iterates through the list starting from the provided node
 * \param list      the list to iterate through
 * \param node      A pointer to a pointer to the current node. If it points to 
 *                  no node (*node == NULL), the head of the list will be 
 *                  returned 
 * \return the next node or NULL if the end of the list is reached
 */
node_t* iter(list_t* list, node_t** node);

/**
 * \brief Deletes the whole list, no recursive free of allocated data as the 
 *        stored data is assumed to be constant
 * \param list      the list to iterate through
 * \return void
 */
void freeConstantData(list_t* list);

/**
 * \brief Returns the head of the list
 * \param list      The list to get the head from
 * \return the node which corresponds to the list's head
 */
node_t* getHead(list_t* list);

/**
 * \brief Returns the tail of the list
 * \param list      The list to get the tail from
 * \return the node which corresponds to the list's tail
 */
node_t* getTail(list_t* list);

/**
 * \brief Returns the node of the list at the given index, negative indices 
 *        retrieve the nodes counting backwards from the tail
 * \param list      The list to get the node from
 * \param index     The index, counting from the head at which the node to 
 *                  extract is located (value must be 
 *                  {-list.length, ... , list.lenght - 1})
 * \return the node which corresponds to the list's head
 */
node_t* getNodeAt(list_t* list, int index);

/**
 * \brief Extracts the data from the given node
 * \param node      the node to extract the data from
 * \return the pointer to the stored data
 */
void* getNodeData(node_t* node);

/**
 * \brief Searches the node which's data fulfills the criterion
 * \param list      the list to be searched
 * \param criterion the function used to asses if a node's data is the sought 
 *                  one, with signature: 
 *                  bool criterion(void* data); 
 * \return returns the first node that matches the criterion
 */
node_t* search(list_t* list, bool (*criterion)(void*));

/**
 * \brief iterates through the list and prints by means of the provided function 
 *        the node's data
 * \param list      the list to print
 * \param printData the function used to print the data with signature: 
 *                  void printData(void* data);
 */
void printList(list_t* list, void (*printData)(void*));