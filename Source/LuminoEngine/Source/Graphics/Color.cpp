﻿
#include "../Internal.h"
#include <Lumino/Math/Vector3.h>
#include <Lumino/Math/Vector4.h>
#include <Lumino/Graphics/Color.h>

LN_NAMESPACE_BEGIN
LN_NAMESPACE_GRAPHICS_BEGIN

//==============================================================================
// Color32
//==============================================================================
const Color32 Color32::Transparency = Color32(0, 0, 0, 0);
const Color32 Color32::TransparencyWhite = Color32(255, 255, 255, 0);
//const const Color32 Color32::Black = Color32(0, 0, 0, 255);
//const const Color32 Color32::White = Color32(255, 255, 255, 255);
//const const Color32 Color32::Gray = Color32(127, 127, 127, 255);
//const const Color32 Color32::Red = Color32(255, 0, 0, 255);
//const const Color32 Color32::Green = Color32(0, 255, 0, 255);
//const const Color32 Color32::Blue = Color32(0, 0, 255, 255);
const Color32 Color32::AliceBlue(240, 248, 255);
const Color32 Color32::AntiqueWhite(250, 235, 215);
const Color32 Color32::Aqua(0, 255, 255);
const Color32 Color32::Aquamarine(127, 255, 212);
const Color32 Color32::Azure(240, 255, 255);
const Color32 Color32::Beige(245, 245, 220);
const Color32 Color32::Bisque(255, 228, 196);
const Color32 Color32::Black(0, 0, 0);
const Color32 Color32::BlanchedAlmond(255, 235, 205);
const Color32 Color32::Blue(0, 0, 255);
const Color32 Color32::BlueViolet(138, 43, 226);
const Color32 Color32::Brown(165, 42, 42);
const Color32 Color32::BurlyWood(222, 184, 135);
const Color32 Color32::CadetBlue(95, 158, 160);
const Color32 Color32::Chartreuse(127, 255, 0);
const Color32 Color32::Chocolate(210, 105, 30);
const Color32 Color32::Coral(255, 127, 80);
const Color32 Color32::CornflowerBlue(100, 149, 237);
const Color32 Color32::Cornsilk(255, 248, 220);
const Color32 Color32::Crimson(220, 20, 60);
const Color32 Color32::Cyan(0, 255, 255);
const Color32 Color32::DarkBlue(0, 0, 139);
const Color32 Color32::DarkCyan(0, 139, 139);
const Color32 Color32::DarkGoldenrod(184, 134, 11);
const Color32 Color32::DarkGray(169, 169, 169);
const Color32 Color32::DarkGreen(0, 100, 0);
const Color32 Color32::DarkKhaki(189, 183, 107);
const Color32 Color32::DarkMagenta(139, 0, 139);
const Color32 Color32::DarkOliveGreen(85, 107, 47);
const Color32 Color32::DarkOrange(255, 140, 0);
const Color32 Color32::DarkOrchid(153, 50, 204);
const Color32 Color32::DarkRed(139, 0, 0);
const Color32 Color32::DarkSalmon(233, 150, 122);
const Color32 Color32::DarkSeaGreen(143, 188, 143);
const Color32 Color32::DarkSlateBlue(72, 61, 139);
const Color32 Color32::DarkSlateGray(47, 79, 79);
const Color32 Color32::DarkTurquoise(0, 206, 209);
const Color32 Color32::DarkViolet(148, 0, 211);
const Color32 Color32::DeepPink(255, 20, 147);
const Color32 Color32::DeepSkyBlue(0, 191, 255);
const Color32 Color32::DimGray(105, 105, 105);
const Color32 Color32::DodgerBlue(30, 144, 255);
const Color32 Color32::Firebrick(178, 34, 34);
const Color32 Color32::FloralWhite(255, 250, 240);
const Color32 Color32::ForestGreen(34, 139, 34);
const Color32 Color32::Fuchsia(255, 0, 255);
const Color32 Color32::Gainsboro(220, 220, 220);
const Color32 Color32::GhostWhite(248, 248, 255);
const Color32 Color32::Gold(255, 215, 0);
const Color32 Color32::Goldenrod(218, 165, 32);
const Color32 Color32::Gray(128, 128, 128);
const Color32 Color32::Green(0, 128, 0);
const Color32 Color32::GreenYellow(173, 255, 47);
const Color32 Color32::Honeydew(240, 255, 240);
const Color32 Color32::HotPink(255, 105, 180);
const Color32 Color32::IndianRed(205, 92, 92);
const Color32 Color32::Indigo(75, 0, 130);
const Color32 Color32::Ivory(255, 255, 240);
const Color32 Color32::Khaki(240, 230, 140);
const Color32 Color32::Lavender(230, 230, 250);
const Color32 Color32::LavenderBlush(255, 240, 245);
const Color32 Color32::LawnGreen(124, 252, 0);
const Color32 Color32::LemonChiffon(255, 250, 205);
const Color32 Color32::LightBlue(173, 216, 230);
const Color32 Color32::LightCoral(240, 128, 128);
const Color32 Color32::LightCyan(224, 255, 255);
const Color32 Color32::LightGoldenrodYellow(250, 250, 210);
const Color32 Color32::LightGray(211, 211, 211);
const Color32 Color32::LightGreen(144, 238, 144);
const Color32 Color32::LightPink(255, 182, 193);
const Color32 Color32::LightSalmon(255, 160, 122);
const Color32 Color32::LightSeaGreen(32, 178, 170);
const Color32 Color32::LightSkyBlue(135, 206, 250);
const Color32 Color32::LightSlateGray(119, 136, 153);
const Color32 Color32::LightSteelBlue(176, 196, 222);
const Color32 Color32::LightYellow(255, 255, 224);
const Color32 Color32::Lime(0, 255, 0);
const Color32 Color32::LimeGreen(50, 205, 50);
const Color32 Color32::Linen(250, 240, 230);
const Color32 Color32::Magenta(255, 0, 255);
const Color32 Color32::Maroon(128, 0, 0);
const Color32 Color32::MediumAquamarine(102, 205, 170);
const Color32 Color32::MediumBlue(0, 0, 205);
const Color32 Color32::MediumOrchid(186, 85, 211);
const Color32 Color32::MediumPurple(147, 112, 219);
const Color32 Color32::MediumSeaGreen(60, 179, 113);
const Color32 Color32::MediumSlateBlue(123, 104, 238);
const Color32 Color32::MediumSpringGreen(0, 250, 154);
const Color32 Color32::MediumTurquoise(72, 209, 204);
const Color32 Color32::MediumVioletRed(199, 21, 133);
const Color32 Color32::MidnightBlue(25, 25, 112);
const Color32 Color32::MintCream(245, 255, 250);
const Color32 Color32::MistyRose(255, 228, 225);
const Color32 Color32::Moccasin(255, 228, 181);
const Color32 Color32::NavajoWhite(255, 222, 173);
const Color32 Color32::Navy(0, 0, 128);
const Color32 Color32::OldLace(253, 245, 230);
const Color32 Color32::Olive(128, 128, 0);
const Color32 Color32::OliveDrab(107, 142, 35);
const Color32 Color32::Orange(255, 165, 0);
const Color32 Color32::OrangeRed(255, 69, 0);
const Color32 Color32::Orchid(218, 112, 214);
const Color32 Color32::PaleGoldenrod(238, 232, 170);
const Color32 Color32::PaleGreen(152, 251, 152);
const Color32 Color32::PaleTurquoise(175, 238, 238);
const Color32 Color32::PaleVioletRed(219, 112, 147);
const Color32 Color32::PapayaWhip(255, 239, 213);
const Color32 Color32::PeachPuff(255, 218, 185);
const Color32 Color32::Peru(205, 133, 63);
const Color32 Color32::Pink(255, 192, 203);
const Color32 Color32::Plum(221, 160, 221);
const Color32 Color32::PowderBlue(176, 224, 230);
const Color32 Color32::Purple(128, 0, 128);
const Color32 Color32::Red(255, 0, 0);
const Color32 Color32::RosyBrown(188, 143, 143);
const Color32 Color32::RoyalBlue(65, 105, 225);
const Color32 Color32::SaddleBrown(139, 69, 19);
const Color32 Color32::Salmon(250, 128, 114);
const Color32 Color32::SandyBrown(244, 164, 96);
const Color32 Color32::SeaGreen(46, 139, 87);
const Color32 Color32::SeaShell(255, 245, 238);
const Color32 Color32::Sienna(160, 82, 45);
const Color32 Color32::Silver(192, 192, 192);
const Color32 Color32::SkyBlue(135, 206, 235);
const Color32 Color32::SlateBlue(106, 90, 205);
const Color32 Color32::SlateGray(112, 128, 144);
const Color32 Color32::Snow(255, 250, 250);
const Color32 Color32::SpringGreen(0, 255, 127);
const Color32 Color32::SteelBlue(70, 130, 180);
const Color32 Color32::Tan(210, 180, 140);
const Color32 Color32::Teal(0, 128, 128);
const Color32 Color32::Thistle(216, 191, 216);
const Color32 Color32::Tomato(255, 99, 71);
const Color32 Color32::Transparent(255, 255, 255);
const Color32 Color32::Turquoise(64, 224, 208);
const Color32 Color32::Violet(238, 130, 238);
const Color32 Color32::Wheat(245, 222, 179);
const Color32 Color32::White(255, 255, 255);
const Color32 Color32::WhiteSmoke(245, 245, 245);
const Color32 Color32::Yellow(255, 255, 0);
const Color32 Color32::YellowGreen(154, 205, 50);

