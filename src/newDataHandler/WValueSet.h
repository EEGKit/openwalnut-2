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

#ifndef WVALUESET_H
#define WVALUESET_H

#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>

/**
 * The valueset base class. This class contains only the base information needed: its size. It represents the actual date that resides <b>somewhere</b>. The derived classes can then handle storage.
 */
class WValueSetBase
{
public:
    /**
     * Convenience typedef for a boost::shared_ptr< WValueSet<...> >.
     */
    typedef boost::shared_ptr< WValueSetBase > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WValueSet<...> >.
     */
    typedef boost::shared_ptr< const WValueSetBase > ConstSPtr;

    /**
     * Constructor to create an empty value-set. Instantiating this class directly is quite useless as it does not store any data. Derive from
     * it.
     *
     * \param elements the number of elements to be used.
     */
    explicit WValueSetBase( size_t elements ):
        m_elements( elements )
    {
        // initialize
    }

    /**
     * Destructor.
     */
    virtual ~WValueSetBase()
    {
        // clean-up
    }

    /**
     * Returns the size of the valueset. It is the number of elements in it.
     *
     * \return number of elements
     */
    size_t size() const
    {
        return m_elements;
    }

private:
    /**
     * The number of elements in the data array.
     */
    size_t m_elements;
};

/**
 * This is a simple class handling storage of values in memory. It is typed according to the values it stores. Be aware, that this class deletes
 * its stored values upon destruction. If you want to share the values among several classes/functions, use a const shared pointer.
 *
 * \tparam ValueT the real type of the values.
 */
template< typename ValueT >
class WValueSet: public WValueSetBase
{
public:

    /**
     * The real type used for storing the values.
     */
    typedef ValueT ValueType;

    //! The type of values returned by this valueset.
    typedef ValueT& ValueReturnType;

    //! The type of values returned by this valueset when using const functions.
    typedef ValueT const& ValueReturnTypeConst;

    /**
     * Convenience typedef for a boost::shared_ptr< WValueSet<...> >.
     */
    typedef boost::shared_ptr< WValueSet< ValueType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WValueSet<...> >.
     */
    typedef boost::shared_ptr< const WValueSet< ValueType > > ConstSPtr;

    /**
     * Default constructor. This knows the real value-type and provides the needed mechanism to unveil it to an specified operator. It
     * additionally stores data as array in memory. If you want to write your own value-set, derive from WValueSetTyped.
     *
     * \param elements number of elements in the array
     */
    explicit WValueSet( size_t elements ):
        WValueSetBase( elements ),
        m_data( new ValueType[ elements ] )
    {
        // initialize
    }

    /**
     * Destructor. Does not necessarily remove the data array as it is reference counted and may be used by other value-sets.
     */
    virtual ~WValueSet()
    {
        // clean-up
        delete[] m_data;
    }

    /**
     * Provides access to an item at a given index.
     *
     * \param index the index to access
     *
     * \return the value at index.
     */
    ValueReturnTypeConst operator[]( size_t index ) const
    {
        return m_data[ index ];
    }

    /**
     * Provides access to an item at a given index.
     *
     * \param index the index to access
     *
     * \return the value at index.
     */
    ValueReturnType operator[]( size_t index )
    {
        return m_data[ index ];
    }

protected:
private:

    /**
     * The plain data. Created on construction and deleted on destruction.
     */
    ValueType* m_data;
};

// ######################################### data proxy object ###################################################

/**
 * A proxy object for access to a proxy valueset used to cast data
 * from an input type to the type stored in the valueset.
 *
 * \tparam T The type of the data that should be passed to and from the valueset.
 */
template< typename T >
class WDataProxy
{
public:

    /**
     * Constructor.
     *
     * \param vs A pointer to the base class of the proxy valuesets.
     * \param index The index of the element in the valueset this proxy object refers to.
     */
    WDataProxy( WValueSet< WDataProxy< T > >* vs, std::size_t index )
        : m_vs( vs ),
          m_index( index )
    {
    }

    /**
     * Cast the data aquired from the valueset to the type T.
     *
     * \return The casted data.
     */
    operator T() const
    {
        // the actual cast of the integral data elements is refered to the operator= of type T
        return m_vs->getData( m_index );
    }

    /**
     * Assignment operator. Allows to assign from any type that can be cast to T.
     *
     * \tparam S The type of the data to assign from.
     *
     * \param s The data to assign from.
     *
     * \return *this.
     */
    template< typename S >
    WDataProxy& operator= ( S const& s )
    {
        // does an implicit cast of s to type T and lets the valueset
        // do the cast from T to its stored data type via the operator= of
        // its stored data type
        m_vs->setData( m_index, s );
        return *this;
    }

