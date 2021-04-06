#pragma once

#include <array>
#include <string_view>

namespace enum_ex_detail
{
    using view = std::string_view;

    constexpr auto trim_element( const char* str, size_t from, size_t to )
    {
        while( str[from] == ',' || str[from] == ' ' || str[from] == '\t' )
        {
            ++from;
        }

        for( size_t i = from; i < to; ++i )
        {
            if( str[i] == ' ' || str[i] == '=' || str[i] == '\t' )
            {
                to = i;
                break;
            }
        }

        return view{ &str[from], to - from };
    }

    template<typename EnumName, size_t size>
    class indexation
    {
        view arr[size];

    public:

        constexpr indexation( const char* str ) noexcept
        {
            size_t curr {}, count {}, begin {};
            while( str[count] != 0 )
            {
                if( str[count] == ',' )
                {
                    arr[curr] = trim_element( str, begin, count );
                    begin = count;
                    ++curr;
                }

                 ++count;
            }

            arr[curr] = trim_element( str, begin, count );
        }

        constexpr auto get( typename EnumName::enum_ idx ) const
        {
            return arr[size_t(idx)];
        }
    };

    constexpr auto elements_count( const char* str ) -> size_t
    {
        if( str[0] == 0 ) { return 1; }
        if( str[0] == ',' ) { return elements_count( &str[1] ) + 1; }
        return elements_count( &str[1] );
    }
} // namespace string_enum_detail


// Prevents unused member function warnings
#define ENUM_EX_SUPPRESS_UNUSED_WARINING(var) long _dummy_tmp_##var = (long(var) & 0);

// Generic enum definition for local scope usage
#define enum_ex_local_typed( enum_base, enum_name, ... ) \
static constexpr auto _count##enum_name = ::enum_ex_detail::elements_count(#__VA_ARGS__); \
struct enum_name \
{ \
    using base = enum_base; \
    enum enum_ : base { __VA_ARGS__ }; \
    using arr_type = std::array<enum_, _count##enum_name>; \
    using idx_type = ::enum_ex_detail::indexation<enum_name, _count##enum_name>; \
    static inline constexpr auto name() -> const char* { return #enum_name; } ENUM_EX_SUPPRESS_UNUSED_WARINING( name ) \
    static inline constexpr auto count() -> size_t { return _count##enum_name; } ENUM_EX_SUPPRESS_UNUSED_WARINING( count ) \
    static inline constexpr auto value( enum_ val ) -> enum_base { return base(val); } ENUM_EX_SUPPRESS_UNUSED_WARINING( value ) \
    static inline constexpr auto data() -> const arr_type { return arr_type{ __VA_ARGS__ }; } ENUM_EX_SUPPRESS_UNUSED_WARINING( data ) \
}; \
static constexpr auto enum_name##_to_string = enum_name::idx_type( #__VA_ARGS__ );

// Generic enum definition
#define enum_ex_typed( enum_base, enum_name, ... ) \
enum_ex_local_typed( enum_base, enum_name, __VA_ARGS__ ) \
inline constexpr auto to_string( enum_name::enum_ val ) -> const ::enum_ex_detail::view { return enum_name##_to_string.get(val); }

// Default enum definitions
#define enum_ex( enum_name, ... ) enum_ex_typed( uint32_t, enum_name, __VA_ARGS__ )
#define enum_ex_local( enum_name, ... ) enum_ex_local_typed( uint32_t, enum_name, __VA_ARGS__ )

// SFINAE detector
template<typename T, typename = int>
struct is_enum_ex : std::false_type {};

template <typename T>
struct is_enum_ex<T, decltype((void)T::name,
                              (void)T::count,
                              (void)T::value,
                              (void)T::data,
                              0)> : std::true_type {};
