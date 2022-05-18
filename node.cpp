
#include "node.h"

template <class T>
Node<T>::Node(T content) : content(content)
{}

template <class T>
Node<T>::Node(Node<T> &other) : content(other.content), next(nullptr)
{}

// a node is just a container, it doesn't own its content
template <class T>
Node<T>::~Node()
{}

template <class T>
void Node<T>::setNext(NodePtr<T> nextNode)
{
    next = nextNode;
}

template <class T>
NodePtr<T> Node<T>::getNext() const
{
    return next;
}
template <class T>
T Node<T>::getContent() const
{
    return content;
}

template class Node<TilePtr>;