#ifndef MATHEMATICS_SIMD_FLOAT4_HPP
#define MATHEMATICS_SIMD_FLOAT4_HPP

#include <immintrin.h>
#include "constants.hpp"
#include "float2.hpp"
#include "float3.hpp"


namespace mathsimd {

    struct float4 {
    private:
        alignas(16) float _val[4]{0.f, 0.f, 0.f, 0.f};
    public:
        float4() = default;
        float4(float const &x, float const &y, float const &z, float const &w) : _val{x, y, z, w} {}
        float4(float const &x, float3 const &yzw) : _val{x, yzw.x(), yzw.y(), yzw.z()} {}
        float4(float3 const &xyz, float const &w) : _val{xyz.x(), xyz.y(), xyz.z(), w} {}
        float4(float const &x, float const &y, float2 const &zw) : _val{x, y, zw.x(), zw.y()} {}
        float4(float2 const &xy, float const &z, float const &w) : _val{xy.x(), xy.y(), z, w} {}
        float4(float const &x, float2 const &yz, float const &w) : _val{x, yz.x(), yz.y(), w} {}
        float4(float4 const &other) { _mm_store_ps(_val, _mm_load_ps(other._val)); }
        float4(float const* other) { _mm_storeu_ps(_val, _mm_loadu_ps(other));}
        float4(__m128 const &other) { _mm_store_ps(_val, other); }
        inline operator float const*() const { return _val; }
        inline operator __m128() const { return _mm_load_ps(_val); }
        inline float4 &operator=(float4 const &other) = default;
        inline float4 &operator=(__m128 const &other) { _mm_store_ps(_val, other); return *this; }
        float &x() { return _val[0]; }
        float &y() { return _val[1]; }
        float &z() { return _val[2]; }
        float &w() { return _val[3]; }
        [[nodiscard]] float x() const { return _val[0]; }
        [[nodiscard]] float y() const { return _val[1]; }
        [[nodiscard]] float z() const { return _val[2]; }
        [[nodiscard]] float w() const { return _val[3]; }

        #define ARITHMETIC(OP) \
        friend float4 operator OP (float4 const &a, float4 const &b); \
        friend float4 operator OP (float const &a, float4 const &b); \
        friend float4 operator OP (float4 const &a, float const &b);
        ARITHMETIC(+)
        ARITHMETIC(-)
        ARITHMETIC(*)
        ARITHMETIC(/)
        #undef ARITHMETIC

        inline float4 sign() const {
            __m128 zero = _mm_setzero_ps();
            __m128 val = _mm_load_ps(_val);
            __m128 positive = _mm_and_ps(_mm_cmpgt_ps(val, zero), _mm_set1_ps(1.0f));
            __m128 negative = _mm_and_ps(_mm_cmplt_ps(val, zero), _mm_set1_ps(-1.0f));

            return _mm_or_ps(positive, negative);
        }

        friend float dot(float4 const &a, float4 const &b);

        [[nodiscard]] inline float sqrMagnitude() const { return dot(*this, *this); }
        [[nodiscard]] inline float magnitude() const { 
            auto f = sqrMagnitude(); 
            auto v = _mm_load_ss(&f);
            _mm_store_ss(&f, _mm_mul_ss(v, _mm_rsqrt_ss(v)));
            return f;
        }
        [[nodiscard]] inline float4 normalized() const {
            auto const f = sqrMagnitude();
            return _mm_mul_ps( *this, _mm_rsqrt_ps(_mm_load_ps1(&f)) );
        }

        friend float4 cross(float4 const &a, float4 const &b);

        #define FUNC(NAME,X,Y,Z,W) \
        static inline float4 NAME () { return {X,Y,Z,W}; }
        FUNC(up, 0,1,0,0)
        FUNC(down, 0,-1,0,0)
        FUNC(right, 1,0,0,0)
        FUNC(left, -1,0,0,0)
        FUNC(forward, 0,0,1,0)
        FUNC(back, 0,0,-1,0)
        FUNC(in, 0,0,0,1)
        FUNC(out, 0,0,0,-1)
        FUNC(one, 1,1,1,1)
        FUNC(zero, 0,0,0,0)
        FUNC(origin, 0,0,0,1)
        #undef FUNC
    };
}
#endif //MATHEMATICS_SIMD_FLOAT4_HPP