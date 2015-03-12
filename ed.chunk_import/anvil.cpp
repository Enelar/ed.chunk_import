#include "anvil.h"
#include <algorithm>


#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>
#include <sstream>
#include <string>

namespace
{
  // http://minecraft.gamepedia.com/Region_file_format
  const int block_size = 4096;
  const int location_blocks = 1;
  const int chunks_per_file = 1024;
  const int location_size = 6;
}

namespace
{
  int ReadBigEndian(int size, istream &it)
  {
    int ret = 0;
    for (int i = 0; i < size; i++)
    {
      ret <<= 8;
      char ch;
      it.get(ch);
      ret += (unsigned char)ch;
    }
    return ret;
  }
}


anvil::anvil(string filename)
  : file(filename, std::ios::in | std::ios::binary)
{

}

anvil::chunkT anvil::Read(int x_coordinate, int z_coordinate)
{ // http://minecraft.gamepedia.com/Region_file_format#Structure
  int loc[] = { x_coordinate % 32, z_coordinate % 32 };
  for (auto &a : loc)
    if (a < 0)
      a += 32;
  for (auto a : loc)
    if (a < 0 || a > 31)
      throw "wow, error in coordinates";
  auto id = 4 * (loc[0] + loc[1] * 32);
  return Read(id);
}


anvil::chunkT anvil::Read(int id)
{
  auto chunk_location = ChunkLocation(id);
  file.seekg(chunk_location.first * block_size, ios_base::beg);
  int size = ReadBigEndian(4, file);
  enum _comp
  {
    GZIP = 1,
    ZLIB = 2
  } compression = (_comp)ReadBigEndian(4, file);

  std::stringstream decompressed;
  {
    boost::iostreams::filtering_istreambuf fis;
    switch (compression)
    {
    case GZIP:
      fis.push(boost::iostreams::gzip_decompressor());
      break;
    case ZLIB:
      fis.push(boost::iostreams::zlib_decompressor());
      break;
    default:
      throw "compression not supported";
    }

    fis.push(file);
    boost::iostreams::copy(fis, decompressed);
    decompressed.seekg(std::ios::beg);
  }

  auto nbt = NBT::Tag::read(decompressed);
  return nbt;
}

std::pair<int, int> anvil::ChunkLocation(int id)
{
  if (id < 0 || id > 1023)
    throw "wow, id out of range";
  file.seekg(id * location_size, ios_base::beg);
  auto
    location = ReadBigEndian(3, file),
    size = ReadBigEndian(3, file);
  return{ location, size };
}

namespace
{
  bool IsZero(const char &a) // sligtly faster than lambdas
  {
    return !a;
  }
}

vector<int> anvil::Present()
{
  file.seekg(0, ios_base::beg);
  const int size = location_blocks * block_size;
  char buf[size];
  file.read(buf, size);

  vector<int> ret;
  ret.reserve(chunks_per_file);
  for (auto i = 0; i < chunks_per_file; i++)
  {
    auto
      begin = buf + i * location_size,
      end = begin + location_size;

    auto find = find_if_not(begin, end, IsZero);
    if (find == end)
      continue;
    ret.push_back(i);
  }

  ret.shrink_to_fit();
  return ret;
}