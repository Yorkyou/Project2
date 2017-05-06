//
//  instruction.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.
//
using namespace::std;
#include <algorithm>
#include <iostream>
#include "instruction.hpp"
string Instruction::BinaryToHex(bitset<32>a){
    string r="";
    bitset<4>data;
    for (int i = 0; i < 8; i++) {//3 2 1 0?
        data[0] = a[i*4];
        data[1] = a[i*4+1];
        data[2] = a[i*4+2];
        data[3] = a[i*4+3];
        if (data.to_ulong() > 9) {
           char temp = char(data.to_ulong()-10+'A');
            r.push_back(temp);
        }
        else r+= to_string(data.to_ulong());
    }
    reverse(r.begin(), r.end());
    return "0x"+r;
}
void Instruction::RecordRegAndUpdate(Reg* r, int cycle){
    if (cycle==0) {
        output += "cycle 0\n";
        for (int i = 0; i < 35; i++) {
            if (i==32)
                output += "$HI: "+BinaryToHex(r->reg[i])+"\n";
            else if (i==33)
                output += "$LO: "+BinaryToHex(r->reg[i])+"\n";
            else if (i==34)
                output += "PC: "+BinaryToHex(r->reg[i])+"\n";
            else
                output += "$"+(i<10 ?to_string(0)+to_string(i) :to_string(i))+": "+BinaryToHex(r->reg[i])+"\n";
            //initialize old reg
            this->Oldreg[i] = r->reg[i];
        }
    }
    else{
        output += "cycle "+to_string(cycle)+"\n";
        for (int i = 0; i < 34; i++) {
            if (r->reg[i]!=this->Oldreg[i]) {
                if (i==32)
                    output += "$HI: "+BinaryToHex(r->reg[i])+"\n";
                else if (i==33)
                    output += "$LO: "+BinaryToHex(r->reg[i])+"\n";
                else
                    output += "$"+(i<10 ?to_string(0)+to_string(i) :to_string(i))+": "+BinaryToHex(r->reg[i])+"\n";
            }
            this->Oldreg[i] = r->reg[i];
        }
        output += "PC: "+BinaryToHex(r->reg[34])+"\n";
    }
}
void Instruction::RecordStage(){
    output += "IF: "+stageOutput[0]+"\n"+
              "ID: "+stageOutput[1]+"\n"+
              "EX: "+stageOutput[2]+"\n"+
              "DM: "+stageOutput[3]+"\n"+
              "WB: "+stageOutput[4]+"\n\n\n";
}
void Instruction::RecordError(Reg *r){//also make 5 error detect bool to 0
    if(r->write0) error+="In cycle "+to_string(cycle+1)+": Write $0 Error\n";
    if(r->d_MemAddressOverflow)error+="In cycle "+to_string(cycle+1)+": Address Overflow\n";
    if(r->d_MemMissalign)error+="In cycle "+to_string(cycle+1)+": Misalignment Error\n";
    if(r->overWriteHILO) error+="In cycle "+to_string(cycle+1)+": Overwrite HI-LO registers\n";
    if(r->numberOverflow)error+="In cycle "+to_string(cycle+1)+": Number Overflow\n";
    r->write0 = false;
    r->overWriteHILO = false;
    r->d_MemAddressOverflow = false;
    r->d_MemAddressOverflow = false;
    r->d_MemMissalign = false;
    r->numberOverflow = false;
}
void Instruction::StartInstrctuion(Reg *r,Memory *m,stage *s){
    bitset<32> command;
    cycle = 0;
    // r->reg[34] As PC -->maybe there is signed / unsigend -->print out data bf execution
    // If PC is adjusted in command -->don't need to add four
    for (command=bitset<32>(m->InstToMem[int(r->reg[34].to_ulong())])
         ;!command.none()|| r->reg[34].to_ulong() < 1024
         ;command=bitset<32>(m->InstToMem[int(r->reg[34].to_ulong())]),cycle++) {
        
        RecordRegAndUpdate(r, cycle);
        stageOutput[4] =s->WB(r, cycle);
        stageOutput[3] =s->DM(r, m, cycle);
        stageOutput[2] =s->EX(r, cycle);
        stageOutput[1] =s->ID(r, cycle);
        stageOutput[0] =s->IF(command, this, r);
        RecordStage();
        RecordError(r);
        s->stageUpdate(s->stall);
        if (r->halt)break;
        if (!r->PCisadjusted && !s->stall && !s->flush) {
            r->pre_PC = r->reg[34];
            r->PCmoveForward();
        }
        else r->PCisadjusted = false;
        
        //halt ~~~
    }
}
