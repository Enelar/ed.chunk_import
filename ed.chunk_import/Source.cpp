#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <exception>

#include <MCModify\include\NamedBinaryTag.hpp>

#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/copy.hpp>

using namespace NBT;

auto TestRead(std::string filename)
{
  std::stringstream decompressed;
  {
    std::ifstream compressed(filename, std::ios::in | std::ios::binary);
    if (!compressed)
      throw "exception";
    boost::iostreams::filtering_istreambuf fis;
    fis.push(boost::iostreams::gzip_decompressor());
    fis.push(compressed);
    boost::iostreams::copy(fis, decompressed);
    decompressed.seekg(std::ios::beg);
  }

  auto nbt = NBT::Tag::read(decompressed);
  return nbt;
}

auto main()
{
  auto test = TestRead("../level.in.dat");
}