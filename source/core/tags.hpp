/**
 * @file tags.hpp
 * @brief Holds structs used as tags for targeted overloading of function & function-like structures
 */

/**
 * @brief Tags for the TextConfig struct
 *
 * Used to specifiy which "load" values to set
 */
namespace TextConfigTag
{
struct LoadFont
{
};

struct LoadFontBitmap
{
};

struct LoadFontPalette
{
};

struct LoadFontMetadata
{
};
} // namespace TextConfigTag
