#include "anvil.h"
#include "convert.h"

auto main()
{
  anvil test("../r.0.0.mca");

  for (auto id : test.Present())
  {
    auto their_chunk = test.Read(id);
    auto our_chunk = Convert(their_chunk);
  }
}