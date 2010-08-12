//---------------------------------------------------------------------------
//
// Project: OpenWalnut ( http://www.openwalnut.org )
//
// Copyright 2009 OpenWalnut Community, BSV@Uni-Leipzig and CNCF@MPI-CBS
// For more information see http://www.openwalnut.org/copying
//
// This file is part of OpenWalnut.
//
// OpenWalnut is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenWalnut is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenWalnut. If not, see <http://www.gnu.org/licenses/>.
//
//---------------------------------------------------------------------------

#ifndef WSHAREDSEQUENCECONTAINER_H
#define WSHAREDSEQUENCECONTAINER_H

#include <algorithm>

#include <boost/thread.hpp>

#include "WSharedObject.h"

/**
 * This class provides a common interface for thread-safe access to sequence containers (list, vector, dequeue ).
 * \param S the sequence container to use. Everything is allowed here which privides push_back and pop_back as well as size functionality.
 */
template < typename S >
class WSharedSequenceContainer: public WSharedObject< S >
{
public:

    // Some helpful typedefs

    /**
     * A typedef for the correct const iterator useful to traverse this sequence container.
     */
    typedef typename S::const_iterator   ConstIterator;

    /**
     * A typedef for the correct iterator to traverse this sequence container.
     */
    typedef typename S::iterator         Iterator;

    /**
     * Default constructor.
     */
    WSharedSequenceContainer();

    /**
     * Destructor.
     */
    virtual ~WSharedSequenceContainer();

    //////////////////////////////////////////////////////////////////////////////////////////
    // These methods implement common methods of all sequence containers. The list is not
    // complete but should be enough for now.
    // \NOTE: all methods using or returning iterators are NOT implemented here. Use the access
    // Object (getAccessObject) to iterate.
    //////////////////////////////////////////////////////////////////////////////////////////

    /**
     * Adds a new element at the end of the container.
     *
     * \param x the new element.
     */
    void push_back( const typename S::value_type& x );

    /**
     * Removes an element from the end.
     */
    void pop_back();

    /**
     * Clears the container.
     */
    void clear();

    /**
     * The size of the container.
     *
     * \return the size.
     *
     * \note: be aware that the size can change at every moment after getting the size, since the read lock got freed. Better use
     * access objects to lock the container and use size() on the container directly.
     */
    size_t size();

    /**
     * Get item at position n. Uses the [] operator of the underlying container. Please do not use this for iteration as it locks every access.
     * Use iterators and read/write tickets for fast iteration.
     *
     * \param n the item index
     *
     * \return reference to element at the specified position
     */
    typename S::value_type& operator[] ( size_t n );

    /**
     * Get item at position n. Uses the [] operator of the underlying container. Please do not use this for iteration as it locks every access.
     * Use iterators and read/write tickets for fast iteration.
     *
     * \param n the item index
     *
     * \return reference to element at the specified position
     */
    const typename S::value_type& operator[] ( size_t n ) const;

    /**
     * Searches and removes the specified element. If it is not found, nothing happens. It mainly is a comfortable forwarder for std::remove.
     *
     * \param element the element to remove
     *
     * \return the new end iterator.
     */
    Iterator erase( const typename S::value_type& element );

    /**
     * Erase the element at the specified position. Read your STL reference for more details.
     *
     * \param position where to erase
     *
     * \return A random access iterator pointing to the new location of the element that followed the last element erased by the function call.
     */
    Iterator erase( Iterator position );

    /**
     * Erase the specified range of elements. Read your STL reference for more details.
     *
     * \param first Iterators specifying a range within the vector to be removed: [first,last).
     * \param last Iterators specifying a range within the vector to be removed: [first,last).
     *
     * \return A random access iterator pointing to the new location of the element that followed the last element erased by the function call.
     */
    Iterator erase( Iterator first, Iterator last );

protected:

private:
};

template < typename S >
WSharedSequenceContainer< S >::WSharedSequenceContainer():
    WSharedObject< S >()
{
    // init members
}

template < typename S >
WSharedSequenceContainer< S >::~WSharedSequenceContainer()
{
    // clean up
}

template < typename S >
void WSharedSequenceContainer< S >::push_back( const typename S::value_type& x )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().push_back( x );
}

template < typename S >
void WSharedSequenceContainer< S >::pop_back()
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().pop_back();
}

template < typename S >
void WSharedSequenceContainer< S >::clear()
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    a->get().clear();
}

template < typename S >
size_t WSharedSequenceContainer< S >::size()
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    size_t size = a->get().size();
    return size;
}

template < typename S >
typename S::value_type& WSharedSequenceContainer< S >::operator[]( size_t n )
{
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    return a->get().operator[]( n );
}

template < typename S >
const typename S::value_type& WSharedSequenceContainer< S >::operator[]( size_t n ) const
{
    typename WSharedObject< S >::ReadTicket a = WSharedObject< S >::getReadTicket();
    return a->get().operator[]( n );
}

    template < typename S >
typename WSharedSequenceContainer< S >::Iterator WSharedSequenceContainer< S >::erase( const typename S::value_type& element )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    return std::remove( a->get().begin(), a->get().end(), element );
}

template < typename S >
typename WSharedSequenceContainer< S >::Iterator WSharedSequenceContainer< S >::erase( typename WSharedSequenceContainer< S >::Iterator position )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    return a->get().erase( position );
}

template < typename S >
typename WSharedSequenceContainer< S >::Iterator WSharedSequenceContainer< S >::erase(
        typename WSharedSequenceContainer< S >::Iterator first,
        typename WSharedSequenceContainer< S >::Iterator last )
{
    // Lock, if "a" looses focus -> look is freed
    typename WSharedObject< S >::WriteTicket a = WSharedObject< S >::getWriteTicket();
    return a->get().erase( first, last );
}

#endif  // WSHAREDSEQUENCECONTAINER_H

