#include "anvil.h"
#include <algorithm>

namespace
{
  // http://minecraft.gamepedia.com/Region_file_format
  const int block_size = 4096;
  const int location_blocks = 1;
  const int chunks_per_file = 1024;
  const int location_size = 6;
}

anvil::anvil(string filename)
  : file(filename, std::ios::in | std::ios::binary)
{

}

anvil::chunkT anvil::Read(int x_coordinate, int y_coordinate)
{
  return{};
}


anvil::chunkT anvil::Read(int id)
{
  return{};
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