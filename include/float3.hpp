#pragma clang diagnostic push
#pragma ide diagnostic ignored "portability-simd-intrinsics"
#pragma ide diagnostic ignored "hicpp-explicit-conversions"
#ifndef MATHEMATICS_SIMD_FLOAT3_HPP
#define MATHEMATICS_SIMD_FLOAT3_HPP

#include <immintrin.h>
#include <utility>
#include <cmath>
#include <cstdio>
#include <iostream>
#include "constants.hpp"

namespace mathsimd {

    struct float3 {
    private:
        __m128 _val{0, 0, 0};
    public:
        float3() = default;
        float3(float const &x, float const &y, float const &z) : _val{x, y, z} {}
        float3(float3 const &other) : _val(other) {}
        float3(__m128 const &other) : _val(other) {}
        inline operator __m128() const { return _val; }
        inline float3 &operator=(float3 const &other) = default;
        inline float3 &operator=(__m128 const &other) { _val = other; return *this; }
        float &x() { return *reinterpret_cast<float*>(&_val); }
        float &y() { return *(reinterpret_cast<float*>(&_val) + 1); }
        float &z() { return *(reinterpret_cast<float*>(&_val) + 2); }
        [[nodiscard]] float x() const { return _val[0]; }
        [[nodiscard]] float y() const { return _val[1]; }
        [[nodiscard]] float z() const { return _val[2]; }

        #define ARITHMETIC(OP) \
        friend float3 operator OP (float3 const &a, float3 const &b); \
        template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type *> \
        friend float3 operator OP (T const &a, float3 const &b); \
        template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type *> \
        friend float3 operator OP (float3 const &a, T const &b);
        ARITHMETIC(+)
        ARITHMETIC(-)
        ARITHMETIC(*)
        #undef ARITHMETIC
        template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type *>
        friend float3 operator / (float3 const &a, T const &b);

        static inline float dot(float3 const &a, float3 const &b) {
            auto c = a * b;
            return _mm_add_ss(_mm_add_ss(c, _mm_shuffle_ps(c,c,85)), _mm_unpackhi_ps(c,c))[0];
        }

        [[nodiscard]] inline float sqrMagnitude() const { return dot(*this, *this); }
        [[nodiscard]] inline float magnitude() const { return std::sqrt(sqrMagnitude()); }
        [[nodiscard]] inline float3 normalized() const { return this->_val / magnitude(); }

        static inline float3 cross(float3 const &a, float3 const &b) {
            auto tmp0 = _mm_shuffle_ps(a._val,a._val,_MM_SHUFFLE(3,0,2,1));
            auto tmp1 = _mm_shuffle_ps(b._val,b._val,_MM_SHUFFLE(3,1,0,2));
            auto tmp2 = _mm_shuffle_ps(a._val,a._val,_MM_SHUFFLE(3,1,0,2));
            auto tmp3 = _mm_shuffle_ps(b._val,b._val,_MM_SHUFFLE(3,0,2,1));
            return _mm_sub_ps(_mm_mul_ps(tmp0,tmp1),_mm_mul_ps(tmp2,tmp3));
        }

        #define FUNC(NAME,X,Y,Z) \
        static inline float3 NAME () { return {X,Y,Z}; }
        FUNC(up, 0,1,0)
        FUNC(down, 0,-1,0)
        FUNC(right, 1,0,0)
        FUNC(left, -1,0,0)
        FUNC(forward, 0,0,1)
        FUNC(back, 0,0,-1)
        FUNC(one, 1,1,1)
        FUNC(zero, 0,0,0)
        #undef FUNC
    };

    inline __m128 _mm_abs_ps(__m128 fp_val) {
        static const __m128i NEG{0x7fffffff7fffffff,0x7fffffff7fffffff};
        auto tmp = _mm_and_si128(_mm_castps_si128(fp_val), NEG);
        return _mm_castsi128_ps(tmp);
    }

    #define ARITHMETIC(OP) \
        inline float3 operator OP (float3 const &a, float3 const &b) { return a._val OP b._val; } \
        template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr> \
        inline float3 operator OP (T const &a, float3 const &b) { return static_cast<float>(a) OP b._val; } \
        template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr> \
        inline float3 operator OP (float3 const &a, T const &b) { return a._val OP static_cast<float>(b); }
    ARITHMETIC(+)
    ARITHMETIC(-)
    ARITHMETIC(*)
    #undef ARITHMETIC
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type * = nullptr>
    inline float3 operator / (float3 const &a, T const &b) { return a / static_cast<float>(b); }

    inline bool operator==(float3 const &a, float3 const &b) {
        auto tmp = static_cast<__m128>(a - b) < EPSILON_F;
        return _mm_movemask_epi8(tmp) == 0xffff;
    }

    inline bool operator!=(float3 const &a, float3 const &b) { return !(a == b); }

    inline std::ostream& operator << (std::ostream& stream, float3 const &input) {
        stream << "(" << input.x() << ", " << input.y() << ", " << input.z() <<')';
        return stream;
    }

}
#endif //MATHEMATICS_SIMD_FLOAT3_HPP

#pragma clang diagnostic pop