    /**
     * Assignment operator for data proxy objects. This is essentially the default assignment
     * operator, implemented here to avoid the other assignment operator overwriting this one.
     *
     * \param p The data proxy to assign from.
     *
     * \return *this.
     */
    WDataProxy& operator= ( WDataProxy const& p )
    {
        if( this != &p )
        {
            m_vs = p.m_vs;
            m_index = p.m_index;
        }
        return *this;
    }

private:

    /**
     * A pointer to the proxy valueset. The proxy valueset instance knows the type
     * of the data stored in the actual dataset. We can thus call the correct virtual
     * functions for data conversion.
     */
    WValueSet< WDataProxy< T > >* const m_vs;

    //! The index of the referenced data element in the valueset.
    std::size_t m_index;
};

/**
 * A proxy object for access to a proxy valueset used to cast data
 * from an input type to the type stored in the valueset. This is the const
 * version that does not provide an assignment operator for data.
 *
 * \tparam T The type of the data that should be passed to and from the valueset.
 */
template< typename T >
class WDataProxyConst
{
public:

    /**
     * Constructor.
     *
     * \param vs A pointer to the base class of the proxy valuesets.
     * \param index The index of the element in the valueset this proxy object refers to.
     */
    WDataProxyConst( WValueSet< WDataProxy< T > > const* vs, std::size_t index )
        : m_vs( vs ),
          m_index( index )
    {
    }

    /**
     * Cast the data aquired from the valueset to the type T.
     *
     * \return The casted data.
     */
    operator T() const
    {
        return m_vs->getData( m_index );
    }

private:

    /**
     * A pointer to the proxy valueset. The proxy valueset instance knows the type
     * of the data stored in the actual dataset. We can thus call the correct virtual
     * functions for data conversion.
     */
    WValueSet< WDataProxy< T > > const* const m_vs;

    //! The index of the referenced data element in the valueset.
    std::size_t m_index;
};


// ######################################### proxy valueset ###################################################

/**
 * A specialization of the valueset for access to data using data proxy objects. This can
 * be used to access a valueset containing data of unknown type via the valueset returning reference
 * ("data proxy") objects with specified type, which can then be used similar to a reference to the data.
 * Types cast will be refered to the operator='s of the internal valueset data type and the type used to access
 * or cast operators, if they exist.
 *
 * This is a base class for the proxy valueset that knows the type of the data stored.
 *
 * \tparam T The type used for the data access.
 */
template< typename T >
class WValueSet< WDataProxy< T > >
{
public:

    //! The value type of this valueset.
    typedef WDataProxy< T > ValueType;

    //! The value type of this valueset when const.
    typedef WDataProxyConst< T > ValueTypeConst;

    //! The type returned by access operators.
    typedef ValueType ValueReturnType;

    //! The type returned by const access operators.
    typedef ValueTypeConst ValueReturnTypeConst;

    /**
     * Convenience typedef for a boost::shared_ptr< WValueSet<...> >.
     */
    typedef boost::shared_ptr< WValueSet< ValueType > > SPtr;

    /**
     * Convenience typedef for a boost::shared_ptr< const WValueSet<...> >.
     */
    typedef boost::shared_ptr< const WValueSet< ValueType > > ConstSPtr;

    /**
     * Destructor.
     */
    virtual ~WValueSet()
    {
    }

    /**
     * Const access operator. Returns a const data proxy object that references
     * a data element in the valueset pointed to by this proxy valueset.
     * The data proxy then refers read or write operations back to the valueset
     * that inherits from this class, which knows about both the data proxy's access
     * type and the actual valueset's stored data type, which in turn allows us to
     * perform casts between those.
     *
     * \param index The index of the data element in the valueset.
     *
     * \return The data proxy object.
     */
    virtual ValueTypeConst operator[]( size_t index ) const = 0;

    /**
     * Non-const access operator. Returns a data proxy object that references
     * a data element in the valueset pointed to by this proxy valueset.
     * The data proxy then refers read or write operations back to the valueset
     * that inherits from this class, which knows about both the data proxy's access
     * type and the actual valueset's stored data type, which in turn allows us to
     * perform casts between those.
     *
     * \param index The index of the data element in the valueset.
     *
     * \return The data proxy object.
     */
    virtual ValueType operator[]( size_t index ) = 0;

    /**
     * Set the data in the valueset to the given data. This actually does all the
     * type conversions. Called by the proxy object.
     *
     * \param index The index of the data element in the valueset.
     * \param t The data to assign from.
     */
    virtual void setData( std::size_t index, T t ) = 0;

    /**
     * Get the data from the valueset. This actually does all the
     * type conversions. Called by the proxy object.
     *
     * \param index The index of the data element in the valueset.
     * \return The data in the valueset cast to type T.
     */
    virtual T getData( std::size_t index ) const = 0;

protected:

