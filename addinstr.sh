#! /bin/bash

if [ $# -ne 1 ]
then
echo "Usage: `basename $0` {classname}"
exit
fi

if [[ (-e "src/instr/$1.cpp") || (-e "src/instr/$1.hpp") ]]
then
echo "Error: class files already exist."
exit
fi

touch src/instr/$1.cpp src/instr/$1.hpp

cat >src/instr/$1.hpp <<EOF
#pragma once

#include <stdint.h>
#include <vector>

#include "../Instr.hpp"

namespace Dyncprop {

  class Home;
  class Data;
  class State;

  /**
   * $1
   **/
  
  class $1: public Instr {    
  public:
    virtual std::vector<Home> inputs() const;
    virtual std::vector<Home> outputs() const;
    virtual std::vector<uint8_t> opcode() const;
    virtual const Instr* cprop(Home input, Data value) const;
    virtual const Instr* canonicalize() const;

  public:
    virtual ~$1();    

  public:
    static Instr* parse(const uint8_t* ip);
  };

}
EOF

cat >src/instr/$1.cpp <<EOF
#include <stdlib.h>
#include <stdio.h>

#include "../Instr.hpp"
#include "../Data.hpp"
#include "../Home.hpp"
#include "../State.hpp"
#include "$1.hpp"

namespace Dyncprop {

  $1::~$1()
  {
    
  }

  std::vector<Home> $1::inputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<Home> $1::outputs() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  std::vector<uint8_t> $1::opcode() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  const Instr* $1::cprop(Home input, Data value) const
  {
    return NULL;
  }
  
  const Instr* $1::canonicalize() const
  {
    fprintf(stderr, "Error: Not implemented yet (%s:%d).\n", __FILE__, __LINE__);
    exit(1);
  }

  Instr* $1::parse(const uint8_t* ip)
  {
    return NULL;
  }

  
}
EOF

echo "INSTR($1)" >>src/Instructions.list

echo "#include \"instr/$1.hpp\"" >>src/Instructions.hpp

kate src/instr/$1.cpp src/instr/$1.hpp


