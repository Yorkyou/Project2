//
//  register.hpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/16.
//  Copyright © 2017年 York You. All rights reserved.
//

#ifndef register_hpp
#define register_hpp
#include <bitset>
#include <queue>
#include "memory.hpp"
#include <math.h>
using namespace std;
class Reg{
    friend class Memory;
    friend class Instruction;
    friend class stage;
protected:
    bitset<32>reg[35];//registers:HI(32),LO(33),PC(34),SP(29),RA(31)each view as 32bits
    //fault detection
    bool write0,overWriteHILO,d_MemAddressOverflow,d_MemMissalign,numberOverflow;
    bool ismult;
    bool PCisadjusted;
    bool halt;
    enum {pos,neg,zero};
public:
    Reg(bitset<32> SP,bitset<32> PC);
    long long BitsetToSigned32(bitset<32>);//there is no to_long() function in bitset.
    long BitsetToSigned16(bitset<16>);
    void PCmoveForward();
    //R-type Inst.
    void EX_add(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    //*temp_rd == ex_dm
    void EX_addu(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_sub(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_And(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_Or(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_Xor(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_Nor(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_Nand(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_slt(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_sll(bitset<32>* ex_dm_rd,bitset<32> rt,long C,int rd,int cycle);
    void EX_srl(bitset<32>* ex_dm_rd,bitset<32> rt,long C,int rd,int cycle);
    void EX_sra(bitset<32>* ex_dm_rd,bitset<32> rt,long C,int rd,int cycle);
    void ID_jr(bitset<32> rs,int cycle);
    void EX_mult(bitset<32> rs,bitset<32> rt,int cycle);
    void EX_multu(bitset<32> rs,bitset<32> rt,int cycle);
    void EX_mfhi(bitset<32>* ex_dm_rd,int rd,int cycle);
    void EX_mflo(bitset<32>* ex_dm_rd,int rd,int cycle);
    //I-type inst.
    void EX_addi(bitset<32>* ex_dm_rt, bitset<32> rs, long i,int rt,int cycle);
    void EX_addiu(bitset<32>* ex_dm_rt, bitset<32> rs, long i,int rt,int cycle);
    void EX_CalculateMemIndex(bitset<32>* accessAdr,long i,bitset<32> rs,int cycle);
    void EX_lui(bitset<32>* ex_dm_rt, bitset<16> rs,int rt,int cycle);
    void EX_Andi(bitset<32>* ex_dm_rt, bitset<32> rs,long i,int rt,int cycle);
    void EX_Ori(bitset<32>* ex_dm_rt, bitset<32> rs,long i,int rt,int cycle);
    void EX_Nori(bitset<32>* ex_dm_rt, bitset<32> rs,long i,int rt,int cycle);
    void EX_slti(bitset<32>* ex_dm_rt, bitset<32> rs,long i,int rt,int cycle);
    bool ID_beq(bitset<32> rs,bitset<32> rt,long i,int cycle);
    bool ID_bne(bitset<32> rs,bitset<32> rt,long i,int cycle);
    bool ID_bgtz(bitset<32> rs,long i,int cycle);
    //J-type inst.
    void ID_j(bitset<26> adr,int cycle);
    void ID_jal(bitset<26> adr,bitset<32>* id_ex_31,int cycle);
    //halt
    void DM_lw(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m);
    void DM_lh(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m);
    void DM_lhu(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m);
    void DM_lb(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m);
    void DM_lbu(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m);
    void DM_sw(bitset<32>index,bitset<32> input,Memory *m);
    void DM_sh(bitset<32>index,bitset<32> input,Memory *m);
    void DM_sb(bitset<32>index,bitset<32> input,Memory *m);
    void WB_WriteReg(bitset<32>input,int des);
};

#endif /* register_h */