    /**
     * Constructor.
     */
    WValueSet()
    {
    }
};

/**
 * This is a proxy for valuesets that return data proxy objects.
 * These can refer assignment or fetching operations back to the proxy valueset, which has access to
 * the actual valueset that stores the data.
 *
 * \tparam TargetT The type of the data that will be passed to and gotten from this proxy.
 * \tparam SourceT The type of the data in the valueset pointed to by this class.
 */
template< typename TargetT, typename SourceT >
class WValueSetProxy : public WValueSet< WDataProxy< TargetT > >
{
public:

    //! The value type of this valueset.
    typedef WDataProxy< TargetT > ValueType;

    //! The value type of this valueset when const.
    typedef WDataProxyConst< TargetT > ValueTypeConst;

    //! The type returned by access operators.
    typedef ValueType ValueReturnType;

    //! The type returned by const access operators.
    typedef ValueTypeConst ValueReturnTypeConst;

    /**
     * Constructs a proxy valueset.
     *
     * \param vs The valueset this should be a proxy for.
     */
    explicit WValueSetProxy( typename WValueSet< SourceT >::SPtr const& vs )
        : m_valueSet( vs )
    {
    }

    /**
     * Destructor.
     */
    virtual ~WValueSetProxy()
    {
    }

    /**
     * Non-const access operator. Returns a data proxy object that references
     * a data element in the valueset pointed to by this proxy valueset.
     * The data proxy then refers read or write operations back to this valueset,
     * which knows about both the data proxy's access type and the actual
     * valueset's stored data type, which in turn allows us to perform casts between those.
     *
     * \param index The index of the data element in the valueset.
     *
     * \return The data proxy object.
     */
    ValueType operator[] ( std::size_t index )
    {
        return ValueType( this, index );
    }

    /**
     * Const access operator. Returns a const data proxy object that references
     * a data element in the valueset pointed to by this proxy valueset.
     * The data proxy then refers read or write operations back to this valueset,
     * which knows about both the data proxy's access type and the actual
     * valueset's stored data type, which in turn allows us to perform casts between those.
     *
     * \param index The index of the data element in the valueset.
     *
     * \return The data proxy object.
     */
    ValueTypeConst operator[] ( std::size_t index ) const
    {
        return ValueTypeConst( this, index );
    }

    /**
     * Set the data in the valueset to the given data. This actually does all the
     * type conversions. Called by the proxy object.
     *
     * \param index The index of the data element in the valueset.
     * \param t The data to assign from.
     */
    void setData( std::size_t index, TargetT t )
    {
        // we use the operator= of the source type here
        m_valueSet->operator[] ( index ) = t;
    }

    /**
     * Get the data from the valueset. This actually does all the
     * type conversions. Called by the proxy object.
     *
     * \param index The index of the data element in the valueset.
     * \return The data in the valueset cast to type T.
     */
    TargetT getData( std::size_t index ) const
    {
        // we use the operator= of the target type here
        TargetT t;
        t = m_valueSet->operator[] ( index );
        return t;
    }

private:

    //! A pointer to the actual dataset that contains the data.
    typename WValueSet< SourceT >::SPtr m_valueSet;
};

// ####################################### proxy valueset visitor #############################################

/**
 * A visitor that is used to build a proxy valueset from a pointer to a WValueSetBase. This
 * is intended to be used with the type resolution mechanism.
 *
 * \tparam T The type used to access the data.
 */
template< typename T >
class WGetProxyValueSetVisitor
{
public:

    /**
     * Constructor.
     *
     * \param vsbase A pointer to an actual valueset.
     */
    explicit WGetProxyValueSetVisitor( WValueSetBase::SPtr const& vsbase )
        : m_base( vsbase )
    {
    }

    /**
     * The operator for the visitor mechanism. Builds the correct proxy object from T and the type of the
     * data in the valueset.
     *
     * \tparam S The type of the data in the valueset.
     */
    template< typename S >
    void operator() ( S /* not used */ )
    {
        m_result = typename WValueSetProxy< T, S >::SPtr(
                        new WValueSetProxy< T, S >(
                                boost::shared_dynamic_cast< WValueSet< S > >( m_base ) ) );
    }

    /**
     * Get the proxy valueset.
     *
     * \return The proxy valueset.
     */
    typename WValueSet< WDataProxy< T > >::SPtr get()
    {
        return m_result;
    }

private:

    //! The pointer to the valueset containing the data.
    WValueSetBase::SPtr m_base;

    //! The base pointer to the resulting proxy valueset.
    typename WValueSet< WDataProxy< T > >::SPtr m_result;
};

#endif  // WVALUESET_H
