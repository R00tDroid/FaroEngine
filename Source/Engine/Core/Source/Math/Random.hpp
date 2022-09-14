#pragma once
#include "../Primitives.hpp"
#include <random>

namespace Faro
{
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
    typedef RandomGenerator<uint8> RandomUInt8;
}
