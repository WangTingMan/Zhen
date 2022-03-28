#pragma once
#include <any>
#include <vector>
#include <type_traits>

#include <iostream>

class CommandHolder
{

public:

    std::any const& GetParameter( int a_index )const
    {
        return m_parameters.at( a_index );
    }

    std::size_t GetParameterSize()const
    {
        return m_parameters.size();
    }

    void SetWaitingParameterNum( int a_num )
    {
        m_waitingParameterNum = a_num;
    }

    int GetWaitingParameterNum()const
    {
        return m_waitingParameterNum;
    }

    void reset()
    {
        m_parameters.clear();
        m_waitingParameterNum = 0;
    }

    template< typename Type >
    void AddParameter( Type&& a_args )
    {
        if constexpr( std::is_same<std::any, std::decay_t<Type> >() )
        {
            m_parameters.emplace_back( std::forward<Type>( a_args ) );
        }
        else
        {
            static_assert( std::is_constructible_v<std::any, Type>, "Cannot construct an any with such arguments!" );
            m_parameters.emplace_back( std::make_any<std::decay_t<Type>>( std::forward<Type>( a_args ) ) );
        }
    }

    template< typename Type, typename... T,
        typename = typename std::enable_if<std::is_constructible_v<std::any, std::in_place_type_t<Type>, T...>, std::true_type >::type >
    void AddParameter( T&&... a_args )
    {
        static_assert( std::is_constructible_v<Type, T...>, "Cannot construct a Type with such arguments!" );
        static_assert( std::is_constructible_v<std::any, std::in_place_type_t<Type>, T...>, "Cannot construct an any with such arguments!" );
        m_parameters.emplace_back( std::make_any<std::decay_t<Type>>( std::forward<T>( a_args )... ) );
    }

private:

    int m_waitingParameterNum = 0;
    std::vector<std::any> m_parameters;
};


