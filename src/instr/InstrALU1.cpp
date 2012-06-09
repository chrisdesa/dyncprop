#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "InstrALU1.hpp"

namespace Dyncprop {

  InstrALU1::~InstrALU1()
  {
    
  }
  
  std::vector<Home> InstrALU1::inputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<Home> InstrALU1::outputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<uint8_t> InstrALU1::opcode() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  Instr* InstrALU1::cprop(Home input, Data value) const
  {
    return NULL;
  }

  Instr* InstrALU1::parse(const uint8_t* ip)
  {
    
    return NULL;
  }
}
