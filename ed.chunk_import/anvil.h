#pragma once

#include <MCModify\include\NamedBinaryTag.hpp>
#include <string>
#include <fstream>

using namespace std;
struct anvil
{
  ifstream file;
  anvil(string filename);

  typedef decltype(NBT::Tag::read(declval<iostream &>())) chunkT;

  chunkT Read(int x_coordinate, int z_coordinate);
  chunkT Read(int id);
  vector<int> Present();

private:
  std::pair<int, int> ChunkLocation(int id);
};
