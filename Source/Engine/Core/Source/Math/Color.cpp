#include "Color.h"

namespace Faro 
{
	namespace Math
	{

		Color::Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a)
		{

		}

		/*----------------------------------------------------------------------------------*/

		Color::Color(float r, float g, float b) : r(r), g(g), b(b), a(1)
		{

		}

		/*----------------------------------------------------------------------------------*/

		Color::Color(float c[4])
		{
			d[0] = c[0];
			d[1] = c[1];
			d[2] = c[2];
			d[3] = c[3];
		}

		/*----------------------------------------------------------------------------------*/

		Color::Color()
		{
			d[0] = 0;
			d[1] = 0;
			d[2] = 0;
			d[3] = 1.0f;
		}

		/*----------------------------------------------------------------------------------*/

		Color Color::operator+(Color& other)
		{
			return Color(r + other.r, g + other.g, b + other.b, a + other.a);
		}

		/*----------------------------------------------------------------------------------*/

		Color Color::operator+(Color other)
		{
			return Color(r + other.r, g + other.g, b + other.b, a + other.a);
		}

		/*----------------------------------------------------------------------------------*/

		Color Color::operator-(Color& other)
		{
			return Color(r - other.r, g - other.g, b - other.b, a - other.a);
		}

		/*----------------------------------------------------------------------------------*/

		Color Color::operator-(Color other)
		{
			return Color(r - other.r, g - other.g, b - other.b, a - other.a);
		}

		/*----------------------------------------------------------------------------------*/

		Color Color::Lerp(Color& other, float time)
		{
			return Color((1 - time)*r + other.r*time, (1 - time)*g + other.g*time, (1 - time)*b + other.b*time, (1 - time)*a + other.a*time);
		}

#ifdef _MSC_VER 
#define SSCANF sscanf_s
#else
#define SSCANF sscanf
#endif

		Color Color::FromHex(String hex)
		{
			String value = hex;
			if (value[0] == '#') value.Erase(0);
			int r = 255, g = 255, b = 255, a = 255;

			if (value.Length() == 6) {
				SSCANF(*value, "%02x%02x%02x", &r, &g, &b);
			}
			else if (value.Length() == 8) {
				SSCANF(*value, "%02x%02x%02x%02x", &r, &g, &b, &a);
			}

			return{ r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f };
		}


		Color Color::Red = { 1, 0, 0, 1 };
		Color Color::Green = { 0, 1, 0, 1 };
		Color Color::Blue = { 0, 0, 1, 1 };
		Color Color::White = { 1, 1, 1, 1 };
		Color Color::Black = { 0, 0, 0, 1 };
	}
}
