/*-------------------------------------------------------------------------
RgbwColor provides a color object that can be directly consumed by NeoPixelBus

Written by Michael C. Miller.

I invest time and resources providing this open source code,
please support me by donating (see https://github.com/Makuna/NeoPixelBus)

-------------------------------------------------------------------------
This file is part of the Makuna/NeoPixelBus library.

NeoPixelBus is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

NeoPixelBus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with NeoPixel.  If not, see
<http://www.gnu.org/licenses/>.
-------------------------------------------------------------------------*/
#pragma once

struct RgbColor;
struct HslColor;
struct HsbColor;

struct RgbwwColor;
struct RgbwwwColor;
struct Rgbw64Color;
struct Rgbww80Color;

// ------------------------------------------------------------------------
// RgbwColor represents a color object that is represented by Red, Green, Blue
// component values and an extra White component.  It contains helpful color 
// routines to manipulate the color.
// ------------------------------------------------------------------------
struct RgbwColor : RgbColorBase
{
    typedef NeoRgbwCurrentSettings SettingsObject;

    // ------------------------------------------------------------------------
    // Construct a RgbwColor using R, G, B, W values (0-255)
    // ------------------------------------------------------------------------
    RgbwColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) :
        R(r), G(g), B(b), W(w)
    {
    };

    // ------------------------------------------------------------------------
    // Construct a RgbColor using a single brightness value (0-255)
    // This works well for creating gray tone colors
    // (0) = black, (255) = white, (128) = gray
    // ------------------------------------------------------------------------
    RgbwColor(uint8_t brightness) :
        R(0), G(0), B(0), W(brightness)
    {
    };

    // ------------------------------------------------------------------------
    // Construct a RgbwColor using RgbColor
    // ------------------------------------------------------------------------
    RgbwColor(const RgbColor& color) :
        R(color.R),
        G(color.G),
        B(color.B),
        W(0)
    {
    };

    // ------------------------------------------------------------------------
    // explicitly Construct a RgbColor using RgbwwColor
    // ------------------------------------------------------------------------
    explicit RgbwColor(const RgbwwColor& color);

    // ------------------------------------------------------------------------
    // explicitly Construct a RgbColor using RgbwwwColor
    // ------------------------------------------------------------------------
    explicit RgbwColor(const RgbwwwColor& color);

    // ------------------------------------------------------------------------
     // explicitly Construct a RgbColor using Rgbw64Color
     // ------------------------------------------------------------------------
    explicit RgbwColor(const Rgbw64Color& color);

    // ------------------------------------------------------------------------
    // explicitly Construct a RgbColor using Rgbww80Color
    // ------------------------------------------------------------------------
    explicit RgbwColor(const Rgbww80Color& color);

    // ------------------------------------------------------------------------
    // Construct a RgbwColor using HtmlColor
    // ------------------------------------------------------------------------
    RgbwColor(const HtmlColor& color);

    // ------------------------------------------------------------------------
    // Construct a RgbwColor using HslColor
    // ------------------------------------------------------------------------
    RgbwColor(const HslColor& color);

    // ------------------------------------------------------------------------
    // Construct a RgbwColor using HsbColor
    // ------------------------------------------------------------------------
    RgbwColor(const HsbColor& color);

    // ------------------------------------------------------------------------
    // Construct a RgbwColor that will have its values set in latter operations
    // CAUTION:  The R,G,B, W members are not initialized and may not be consistent
    // ------------------------------------------------------------------------
    RgbwColor()
    {
    };

    // ------------------------------------------------------------------------
    // Comparison operators
    // ------------------------------------------------------------------------
    bool operator==(const RgbwColor& other) const
    {
        return (R == other.R && G == other.G && B == other.B && W == other.W);
    };

    bool operator!=(const RgbwColor& other) const
    {
        return !(*this == other);
    };

    // ------------------------------------------------------------------------
    // CompareTo method
    // compares against another color with the given epsilon (delta allowed)
    // returns the greatest difference of a set of elements, 
    //   0 = equal within epsilon delta
    //   negative - this is less than other
    //   positive - this is greater than other
    // ------------------------------------------------------------------------
    int16_t CompareTo(const RgbwColor& other, uint8_t epsilon = 1)
    {
        return _Compare<RgbwColor, int16_t>(*this, other, epsilon);
    }

    // ------------------------------------------------------------------------
    // Compare method
    // compares two colors with the given epsilon (delta allowed)
    // returns the greatest difference of a set of elements, 
    //   0 = equal within epsilon delta
    //   negative - left is less than right
    //   positive - left is greater than right
    // ------------------------------------------------------------------------
    static int16_t Compare(const RgbwColor& left, const RgbwColor& right, uint8_t epsilon = 1)
    {
        return _Compare<RgbwColor, int16_t>(left, right, epsilon);
    }

    // ------------------------------------------------------------------------
    // operator [] - readonly
    // access elements in order by index rather than R,G,B
    // see static Count for the number of elements
    // ------------------------------------------------------------------------
    uint8_t operator[](size_t idx) const
    {
        switch (idx)
        {
        case 0:
            return R;
        case 1:
            return G;
        case 2:
            return B;
        default:
            return W;
        }
    }

    // ------------------------------------------------------------------------
    // operator [] - read write
    // access elements in order by index rather than R,G,B
    // see static Count for the number of elements
    // ------------------------------------------------------------------------
    uint8_t& operator[](size_t idx)
    {
        switch (idx)
        {
        case 0:
            return R;
        case 1:
            return G;
        case 2:
            return B;
        default:
            return W;
        }
    }

    // ------------------------------------------------------------------------
    // Returns if the color is grey, all values are equal other than white
    // ------------------------------------------------------------------------
    bool IsMonotone() const
    {
        return (R == B && R == G);
    };

    // ------------------------------------------------------------------------
    // Returns if the color components are all zero, the white component maybe 
    // anything
    // ------------------------------------------------------------------------
    bool IsColorLess() const
    {
        return (R == 0 && B == 0 && G == 0);
    };

    // ------------------------------------------------------------------------
    // CalculateBrightness will calculate the overall brightness
    // NOTE: This is a simple linear brightness
    // ------------------------------------------------------------------------
    uint8_t CalculateBrightness() const;

    // ------------------------------------------------------------------------
    // Dim will return a new color that is blended to black with the given ratio
    // ratio - (0-255) where 255 will return the original color and 0 will return black
    // 
    // NOTE: This is a simple linear blend
    // ------------------------------------------------------------------------
    RgbwColor Dim(uint8_t ratio) const;

    // ------------------------------------------------------------------------
    // Brighten will return a new color that is blended to white with the given ratio
    // ratio - (0-255) where 255 will return the original color and 0 will return white
    // 
    // NOTE: This is a simple linear blend
    // ------------------------------------------------------------------------
    RgbwColor Brighten(uint8_t ratio) const;

    // ------------------------------------------------------------------------
    // Darken will adjust the color by the given delta toward black
    // NOTE: This is a simple linear change
    // delta - (0-255) the amount to dim the color
    // ------------------------------------------------------------------------
    void Darken(uint8_t delta);

    // ------------------------------------------------------------------------
    // Lighten will adjust the color by the given delta toward white
    // NOTE: This is a simple linear change
    // delta - (0-255) the amount to lighten the color
    // ------------------------------------------------------------------------
    void Lighten(uint8_t delta);

    // ------------------------------------------------------------------------
    // LinearBlend between two colors by the amount defined by progress variable
    // left - the color to start the blend at
    // right - the color to end the blend at
    // progress - (0.0 - 1.0) value where 0 will return left and 1.0 will return right
    //     and a value between will blend the color weighted linearly between them
    // ------------------------------------------------------------------------
    static RgbwColor LinearBlend(const RgbwColor& left, const RgbwColor& right, float progress);
    // progress - (0 - 255) value where 0 will return left and 255 will return right
    //     and a value between will blend the color weighted linearly between them
    // ------------------------------------------------------------------------
    static RgbwColor LinearBlend(const RgbwColor& left, const RgbwColor& right, uint8_t progress);

    // ------------------------------------------------------------------------
    // BilinearBlend between four colors by the amount defined by 2d variable
    // c00 - upper left quadrant color
    // c01 - upper right quadrant color
    // c10 - lower left quadrant color
    // c11 - lower right quadrant color
    // x - unit value (0.0 - 1.0) that defines the blend progress in horizontal space
    // y - unit value (0.0 - 1.0) that defines the blend progress in vertical space
    // ------------------------------------------------------------------------
    static RgbwColor BilinearBlend(const RgbwColor& c00, 
        const RgbwColor& c01, 
        const RgbwColor& c10, 
        const RgbwColor& c11, 
        float x, 
        float y);

    uint16_t CalcTotalTenthMilliAmpere(const SettingsObject& settings)
    {
        auto total = 0;

        total += R * settings.RedTenthMilliAmpere / Max;
        total += G * settings.GreenTenthMilliAmpere / Max;
        total += B * settings.BlueTenthMilliAmpere / Max;
        total += W * settings.WhiteTenthMilliAmpere / Max;

        return total;
    }

    // ------------------------------------------------------------------------
    // Red, Green, Blue, White color members (0-255) where 
    // (0,0,0,0) is black and (255,255,255, 0) and (0,0,0,255) is white
    // Note (255,255,255,255) is extreme bright white
    // ------------------------------------------------------------------------
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t W;

    const static uint8_t Max = 255;
    const static size_t Count = 4; // four elements in []

private:
    inline static uint8_t _elementDim(uint8_t value, uint8_t ratio)
    {
        return (static_cast<uint16_t>(value) * (static_cast<uint16_t>(ratio) + 1)) >> 8;
    }

    inline static uint8_t _elementBrighten(uint8_t value, uint8_t ratio)
    {
        uint16_t element = ((static_cast<uint16_t>(value) + 1) << 8) / (static_cast<uint16_t>(ratio) + 1);

        if (element > Max)
        {
            element = Max;
        }
        else
        {
            element -= 1;
        }
        return element;
    }
};

