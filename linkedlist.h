#ifndef ASSIGN2_LINKEDLIST_H
#define ASSIGN2_LINKEDLIST_H

#include "node.h"
#include <vector>
#include <sstream>

/*
 * This is the template of a generic linked list containing only basic
 * operation required for the game.
 */

template <class T>
class LinkedList
{
public:
    LinkedList();
    LinkedList(const LinkedList<T> &);
    // initialise the list form a value list
    LinkedList(std::shared_ptr<std::vector<T>> contentList);

    ~LinkedList();

    // void print();

    // // add a value to the start of the list void prepend(T *newValue);

    // add the new value(content) to the end of the list
    void append(T newConent);

    // remove and return a value form the start of the list return nullptr if
    // the list is empty
    T dequeue();

    // remove and return a value form the end of the list return nullptr if
    // the list is empty T *pop();

    NodePtr<T> getHead() const;

    // return a hard copy of the content of the node at the given index
    // without removing returns a null point if the index is out of range.
    T peekAt(int index) const;

    // remove and return the node at the given index returns a null point if
    // the index is out of range.
    T popAt(int index);

    // THIS METHOD IS NOT USED DUE TO THE REQUIREMNTS add the new
    // value(content) to the given index of the list the range of index
    // includes 0 and the length. do nothing if the index is out of range.
    void insertAt(int index, T newConent);

    void clear();

    // check if a value is in the list
    bool hasLetter(Letter letter);
    // Return a pointer to a tile with a given letter
    T getTile(Letter letter);

    // Return all the letters in the hand in a string
    std::string getLetters();

    int getLength();

private:
    NodePtr<T> head;
    NodePtr<T> tail;
    int length;
};

template <class T>
using LinkedListPtr = std::shared_ptr<LinkedList<T>>;

#endif // ASSIGN2_LINKEDLIST_H