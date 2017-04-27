//
//  stage.h
//  Computer Architecture Project2
//
//  Created by York You on 2017/4/15.
//  Copyright © 2017年 York You. All rights reserved.
//

#ifndef stage_hpp
#define stage_hpp
using namespace std;
class Instruction;
#include "memory.hpp"
#include "register.hpp"
#include "instruction.hpp"
#include <queue>
#include <bitset>
#include <string>

class stage{
    friend class Memory;
    friend class Reg;
    friend class Instruction;
public:
    stage();
    //5 stages using five funct.(also write string info for each stage)
    string IF(bitset<32> command,Instruction *i,Reg *r);
    string ID(Reg *r,int cycle);
    string EX(Reg *r,int cycle);
    string DM(Reg *r,Memory *m,int cycle);
    string WB(Reg *r,int cycle);//Write 0 detection
    //stage update
    void stageUpdate(bool stall);
    void BecomesNOP(int s);
    bool IsNOP(int op,int rd,int rt,int shamt,int funct){
        if (op==0x00 && rd==0 && rt==0 && shamt==0 && funct==0x00) {
            return true;
        }
        else return false;
    }
    void insertNope(int stage){
        opcode[stage]=0x00;
        rd[stage]=0;
        rt[stage]=0;
        shamt[stage]=0;
        funct[stage]=0x00;
    }
private:
    bool stall,flush;//留給instruction去重新設定為false
    bool EX_DM_available[32];//If true we need to forwarding/stall
    bool EX_DM_TOBEavailable[32];//If true we need to forwarding/stall
    bool DM_WB_available[32];//If true we need to forwarding/stall
    bool DM_WB_TOBEavailable[32];//If true we need to forwarding/stall
    bool TOBE_done[32];//If not available -->stall
    bool done[32];//used for fly
    bitset<32> id_ex_31;
    bitset<32> ex_dm[32];//32 temp
    bitset<32> dm_wb[32];//32 temp
    bitset<32> accessAdr;//index for memory access
    bitset<32> command;
    int opcode[5],shamt[5],funct[5],rs[5],rt[5],rd[5];
    //ID EX DM WB
    bitset<16> immediate[5];//IF ID EX DM WB
    bitset<26> adress[5];//IF EX DM WB
    int terminate;//5 continueous halt or not
};

#endif /* stage_hpp */
