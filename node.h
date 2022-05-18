
#ifndef ASSIGN2_NODE_H
#define ASSIGN2_NODE_H

#include "tile.h"

template <class T>
class Node
{
public:
   Node(T content);
   Node(Node &other);
   ~Node();
   void setNext(std::shared_ptr<Node<T>>);
   std::shared_ptr<Node<T>> getNext() const;
   T getContent() const;

private:
   const T content;
   std::shared_ptr<Node<T>> next;
};

template <class T>
using NodePtr = std::shared_ptr<Node<T>>;

template <class T>
using ContentPtr = std::shared_ptr<T>;

#endif // ASSIGN2_NODE_H
