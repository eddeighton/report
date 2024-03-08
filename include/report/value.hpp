
//  Copyright (c) Deighton Systems Limited. 2022. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#ifndef GUARD_2024_March_08_value
#define GUARD_2024_March_08_value

#include <variant>
#include <string>
#include <sstream>
#include <memory>

namespace report
{

struct ValueBase
{
    struct IValue
    {
        virtual ~IValue() = default;
        virtual void print( std::ostream& os ) = 0;
        virtual void load( std::istream& is ) = 0;
    };

    template< typename T >
    struct IValueImpl : public IValue
    {
        T m_value;
        virtual void print( std::ostream& os )
        {
            os << m_value;
        }
        virtual void load( std::istream& is )
        {
            is >> m_value;
        }
    };

    using IValuePtr = std::unique_ptr< IValue >;

    template< typename T >
    ValueBase( T value )
        :   m_pValue{ std::make_unique< T >( std::move( value ) ) }
    {

    }


    IValuePtr m_pValue;
};


using Value = std::variant< int, std::string >;

inline std::string toString( const Value& value )
{
    std::ostringstream osValue;
    std::visit( [ & ]( const auto& arg ) { osValue << arg; }, value );
    return osValue.str();
}


}

#endif //GUARD_2024_March_08_value
