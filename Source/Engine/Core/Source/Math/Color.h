#ifndef COLOR_HEADER
#define COLOR_HEADER
#include "../Containers/String.h"

namespace Faro {
    namespace Math {

        struct Color
        {
            union
            {
                float d[4];
                #pragma warning(suppress: 4201)
                struct { float r, g, b, a; };
            };

            Color(float, float, float, float);
            Color(float[4]);
            Color(float, float, float);
            Color();

            Color operator+ (Color&);
            Color operator+ (Color);
            Color operator- (Color&);
            Color operator- (Color);

            Color Lerp(Color&, float);

            // Predefined colors
            static Color Red;
            static Color Green;
            static Color Blue;
            static Color White;
            static Color Black;

            static Color FromHex(String);
        };

    }
}
#endif