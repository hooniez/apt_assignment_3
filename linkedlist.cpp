#include "linkedlist.h"
#include <iostream>
#include "tile.h"

template <class T>
LinkedList<T>::LinkedList() : head(nullptr), tail(nullptr), length(0)
{}

template <class T>
LinkedList<T>::LinkedList(std::shared_ptr<std::vector<T>> contentList)
{
    head = nullptr;
    tail = nullptr;
    length = contentList->size();
    if (length > 0)
    {
        head = std::make_shared<Node<T>>(contentList->at(0));
        NodePtr<T> currNode = head;
        for (int i = 1; i < length; ++i)
        {

            currNode->setNext(std::make_shared<Node<T>>(contentList->at(i)));
            currNode = currNode->getNext();
        }
        tail = currNode;
    }
}

template <class T>
LinkedList<T>::~LinkedList()
{
    clear();
}

template <class T>
void LinkedList<T>::append(T newContent)
{
    NodePtr<T> newNode = std::make_shared<Node<T>>(newContent);
    // If LinkedList is empty
    if (!head)
    {
        head = newNode;
        tail = newNode;
    }
    else
    { // If any element exists in LinkedList
        tail->setNext(newNode);
        tail = newNode;
    }
    ++length;
}

template <class T>
T LinkedList<T>::dequeue()
{
    // content to return
    T outContent{};
    // If LinkedList is not empty
    if (head)
    {
        outContent = head->getContent();
        head = head->getNext();
        --length;
    }

    return outContent;
}

template <class T>
T LinkedList<T>::peekAt(int index) const
{
    T outContent{};
    if (index >= 0 && index < length)
    {
        NodePtr<T> currentNode = head;
        for (int i = 0; i < index; ++i)
        {
            currentNode = currentNode->getNext();
        }
        outContent = currentNode->getContent();
    }
    return outContent;
}

template <class T>
T LinkedList<T>::popAt(int index)
{
    T outContent{};

    if (index >= 0 && index < length)
    {
        if (index == 0)
        {
            outContent = head->getContent();
            head = head->getNext();
        }
        else
        {
            NodePtr<T> currentNode = head;

            //  get to the node before the index
            for (int i = 0; i < index - 1; ++i)
            {
                currentNode = currentNode->getNext();
            }
            outContent = currentNode->getNext()->getContent();
            // remove the node
            currentNode->setNext(currentNode->getNext()->getNext());

            if (index == length - 1)
            {
                tail = currentNode;
            }
        }
        --length;
    }
    return outContent;
}

template <class T>
void LinkedList<T>::insertAt(int index, T newContent)
{
    NodePtr<T> newNode = std::make_shared<Node<T>>(newContent);

    // if list is empty and insert position is 0
    if (length == 0 && index == 0)
    {
        head = newNode;
        tail = newNode;
        ++length;
    }
    else if (length > 0 && index >= 0 && index <= length)
    {
        if (index == 0)
        {
            newNode->setNext(head);
            head = newNode;
        }
        else if (index == length)
        {
            tail->setNext(newNode);
            tail = newNode;
        }
        else
        {
            NodePtr<T> currentNode = head;

            //  get to the node before the index
            for (int i = 0; i < index - 1; ++i)
            {
                currentNode = currentNode->getNext();
            }
            newNode->setNext(currentNode->getNext());
            currentNode->setNext(newNode);
        }
        ++length;
    }
}

template <class T>
NodePtr<T> LinkedList<T>::getHead() const
{
    return head;
}

template <class T>
void LinkedList<T>::clear()
{
    head = nullptr;
    tail = nullptr;
}

template <class T>
bool LinkedList<T>::hasLetter(Letter letter)
{
    bool foundLetter = false;
    NodePtr<T> currNode = head;
    while (!foundLetter && currNode)
    {
        if (letter == currNode->getContent()->getLetter())
        {
            foundLetter = true;
        }
        else
        {
            currNode = currNode->getNext();
        }
    }
    return foundLetter;
}

template <class T>
int LinkedList<T>::getLength()
{
    return length;
}

template <class T>
T LinkedList<T>::getTile(Letter letter)
{
    T outContent{};
    NodePtr<T> currNode = head;
    while (currNode)
    {
        if (currNode->getContent()->getLetter() == letter)
            outContent = currNode->getContent();
        currNode = currNode->getNext();
    }
    return outContent;
}

template <class T>
LinkedList<T>::LinkedList(const LinkedList<T> &other) : head(other.head),
                          tail(other.tail), length(other.length)
{}

template class LinkedList<TilePtr>;