/*************************************************
	Name:		list.h
	Purpose:	Lovely singly linked list template class
	Date:		23/10/99
	Author:		Jim Hubbard
**************************************************/
#ifndef __LIST_H
#define __LIST_H

#include <stdlib.h>

template<class T> class List;

template<class T>
class Node
{
public:
	T *data;
	Node<T> *link;

	Node( ) : data(NULL), link(NULL) {}
	Node( T *addn, Node<T> *p ) : data(addn), link(p) {}
	Node<T> * Copy( ) const { return new Node<T>( data, (link == NULL?0:link->Copy( )) ); }
	~Node( ) {delete link;}
	void AddAfter( T *addn ) {link = new Node<T>( addn, link );}
};


template<class T>
class List
{
public:
	Node<T> *head;
	Node<T> *cursor;

	List( ) { head=NULL; cursor=NULL; }
	List( const List<T> &old ) { if( old.head != NULL ) head = old.head->Copy( ); }
	~List( ) {delete head;}
	List<T> & operator=( const List<T> &rhs ) 
	{
		if( this != &rhs ) head = ( rhs.head == NULL?NULL:rhs.head->Copy() );
		return( *this );
	}
	void Add( T *addn ) { head = new Node<T>( addn, head ); }
	void AddToEnd( T *addn )
	{
		Node<T> *prev, *curr = head;
		while( curr != NULL ) { prev = curr; curr = curr->link; }
		prev->AddAfter( addn );
	}
	bool Empty( ) const {return(head==NULL);}
	void SetIterator( ) {cursor = head;}
	T * Next( )
	{
		T *x = cursor->data;
		cursor = cursor->link;
		return( x );
	}
	void Delete( T * killee )
	{
		Node<T> *prev, *curr = head;
		while( curr != NULL && curr->data != killee ) { prev = curr; curr = curr->link; }
		if( curr == NULL ) return;
		(curr == head) ? head=curr->link : prev->link=curr->link;
	}
	bool More( ) const {return( cursor != NULL);}
	void Free( ) { delete head; head = cursor = NULL; }
};

#endif
