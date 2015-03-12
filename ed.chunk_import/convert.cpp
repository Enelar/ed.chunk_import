#include "convert.h"

using namespace NBT;
namespace
{
  template<typename T>
  T *TryExtractFromCompound(const Tag::Compound &compound, const Tag::Name_t &key)
  {
    auto it = compound.v.find(key);
    if (it != compound.v.end())
      return (T *)it->second.get();
    return nullptr;
  };


  template<typename T>
  auto &ExtractFromCompound(const Tag::Compound &compound, const Tag::Name_t &key)
  {
    auto *ret = TryExtractFromCompound<T>(compound, key);
    if (ret->id() != T::ID)
      throw "unexpected element type";
    return *ret;
  };

  std::uint8_t GetHalfbyte(std::uint8_t array[], int i)
  {
    auto *base_ptr = array + i / 2;
    if (i & 1)
      return *base_ptr & 15;
    return *base_ptr >> 4;
  }
}

chunk Convert(const anvil::chunkT &ptr)
{
  if (!ptr)
    throw "chunk not present";

  // http://minecraft.gamepedia.com/Chunk_format
  // http://minecraft.gamepedia.com/Anvil_file_format#Further_information
  if (ptr->id() != Tag::Compound::ID)
    throw "Root item should be compound";

  chunk ret;

  const auto &root = *(Tag::Compound *)ptr.get();
  const auto &level = ExtractFromCompound<Tag::Compound>(root, "Level");

  ret.x = ExtractFromCompound<Tag::Int>(level, "xPos").v;
  ret.z = ExtractFromCompound<Tag::Int>(level, "zPos").v;

  const auto &sections = ExtractFromCompound<Tag::List>(level, "Sections");
  for (auto &ptr : sections.v)
  {
    auto &section = (const Tag::Compound &)(*ptr);
    if (section.id() != Tag::Compound::ID)
      throw "unexpected element type";

    auto y = ExtractFromCompound<Tag::Byte>(section, "Y").v;
    auto &to = ret.AccessBlock(y * 16);

    {
      auto blocks = ExtractFromCompound<Tag::ByteArray>(section, "Blocks");
      auto *add = TryExtractFromCompound<Tag::ByteArray>(section, "Add");

      // YZX
      int i = 0;
      const int dimension_block_per_section = 15;
      for (int y = 0; y < dimension_block_per_section; y++)
        for (int z = 0; z < dimension_block_per_section; z++)
          for (int x = 0; x < dimension_block_per_section; x++)
          {
            int compose = 0;
            if (add)
              compose += GetHalfbyte((std::uint8_t *)&add->v[0], i);
            to[y][z][x] = (compose << 8) + blocks.v[i];
          }

    }

    continue;
    { // TODO: Support this stuff
      auto data = ExtractFromCompound<Tag::ByteArray>(section, "Data");
      auto block_light = ExtractFromCompound<Tag::ByteArray>(section, "BlockLight");
      auto sky_light = ExtractFromCompound<Tag::ByteArray>(section, "SkyLight");
    }
  }

  return ret;
}