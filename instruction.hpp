//
//  instruction.hpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.
//

#ifndef instruction_hpp
#define instruction_hpp
//
//  instruction.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.
//
using namespace::std;
class stage;
#include <iostream>
#include "register.hpp"
#include "memory.hpp"
#include "stage.hpp"

class Instruction{//implementation
    friend class Reg;
    friend class stage;
public:
    Instruction(){error="";output="";std::fill(Oldreg,Oldreg+34,0);};
    void StartInstrctuion(Reg *r,Memory *m,stage *s);
    void RecordRegAndUpdate(Reg *r,int cycle);//need to fix
    void RecordStage();
    void RecordError(Reg *r);
    string BinaryToHex(bitset<32>);
    std::string getOut(){return output;}
    std::string getError(){return error;}
    int cycle;
private:
    bitset<32> Oldreg[35];
    string stageOutput[5];
    std::string output;
    std::string error;
};

#endif /* instruction_hpp */
