//---------------------------------------------------------------------------
//
// Project: OpenWalnut
//
// Copyright 2009 SomeCopyrightowner
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

#ifndef WEXCEPTION_TEST_H
#define WEXCEPTION_TEST_H

#include <string>

#include <boost/shared_ptr.hpp>
#include <cxxtest/TestSuite.h>

#include "../WException.h"

/** 
 * \par Description
 * Test WException
 */
class WExceptionTest : public CxxTest::TestSuite
{
public:
    /**
     * An instantiation should never throw an exception.
     */
    void testInstantiation( void )
    {
        TS_ASSERT_THROWS_NOTHING( WException e() );
        TS_ASSERT_THROWS_NOTHING( WException e( "Some message" ) );
    }

    /**
     * Getting the message means every trace element should be returned.
     */
    void testGetMessage( void )
    {
        WException e( "Dummy exception" );
        e.m_trace.push_back( "first" );
        e.m_trace.push_back( "second" );
        std::string expected = "Dummy exception\n\ntrace: first\ntrace: second";
        TS_ASSERT_EQUALS( expected, e.getTrace() );
        WException f;
        TS_ASSERT_EQUALS( std::string(), f.getTrace() );
    }

    /**
     * Test backtrace. This test always passes on platforms other than Linux!
     */
    void testBacktrace( void )
    {
#ifndef __linux__           // every platform not Linux will pass this test since only Linux is supported
        TS_ASSERT( true );
#endif  // __linux__
#ifdef __linux__
        try
        {
            new WException();
        }
        catch( const WException& e )
        {
            std::string bt = e.getBacktrace();
            // how to test this? Since the trace is different in release and debug mode, we simply test for
            // non empty string here.

            // TODO(ebaum): better test! (maybe use find to search for some texts like CxxTest::TestRunner or
            // something similar which only will work in debug mode)
            TS_ASSERT( bt.length() );
        }
#endif  // __linux__
    }
};

#endif  // WEXCEPTION_TEST_H
