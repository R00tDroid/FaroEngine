#ifndef UTIL_HEADER
#define UTIL_HEADER
#include "../Primitives.h"
#include <random>

namespace Faro
{
    namespace Math
    {
        // PI math
        extern float Pi();

        // default precision: 0
        extern void SetPIPrecision(int);

        template<class T>
        T DegToRad(T degrees) { return (degrees * (T)Pi()) / 180.0f; }

        template<class T>
        T RadToDeg(T radians) { return (radians * 180.0f / (T)Pi()); }

        template<class T>
        T Min(T a, T b)
        {
            return (a <= b) ? a : b;
        }

        template<class T>
        T Max(T a, T b)
        {
            return (a >= b) ? a : b;
        }

        template<class T>
        T Sin(T a)
        {
            return std::sin(a);
        }

        template<class T>
        T Cos(T a)
        {
            return std::cos(a);
        }

        template<class T>
        T Tan(T a)
        {
            return std::tan(a);
        }

        template<class T>
        T SinDeg(T a)
        {
            return std::sin(DegToRad(a));
        }

        template<class T>
        T CosDeg(T a)
        {
            return std::cos(DegToRad(a));
        }

        template<class T>
        T TanDeg(T a)
        {
            return std::tan(DegToRad(a));
        }

        template<class T>
        T ASin(T a)
        {
            return std::asin(a);
        }

        template<class T>
        T ACos(T a)
        {
            return std::acos(a);
        }

        template<class T>
        T ATan(T a)
        {
            return std::atan(a);
        }

        template<class T>
        T ASinDeg(T a)
        {
            return RadToDeg(std::asin(a));
        }

        template<class T>
        T ACosDeg(T a)
        {
            return RadToDeg(std::acos(a));
        }

        template<class T>
        T ATanDeg(T a)
        {
            return RadToDeg(std::atan(a));
        }

        template<class T, class AlphaType>
        T Lerp(T a, T b, AlphaType alpha)
        {
            return a + (b - a) * alpha;
        }

        // Random generation

        class RandomStream
        {
        public:
            RandomStream();

            void Seed(int seed);
            static int RandomSeed();

            template<class Type>
            Type Random(Type min_value, Type max_value)
            {
                std::uniform_real_distribution<double> dist(static_cast<double>(min_value), static_cast<double>(max_value));
                return dist(rng_);
            }

        private:
            std::mt19937 rng_;
        };

        extern RandomStream global_rng_stream_;

        template<class Type>
        struct RandomGenerator
        {
            static Type Random(Type min_value, Type max_value)
            {
                return global_rng_stream_.Random(min_value, max_value);
            }
        };

        typedef RandomGenerator<int32> RandomInt32;
        typedef RandomGenerator<float> RandomFloat;
        typedef RandomGenerator<int8> RandomInt8;
        typedef RandomGenerator<uint8> RandomUint8;
    }
}

#endif