//------------------------------------------------------------------------------
//Color32::Color32(const Color& colorF)
//{
//	r = static_cast<uint8_t>(colorF.r * 255);
//	g = static_cast<uint8_t>(colorF.g * 255);
//	b = static_cast<uint8_t>(colorF.b * 255);
//	a = static_cast<uint8_t>(colorF.a * 255);
//}

static bool TryParseHexChar(int c, int* outValue)
{
	const int zeroChar = (int)'0';
	const int aLower = (int)'a';
	const int aUpper = (int)'A';

	int intChar = (int)c;

	if ((intChar >= zeroChar) && (intChar <= (zeroChar + 9)))
	{
		*outValue = (intChar - zeroChar);
		return true;
	}

	if ((intChar >= aLower) && (intChar <= (aLower + 5)))
	{
		*outValue = (intChar - aLower + 10);
		return true;
	}

	if ((intChar >= aUpper) && (intChar <= (aUpper + 5)))
	{
		*outValue = (intChar - aUpper + 10);
		return true;
	}
	return false;
}
static int ParseHexChar(int c)
{
	int value;
	if (TryParseHexChar(c, &value))
	{
		return value;
	}
	LN_THROW(0, InvalidFormatException);
}

static bool ParseHexColor(const StringRef& str, Color32* outColor)
{
	uint8_t r, g, b, a;
	if (str.getLength() >= 3 && str[0] == _T('#'))
	{
		// #FFFFFFFF
		if (str.getLength() >= 9)
		{
			a = ParseHexChar(str[1]) * 16 + ParseHexChar(str[2]);
			r = ParseHexChar(str[3]) * 16 + ParseHexChar(str[4]);
			g = ParseHexChar(str[5]) * 16 + ParseHexChar(str[6]);
			b = ParseHexChar(str[7]) * 16 + ParseHexChar(str[8]);
		}
		// #FFFFFF
		else if (str.getLength() >= 7)
		{
			a = 255;
			r = ParseHexChar(str[1]) * 16 + ParseHexChar(str[2]);
			g = ParseHexChar(str[3]) * 16 + ParseHexChar(str[4]);
			b = ParseHexChar(str[5]) * 16 + ParseHexChar(str[6]);
		}
		// #FFFF
		else if (str.getLength() >= 5)
		{
			a = ParseHexChar(str[1]);
			a = a + a * 16;
			r = ParseHexChar(str[2]);
			r = r + r * 16;
			g = ParseHexChar(str[3]);
			g = g + g * 16;
			b = ParseHexChar(str[4]);
			b = b + b * 16;
		}
        // #FFF
		else if (str.getLength() >= 4)
		{
			r = ParseHexChar(str[1]);
			r = r + r * 16;
			g = ParseHexChar(str[2]);
			g = g + g * 16;
			b = ParseHexChar(str[3]);
			b = b + b * 16;
            a = 255;
		}
		else
		{
			return false;
		}

		outColor->set(r, g, b, a);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
Color32 Color32::fromString(const StringRef& str)
{
	Color32 c;
	if (!ParseHexColor(str, &c))
	{
		LN_THROW(0, InvalidFormatException);
	}
	return c;
}

//==============================================================================
// Color
//==============================================================================
const Color Color::Transparency(0.0f, 0.0f, 0.0f, 0.0f);
const Color Color::TransparencyWhite(1.0f, 1.0f, 1.0f, 0.0f);
const Color Color::AliceBlue(0.941176f, 0.972549f, 1.0f);
const Color Color::AntiqueWhite(0.980392f, 0.921569f, 0.843137f);
const Color Color::Aqua(0.0f, 1.0f, 1.0f);
const Color Color::Aquamarine(0.498039f, 1.0f, 0.831373f);
const Color Color::Azure(0.941176f, 1.0f, 1.0f);
const Color Color::Beige(0.960784f, 0.960784f, 0.862745f);
const Color Color::Bisque(1.0f, 0.894118f, 0.768627f);
const Color Color::Black(0.0f, 0.0f, 0.0f);
const Color Color::BlanchedAlmond(1.0f, 0.921569f, 0.803922f);
const Color Color::Blue(0.0f, 0.0f, 1.0f);
const Color Color::BlueViolet(0.541176f, 0.168627f, 0.886275f);
const Color Color::Brown(0.647059f, 0.164706f, 0.164706f);
const Color Color::BurlyWood(0.870588f, 0.721569f, 0.529412f);
const Color Color::CadetBlue(0.372549f, 0.619608f, 0.627451f);
const Color Color::Chartreuse(0.498039f, 1.0f, 0.0f);
const Color Color::Chocolate(0.823529f, 0.411765f, 0.117647f);
const Color Color::Coral(1.0f, 0.498039f, 0.313725f);
const Color Color::CornflowerBlue(0.392157f, 0.584314f, 0.929412f);
const Color Color::Cornsilk(1.0f, 0.972549f, 0.862745f);
const Color Color::Crimson(0.862745f, 0.0784314f, 0.235294f);
const Color Color::Cyan(0.0f, 1.0f, 1.0f);
const Color Color::DarkBlue(0.0f, 0.0f, 0.545098f);
const Color Color::DarkCyan(0.0f, 0.545098f, 0.545098f);
const Color Color::DarkGoldenrod(0.721569f, 0.52549f, 0.0431373f);
const Color Color::DarkGray(0.662745f, 0.662745f, 0.662745f);
const Color Color::DarkGreen(0.0f, 0.392157f, 0.0f);
const Color Color::DarkKhaki(0.741176f, 0.717647f, 0.419608f);
const Color Color::DarkMagenta(0.545098f, 0.0f, 0.545098f);
const Color Color::DarkOliveGreen(0.333333f, 0.419608f, 0.184314f);
const Color Color::DarkOrange(1.0f, 0.54902f, 0.0f);
const Color Color::DarkOrchid(0.6f, 0.196078f, 0.8f);
const Color Color::DarkRed(0.545098f, 0.0f, 0.0f);
const Color Color::DarkSalmon(0.913725f, 0.588235f, 0.478431f);
const Color Color::DarkSeaGreen(0.560784f, 0.737255f, 0.560784f);
const Color Color::DarkSlateBlue(0.282353f, 0.239216f, 0.545098f);
const Color Color::DarkSlateGray(0.184314f, 0.309804f, 0.309804f);
const Color Color::DarkTurquoise(0.0f, 0.807843f, 0.819608f);
const Color Color::DarkViolet(0.580392f, 0.0f, 0.827451f);
const Color Color::DeepPink(1.0f, 0.0784314f, 0.576471f);
const Color Color::DeepSkyBlue(0.0f, 0.74902f, 1.0f);
const Color Color::DimGray(0.411765f, 0.411765f, 0.411765f);
const Color Color::DodgerBlue(0.117647f, 0.564706f, 1.0f);
const Color Color::Firebrick(0.698039f, 0.133333f, 0.133333f);
const Color Color::FloralWhite(1.0f, 0.980392f, 0.941176f);
const Color Color::ForestGreen(0.133333f, 0.545098f, 0.133333f);
const Color Color::Fuchsia(1.0f, 0.0f, 1.0f);
const Color Color::Gainsboro(0.862745f, 0.862745f, 0.862745f);
const Color Color::GhostWhite(0.972549f, 0.972549f, 1.0f);
const Color Color::Gold(1.0f, 0.843137f, 0.0f);
const Color Color::Goldenrod(0.854902f, 0.647059f, 0.12549f);
const Color Color::Gray(0.5f, 0.5f, 0.5f);
const Color Color::Green(0.0f, 0.5f, 0.0f);
const Color Color::GreenYellow(0.678431f, 1.0f, 0.184314f);
const Color Color::Honeydew(0.941176f, 1.0f, 0.941176f);
const Color Color::HotPink(1.0f, 0.411765f, 0.705882f);
const Color Color::IndianRed(0.803922f, 0.360784f, 0.360784f);
const Color Color::Indigo(0.294118f, 0.0f, 0.509804f);
const Color Color::Ivory(1.0f, 1.0f, 0.941176f);
const Color Color::Khaki(0.941176f, 0.901961f, 0.54902f);
const Color Color::Lavender(0.901961f, 0.901961f, 0.980392f);
const Color Color::LavenderBlush(1.0f, 0.941176f, 0.960784f);
const Color Color::LawnGreen(0.486275f, 0.988235f, 0.0f);
const Color Color::LemonChiffon(1.0f, 0.980392f, 0.803922f);
const Color Color::LightBlue(0.678431f, 0.847059f, 0.901961f);
const Color Color::LightCoral(0.941176f, 0.5f, 0.5f);
const Color Color::LightCyan(0.878431f, 1.0f, 1.0f);
const Color Color::LightGoldenrodYellow(0.980392f, 0.980392f, 0.823529f);
const Color Color::LightGray(0.827451f, 0.827451f, 0.827451f);
const Color Color::LightGreen(0.564706f, 0.933333f, 0.564706f);
const Color Color::LightPink(1.0f, 0.713725f, 0.756863f);
const Color Color::LightSalmon(1.0f, 0.627451f, 0.478431f);
const Color Color::LightSeaGreen(0.12549f, 0.698039f, 0.666667f);
const Color Color::LightSkyBlue(0.529412f, 0.807843f, 0.980392f);
const Color Color::LightSlateGray(0.466667f, 0.533333f, 0.6f);
const Color Color::LightSteelBlue(0.690196f, 0.768627f, 0.870588f);
const Color Color::LightYellow(1.0f, 1.0f, 0.878431f);
const Color Color::Lime(0.0f, 1.0f, 0.0f);
const Color Color::LimeGreen(0.196078f, 0.803922f, 0.196078f);
const Color Color::Linen(0.980392f, 0.941176f, 0.901961f);
const Color Color::Magenta(1.0f, 0.0f, 1.0f);
const Color Color::Maroon(0.5f, 0.0f, 0.0f);
const Color Color::MediumAquamarine(0.4f, 0.803922f, 0.666667f);
const Color Color::MediumBlue(0.0f, 0.0f, 0.803922f);
const Color Color::MediumOrchid(0.729412f, 0.333333f, 0.827451f);
const Color Color::MediumPurple(0.576471f, 0.439216f, 0.858824f);
const Color Color::MediumSeaGreen(0.235294f, 0.701961f, 0.443137f);
const Color Color::MediumSlateBlue(0.482353f, 0.407843f, 0.933333f);
const Color Color::MediumSpringGreen(0.0f, 0.980392f, 0.603922f);
const Color Color::MediumTurquoise(0.282353f, 0.819608f, 0.8f);
const Color Color::MediumVioletRed(0.780392f, 0.0823529f, 0.521569f);
const Color Color::MidnightBlue(0.0980392f, 0.0980392f, 0.439216f);
const Color Color::MintCream(0.960784f, 1.0f, 0.980392f);
const Color Color::MistyRose(1.0f, 0.894118f, 0.882353f);
const Color Color::Moccasin(1.0f, 0.894118f, 0.709804f);
const Color Color::NavajoWhite(1.0f, 0.870588f, 0.678431f);
const Color Color::Navy(0.0f, 0.0f, 0.5f);
const Color Color::OldLace(0.992157f, 0.960784f, 0.901961f);
const Color Color::Olive(0.5f, 0.5f, 0.0f);
const Color Color::OliveDrab(0.419608f, 0.556863f, 0.137255f);
const Color Color::Orange(1.0f, 0.647059f, 0.0f);
const Color Color::OrangeRed(1.0f, 0.270588f, 0.0f);
const Color Color::Orchid(0.854902f, 0.439216f, 0.839216f);
const Color Color::PaleGoldenrod(0.933333f, 0.909804f, 0.666667f);
const Color Color::PaleGreen(0.596078f, 0.984314f, 0.596078f);
const Color Color::PaleTurquoise(0.686275f, 0.933333f, 0.933333f);
const Color Color::PaleVioletRed(0.858824f, 0.439216f, 0.576471f);
const Color Color::PapayaWhip(1.0f, 0.937255f, 0.835294f);
const Color Color::PeachPuff(1.0f, 0.854902f, 0.72549f);
const Color Color::Peru(0.803922f, 0.521569f, 0.247059f);
const Color Color::Pink(1.0f, 0.752941f, 0.796078f);
const Color Color::Plum(0.866667f, 0.627451f, 0.866667f);
const Color Color::PowderBlue(0.690196f, 0.878431f, 0.901961f);
const Color Color::Purple(0.5f, 0.0f, 0.5f);
const Color Color::Red(1.0f, 0.0f, 0.0f);
const Color Color::RosyBrown(0.737255f, 0.560784f, 0.560784f);
const Color Color::RoyalBlue(0.254902f, 0.411765f, 0.882353f);
const Color Color::SaddleBrown(0.545098f, 0.270588f, 0.0745098f);
const Color Color::Salmon(0.980392f, 0.5f, 0.447059f);
const Color Color::SandyBrown(0.956863f, 0.643137f, 0.376471f);
const Color Color::SeaGreen(0.180392f, 0.545098f, 0.341176f);
const Color Color::SeaShell(1.0f, 0.960784f, 0.933333f);
const Color Color::Sienna(0.627451f, 0.321569f, 0.176471f);
const Color Color::Silver(0.752941f, 0.752941f, 0.752941f);
const Color Color::SkyBlue(0.529412f, 0.807843f, 0.921569f);
const Color Color::SlateBlue(0.415686f, 0.352941f, 0.803922f);
const Color Color::SlateGray(0.439216f, 0.5f, 0.564706f);
const Color Color::Snow(1.0f, 0.980392f, 0.980392f);
const Color Color::SpringGreen(0.0f, 1.0f, 0.498039f);
const Color Color::SteelBlue(0.27451f, 0.509804f, 0.705882f);
const Color Color::Tan(0.823529f, 0.705882f, 0.54902f);
const Color Color::Teal(0.0f, 0.5f, 0.5f);
const Color Color::Thistle(0.847059f, 0.74902f, 0.847059f);
const Color Color::Tomato(1.0f, 0.388235f, 0.278431f);
const Color Color::Transparent(1.0f, 1.0f, 1.0f);
const Color Color::Turquoise(0.25098f, 0.878431f, 0.815686f);
const Color Color::Violet(0.933333f, 0.509804f, 0.933333f);
const Color Color::Wheat(0.960784f, 0.870588f, 0.701961f);
const Color Color::White(1.0f, 1.0f, 1.0f);
const Color Color::WhiteSmoke(0.960784f, 0.960784f, 0.960784f);
const Color Color::Yellow(1.0f, 1.0f, 0.0f);
const Color Color::YellowGreen(0.603922f, 0.803922f, 0.196078f);

static const float g_color255Table[256] =
{
	// [0]
	0, 0.00392157, 0.00784314, 0.0117647, 0.0156863, 0.0196078, 0.0235294, 0.027451, 0.0313725, 0.0352941, 0.0392157, 0.0431373, 0.0470588, 0.0509804, 0.054902, 0.0588235,
	0.0627451, 0.0666667, 0.0705882, 0.0745098, 0.0784314, 0.0823529, 0.0862745, 0.0901961, 0.0941176, 0.0980392, 0.101961, 0.105882, 0.109804, 0.113725, 0.117647, 0.121569,
	0.12549, 0.129412, 0.133333, 0.137255, 0.141176, 0.145098, 0.14902, 0.152941, 0.156863, 0.160784, 0.164706, 0.168627, 0.172549, 0.176471, 0.180392, 0.184314,
	0.188235, 0.192157, 0.196078, 0.2, 0.203922, 0.207843, 0.211765, 0.215686, 0.219608, 0.223529, 0.227451, 0.231373, 0.235294, 0.239216, 0.243137, 0.247059,
	0.25098, 0.254902, 0.258824, 0.262745, 0.266667, 0.270588, 0.27451, 0.278431, 0.282353, 0.286275, 0.290196, 0.294118, 0.298039, 0.301961, 0.305882, 0.309804,
	0.313725, 0.317647, 0.321569, 0.32549, 0.329412, 0.333333, 0.337255, 0.341176, 0.345098, 0.34902, 0.352941, 0.356863, 0.360784, 0.364706, 0.368627, 0.372549,
	0.376471, 0.380392, 0.384314, 0.388235, 0.392157, 0.396078, 0.4, 0.403922, 0.407843, 0.411765, 0.415686, 0.419608, 0.423529, 0.427451, 0.431373, 0.435294,
	0.439216, 0.443137, 0.447059, 0.45098, 0.454902, 0.458824, 0.462745, 0.466667, 0.470588, 0.47451, 0.478431, 0.482353, 0.486275, 0.490196, 0.494118, 0.498039,
	// [128]
	0.500000, 0.505882, 0.509804, 0.513725, 0.517647, 0.521569, 0.52549, 0.529412, 0.533333, 0.537255, 0.541176, 0.545098, 0.54902, 0.552941, 0.556863, 0.560784,
	0.564706, 0.568627, 0.572549, 0.576471, 0.580392, 0.584314, 0.588235, 0.592157, 0.596078, 0.6, 0.603922, 0.607843, 0.611765, 0.615686, 0.619608, 0.623529,
	0.627451, 0.631373, 0.635294, 0.639216, 0.643137, 0.647059, 0.65098, 0.654902, 0.658824, 0.662745, 0.666667, 0.670588, 0.67451, 0.678431, 0.682353, 0.686275,
	0.690196, 0.694118, 0.698039, 0.701961, 0.705882, 0.709804, 0.713725, 0.717647, 0.721569, 0.72549, 0.729412, 0.733333, 0.737255, 0.741176, 0.745098, 0.74902,
	0.752941, 0.756863, 0.760784, 0.764706, 0.768627, 0.772549, 0.776471, 0.780392, 0.784314, 0.788235, 0.792157, 0.796078, 0.8, 0.803922, 0.807843, 0.811765,
	0.815686, 0.819608, 0.823529, 0.827451, 0.831373, 0.835294, 0.839216, 0.843137, 0.847059, 0.85098, 0.854902, 0.858824, 0.862745, 0.866667, 0.870588, 0.87451,
	0.878431, 0.882353, 0.886275, 0.890196, 0.894118, 0.898039, 0.901961, 0.905882, 0.909804, 0.913725, 0.917647, 0.921569, 0.92549, 0.929412, 0.933333, 0.937255,
	0.941176, 0.945098, 0.94902, 0.952941, 0.956863, 0.960784, 0.964706, 0.968627, 0.972549, 0.976471, 0.980392, 0.984314, 0.988235, 0.992157, 0.996078, 1,
	// [256]
};

//------------------------------------------------------------------------------
Color::Color(const Color32& color)
{
	r = g_color255Table[color.r];
	g = g_color255Table[color.g];
	b = g_color255Table[color.b];
	a = g_color255Table[color.a];
}
//------------------------------------------------------------------------------
Color::Color(const Vector3& vec, float a_)
{
	r = vec.x; g = vec.y; b = vec.z; a = a_;
}

//------------------------------------------------------------------------------
Color::Color(const Vector4& vec)
{
	r = vec.x; g = vec.y; b = vec.z; a = vec.w;
}

//------------------------------------------------------------------------------
void Color::addClamp(const Color& color)
{
	r = Math::clamp(r + color.r, 0.0f, 1.0f);
	g = Math::clamp(g + color.g, 0.0f, 1.0f);
	b = Math::clamp(b + color.b, 0.0f, 1.0f);
	a = Math::clamp(a + color.a, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
void Color::multiplyClamp(const Color& color)
{
	r = Math::clamp(r * color.r, 0.0f, 1.0f);
	g = Math::clamp(g * color.g, 0.0f, 1.0f);
	b = Math::clamp(b * color.b, 0.0f, 1.0f);
	a = Math::clamp(a * color.a, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
Color32 Color::to32BitColor() const
{
	return Color32(
		static_cast<uint8_t>(r * 255),
		static_cast<uint8_t>(g * 255),
		static_cast<uint8_t>(b * 255),
		static_cast<uint8_t>(a * 255));
}

//------------------------------------------------------------------------------
Color Color::fromString(const StringRef& str)
{
	Color32 c = Color32::fromString(str);
	return Color(c);
}

//------------------------------------------------------------------------------
// static
//------------------------------------------------------------------------------
Color Color::lerp(const Color& color1, const Color& color2, float t)
{
	return Color(
		Math::lerp(color1.r, color2.r, t),
		Math::lerp(color1.g, color2.g, t),
		Math::lerp(color1.b, color2.b, t),
		Math::lerp(color1.a, color2.a, t));
}

//==============================================================================
// ToneF
//==============================================================================

const ToneF ToneF::Zero(0, 0, 0, 0);

//------------------------------------------------------------------------------
void ToneF::addClamp(const ToneF& tone)
{
	r = Math::clamp(r + tone.r, 0.0f, 1.0f);
	g = Math::clamp(g + tone.g, 0.0f, 1.0f);
	b = Math::clamp(b + tone.b, 0.0f, 1.0f);
	gray = Math::clamp(gray + tone.gray, 0.0f, 1.0f);
}


//==============================================================================
// HSVColor
//==============================================================================

//------------------------------------------------------------------------------
Color32 HSVColor::toColor() const
{
	unsigned char c1, c2, c3;
	if (S == 0)
	{
		return Color32(V, V, V, a);
	}
	else
	{
		int t = (H * 6) % 360;
		c1 = (V*(255 - S)) / 255;
		c2 = (V*(255 - (S*t) / 360)) / 255;
		c3 = (V*(255 - (S*(360 - t)) / 360)) / 255;
		switch (H / 60)
		{
		case 0: return Color32(V, c3, c1, a);
		case 1: return Color32(c2, V, c1, a);
		case 2: return Color32(c1, V, c3, a);
		case 3: return Color32(c1, c2, V, a);
		case 4: return Color32(c3, c1, V, a);
		case 5: return Color32(V, c1, c2, a);
		}
	}
	return Color32(0, 0, 0, a);
}

//------------------------------------------------------------------------------
Color HSVColor::toColorF() const
{
	Color32 c = toColor();
	return Color(c);
}


LN_NAMESPACE_GRAPHICS_END
LN_NAMESPACE_END
