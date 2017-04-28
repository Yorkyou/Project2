//
//  stage.cpp
//  Computer Architecture Project2
//
//  Created by York You on 2017/4/15.
//  Copyright © 2017年 York You. All rights reserved.
//

#include "stage.hpp"

stage::stage(){
    for (int i = 0; i< 5; i++) {
        opcode[i] = 0;
        shamt[i] = 0;
        funct[i] = 0;
        immediate[i] = 0;
        adress[i] = 0;
        rs[i] = 0;
        rt[i] = 0;
        rd[i] = 0;
        
    }
    for (int i = 0; i < 32; i++) {
        ex_dm[i] = 0;
        dm_wb[i] = 0;
        EX_DM_available[i] = false;
        EX_DM_TOBEavailable[i] = false;
        DM_WB_available[i] = false;
        DM_WB_TOBEavailable[i] = false;
        done[i] =  true;
        TOBE_done_EX[i] = false;
        TOBE_done_DM[i] = false;
    }
    accessAdr = 0;
    id_ex_31 = 0;
    flush = false;
    stall = false;
    terminate = 0;
}
void stage::stageUpdate(bool stall){
    //may have some conditions to update(stall)
    //containg diff vars
    //usually :
    int bound = (stall==true)? 2 : 0;
    for (int i = 3; i >=bound; i--) {
        funct[i+1] = funct[i];
        opcode[i+1] = opcode[i];
        shamt[i+1] = shamt[i];
        immediate[i+1] = immediate[i];
        adress[i+1] = adress[i];
        rs[i+1] = rs[i];
        rt[i+1] = rt[i];
        rd[i+1] = rd[i];
    }

    if (stall) insertNope(2);//insert NOP at EX stage
}
//Instruction Fetch
string stage::IF(bitset<32>command,Instruction *i,Reg *r){
    string out="";
    static bool stall_already = false;
    this->command = command;
    opcode[0] = int((command>>26).to_ulong());
    funct[0] = int((command<<26>>26).to_ulong());
    //get rs,rt,rd.(reg),shamt-->they must be positive so we directly trnaslate to int
    //immediate must be + or -
    rs[0] = int((command<<6>>27).to_ulong());
    rt[0] = int((command<<11>>27).to_ulong());
    rd[0] = int((command<<16>>27).to_ulong());
    shamt[0] = int((command<<21>>27).to_ulong());
    bitset<32> temp1 = command<<16>>16;//used for immediate
    bitset<32> temp2 = command<<6>>6;//used for adr
    for (int i = 0; i < 16; i++) immediate[0][i] = temp1[i];
    for (int i = 0; i < 26; i++) adress[0][i] = temp2[i];
    if (opcode[0]== 0x3F) {
        if(!stall_already)terminate++;
        if (terminate==5) {
            r->halt = true;
        }
    }
    else terminate = 0;
    
    if(stall)stall_already = true;
    else stall_already = false;
    
    out = i->BinaryToHex(command)+(flush==true?" to_be_flushed"
                                   :stall==true?" to_be_stalled":"");
    if(flush)insertNope(0);
    return out;
}

//maybe forward from ex/dm
string stage::ID(Reg *r,int cycle){
    string out="";
    stall=false,flush=false;
    bool fwd_EX_DM_rs=false,fwd_EX_DM_rt=false;
    //not done and not tobedone-->stall(PC不更新)
    //branch : not done-->stall
    if (IsNOP(opcode[1], rd[1], rt[1], shamt[1],funct[1])) {
        return "NOP";
    }
    switch (opcode[1]) {
        case 0x00: //R-type Instructions
            switch (funct[1]) {//select input first and know about fwd
                case 0x20://add
                    out += "ADD";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x21://addu
                    out += "ADDU";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x22://sub
                    out += "SUB";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x24://and
                    out += "AND";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x25://or
                    out += "OR";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x26://xor
                    out += "XOR";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x27://nor
                    out += "NOR";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x28://nand
                    out += "NAND";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x2A://slt
                    out += "SLT";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x00://sll
                    out += "SLL";
                    if (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]]) {
                        stall = true;
                    }
                    break;
                case 0x02://srl
                    out += "SRL";
                    if (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]]) {
                        stall = true;
                    }
                    break;
                case 0x03://sra
                    out += "SRA";
                    if (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]]) {
                        stall = true;
                    }
                    break;
                case 0x08://jr
                    out += "JR";
                    if (!done[rs[1]]) {
                        stall = true;
                    }
                    else {
                        bitset<32>input_rs;
                        if (EX_DM_available[rs[1]]) {
                            fwd_EX_DM_rs = true;
                            input_rs = ex_dm[rs[1]];
                        }
                        else input_rs = r->reg[rs[1]];
                        r->ID_jr(input_rs, cycle+1);
                        flush = true;
                    }
                    break;
                case 0x18:
                    out += "MULT";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x19:
                    out += "MULTU";
                    if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                        (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                        stall = true;
                    }
                    break;
                case 0x10:
                    out += "MFHI";
                    break;
                case 0x12:
                    out += "MFLO";
                    break;
                default:
                    // no such Instructions.
                    // halt.
                    break;
            }
            break;
            //I instructions start from here.
        case 0x08:
            out += "ADDI";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x09:
            out += "ADDIU";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x23://lw
            out += "LW";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x21://lh
            out += "LH";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x25:
            out += "LHU";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x20://lb
            out += "LB";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x24://lbu
            out += "LBU";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x2B://sw
            out += "SW";
            if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                stall = true;
            }
            break;
        case 0x29://sh
            out += "SH";
            if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                stall = true;
            }
            break;
        case 0x28://sb
            out += "SB";
            if ((!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]])||
                (!TOBE_done_EX[rt[1]]&&!TOBE_done_DM[rt[1]]&&!done[rt[1]])) {
                stall = true;
            }
            break;
        case 0x0F://lui
            out += "LUI";
            break;
        case 0x0C://andi
            out += "ANDI";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x0D://ori
            out += "ORI";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x0E://nori
            out += "NORI";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x0A://slti
            out += "SLTI";
            if (!TOBE_done_EX[rs[1]]&&!TOBE_done_DM[rs[1]]&&!done[rs[1]]) {
                stall = true;
            }
            break;
        case 0x04://beq
            out += "BEQ";
            if (!done[rs[1]]||!done[rt[1]] ) {
                stall = true;
            }
            else {
                bitset<32>input_rs,input_rt;
                if (EX_DM_available[rs[1]]) {
                    fwd_EX_DM_rs = true;
                    input_rs = ex_dm[rs[1]];
                    
                }
                else input_rs = r->reg[rs[1]];
                if (EX_DM_available[rt[1]]){
                    fwd_EX_DM_rt = true;
                    input_rt = ex_dm[rt[1]];
                }
                else input_rt = r->reg[rt[1]];
                if(r->ID_beq(input_rs, input_rt, r->BitsetToSigned16(immediate[1]), cycle+1))
                flush = true;
            }
            break;
        case 0x05://bne
            out += "BNE";
            if (!done[rs[1]]||!done[rt[1]] ) {
                stall = true;
            }
            else {
                bitset<32>input_rs,input_rt;
                if (EX_DM_available[rs[1]]) {
                    fwd_EX_DM_rs = true;
                    input_rs = ex_dm[rs[1]];
                    
                }
                else input_rs = r->reg[rs[1]];
                if (EX_DM_available[rt[1]]){
                    fwd_EX_DM_rt = true;
                    input_rt = ex_dm[rt[1]];
                }
                else input_rt = r->reg[rt[1]];
                if(r->ID_bne(input_rs, input_rt, r->BitsetToSigned16(immediate[1]), cycle+1))
                flush = true;
            }
            break;
        case 0x07://bgtz
            out += "BGTZ";
            if (!done[rs[1]]) {
                stall = true;
            }
            else {
                bitset<32>input_rs;
                if (EX_DM_available[rs[1]]) {
                    fwd_EX_DM_rs = true;
                    input_rs = ex_dm[rs[1]];
                }
                else {
                    input_rs = r->reg[rs[1]];
                }
                if(r->ID_bgtz(input_rs, r->BitsetToSigned16(immediate[1]), cycle+1))
                flush = true;
            }
            break;
            //////////J-type Inst.
        case 0x02://j
            out += "J";
            r->ID_j(adress[1], cycle+1);
            flush = true;
            break;
        case 0x03://jal
            out += "JAL";
            r->ID_jal(adress[1],&id_ex_31,cycle+1);//$31 problem
            flush = true;
            break;
            /////////////halt
        case 0x3F:
            out += "HALT";
            break;
        default:
            //no such instructions.
            break;
    }
    //DO stall depend on inst.(我要的東西還沒好)
    if (stall) {
        out+=" to_be_stalled";
    }
    else {
        if (fwd_EX_DM_rs) out += " fwd_EX-DM_rs_$"+to_string(rs[1]);
        if (fwd_EX_DM_rt) out += " fwd_EX-DM_rt_$"+to_string(rt[1]);
    }
    return out;
}

string stage::EX(Reg *r,int cycle){//do a Mux to select input from original/forwarding
    //from ex/dm or dm/wb or regs
    //write message
    string out="";//return
    bool fwd_EX_DM_rs = false,fwd_DM_WB_rs = false;
    bool fwd_EX_DM_rt = false,fwd_DM_WB_rt = false;
    for (int i = 0; i < 32; i++) {
        if(EX_DM_available[i])EX_DM_available[i] = false;//to dm stage
        
        if (EX_DM_TOBEavailable[i]) {
            EX_DM_available[i] = true;//set it to false done at DM stage.
            EX_DM_TOBEavailable[i] = false;
        }
        if (TOBE_done_EX[i]) {
            done[i] = true;
            TOBE_done_EX[i] = false;
        }
    }
    if (IsNOP(opcode[2], rd[2], rt[2], shamt[2],funct[2])) return "NOP";
    
    bitset<32>input_rs;
    bitset<32>input_rt;
    if (EX_DM_available[rs[2]]) {
        input_rs = ex_dm[rs[2]];
        fwd_EX_DM_rs = true;
    }
    else if (DM_WB_available[rs[2]]){
        input_rs = dm_wb[rs[2]];
        fwd_DM_WB_rs = true;
    }
    else input_rs = r->reg[rs[2]];
    
    if (EX_DM_available[rt[2]]) {
        input_rt = ex_dm[rt[2]];
        fwd_EX_DM_rt = true;
    }
    else if (DM_WB_available[rt[2]]){
        input_rt = dm_wb[rt[2]];
        fwd_DM_WB_rt = true;
    }
    else input_rt = r->reg[rt[2]];
    
    switch (opcode[2]) {
        case 0x00: //R-type Instructions
            switch (funct[2]) {//select input first and know about fwd
                case 0x20://add
                    r->EX_add(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);//write to temp reg.
                    out += "ADD";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x21://addu
                    r->EX_addu(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);
                    out += "ADDU";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x22://sub
                    r->EX_sub(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);
                    out += "SUB";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x24://and
                    r->EX_And(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);
                    out += "AND";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x25://or
                    r->EX_Or(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);
                    out += "OR";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x26://xor
                    r->EX_Xor(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);
                    out += "XOR";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x27://nor
                    r->EX_Nor(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);
                    out += "NOR";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x28://nand
                    r->EX_Nand(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);
                    out += "NAND";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x2A://slt
                    r->EX_slt(input_rs, input_rt, &ex_dm[rd[2]],rd[2],cycle+1);
                    out += "SLT";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x00://sll
                    r->EX_sll(&ex_dm[rd[2]], input_rt, shamt[2],rd[2],cycle+1);
                    out += "SLL";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    fwd_EX_DM_rs = false;
                    fwd_DM_WB_rs = false;
                    break;
                case 0x02://srl
                    r->EX_srl(&ex_dm[rd[2]],input_rt, shamt[2],rd[2],cycle+1);
                    out += "SRL";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    fwd_EX_DM_rs = false;
                    fwd_DM_WB_rs = false;
                    break;
                case 0x03://sra
                    r->EX_sra(&ex_dm[rd[2]],input_rt, shamt[2],rd[2],cycle+1);
                    out += "SRA";
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    fwd_EX_DM_rs = false;
                    fwd_DM_WB_rs = false;
                    break;
                case 0x08://jr
                    out += "JR";
                    fwd_EX_DM_rs = false;
                    fwd_DM_WB_rs = false;
                    fwd_EX_DM_rt = false;
                    fwd_DM_WB_rt = false;
                    break;
                case 0x18:
                    r->EX_mult(input_rs,input_rt,cycle+1);
                    out += "MULT";
                    break;
                case 0x19:
                    r->EX_multu(input_rs,input_rt,cycle+1);
                    out += "MULTU";
                    break;
                case 0x10:
                    r->EX_mfhi(&ex_dm[rd[2]],rd[2],cycle+1);
                    out += "MFHI";
                    fwd_EX_DM_rs = false;
                    fwd_DM_WB_rs = false;
                    fwd_EX_DM_rt = false;
                    fwd_DM_WB_rt = false;
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                case 0x12:
                    r->EX_mflo(&ex_dm[rd[2]],rd[2],cycle+1);
                    out += "MFLO";
                    fwd_EX_DM_rs = false;
                    fwd_DM_WB_rs = false;
                    fwd_EX_DM_rt = false;
                    fwd_DM_WB_rt = false;
                    if(rd[2]!=0){
                        done[rd[2]] = false;
                        TOBE_done_EX[rd[2]] = true;
                        EX_DM_TOBEavailable[rd[2]] = true;
                    }
                    break;
                default:
                    // no such Instructions.
                    // halt.
                    break;
            }
            break;
            //I instructions start from here.
        case 0x08:
            r->EX_addi(&ex_dm[rt[2]],input_rs,r->BitsetToSigned16(immediate[2]),rt[2],cycle+1);
            out += "ADDI";
            if(rt[2]!=0){
                done[rt[2]] = false;
                TOBE_done_EX[rt[2]] = true;
                EX_DM_TOBEavailable[rt[2]] = true;
            }
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x09:
           r->EX_addiu(&ex_dm[rt[2]],input_rs,r->BitsetToSigned16(immediate[2]),rt[2],cycle+1);
            out += "ADDIU";
            if(rt[2]!=0){
                done[rt[2]] = false;
                TOBE_done_EX[rt[2]] = true;
                EX_DM_TOBEavailable[rt[2]] = true;
            }
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x23://lw
            r->EX_CalculateMemIndex(&accessAdr, r->BitsetToSigned16(immediate[2]), input_rs, cycle+1);
            out += "LW";
            done[rt[2]] = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x21://lh
            r->EX_CalculateMemIndex(&accessAdr, r->BitsetToSigned16(immediate[2]), input_rs, cycle+1);
            out += "LH";
            done[rt[2]] = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x25://lhu
            r->EX_CalculateMemIndex(&accessAdr, r->BitsetToSigned16(immediate[2]), input_rs, cycle+1);
            out += "LHU";
            done[rt[2]] = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x20://lb
            r->EX_CalculateMemIndex(&accessAdr, r->BitsetToSigned16(immediate[2]), input_rs, cycle+1);
            out += "LB";
            done[rt[2]] = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x24://lbu
            r->EX_CalculateMemIndex(&accessAdr, r->BitsetToSigned16(immediate[2]), input_rs, cycle+1);
            out += "LBU";
            done[rt[2]] = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x2B://sw
            r->EX_CalculateMemIndex(&accessAdr, r->BitsetToSigned16(immediate[2]), input_rs, cycle+1);
            out += "SW";
            ex_dm[rt[2]] = input_rt;//****take rt
            break;
        case 0x29://sh
            r->EX_CalculateMemIndex(&accessAdr, r->BitsetToSigned16(immediate[2]), input_rs, cycle+1);
            out += "SH";
            ex_dm[rt[2]] = input_rt;//****
            break;
        case 0x28://sb
            r->EX_CalculateMemIndex(&accessAdr, r->BitsetToSigned16(immediate[2]), input_rs, cycle+1);
            out += "SB";
            ex_dm[rt[2]] = input_rt;//****
            break;
        case 0x0F://lui
            r->EX_lui(&ex_dm[rt[2]], immediate[2],rt[2],cycle+1);
            out += "LUI";
            if(rt[2]!=0){
                done[rt[2]] = false;
                TOBE_done_EX[rt[2]] = true;
                EX_DM_TOBEavailable[rt[2]] = true;
            }
            fwd_EX_DM_rs = false;
            fwd_DM_WB_rs = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x0C://andi
            r->EX_Andi(&ex_dm[rt[2]],input_rs, immediate[2].to_ulong(),rt[2],cycle+1);
            out += "ANDI";
            if(rt[2]!=0){
                done[rt[2]] = false;
                TOBE_done_EX[rt[2]] = true;
                EX_DM_TOBEavailable[rt[2]] = true;
            }
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x0D://ori
            r->EX_Ori(&ex_dm[rt[2]],input_rs, immediate[2].to_ulong(),rt[2],cycle+1);
            out += "ORI";
            if(rt[2]!=0){
                done[rt[2]] = false;
                TOBE_done_EX[rt[2]] = true;
                EX_DM_TOBEavailable[rt[2]] = true;
            }
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x0E://nori
            r->EX_Nori(&ex_dm[rt[2]],input_rs, immediate[2].to_ulong(),rt[2],cycle+1);
            out += "NORI";
            if(rt[2]!=0){
                done[rt[2]] = false;
                TOBE_done_EX[rt[2]] = true;
                EX_DM_TOBEavailable[rt[2]] = true;
            }
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x0A://slti
            r->EX_slti(&ex_dm[rt[2]], input_rs, r->BitsetToSigned16(immediate[2]),rt[2],cycle+1);
            out += "SLTI";
            if(rt[2]!=0){
                done[rt[2]] = false;
                TOBE_done_EX[rt[2]] = true;
                EX_DM_TOBEavailable[rt[2]] = true;
            }
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x04://beq
            out += "BEQ";
            fwd_EX_DM_rs = false;
            fwd_DM_WB_rs = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x05://bne
            out += "BNE";
            fwd_EX_DM_rs = false;
            fwd_DM_WB_rs = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x07://bgtz
            out += "BGTZ";
            fwd_EX_DM_rs = false;
            fwd_DM_WB_rs = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
            //////////J-type Inst.
        case 0x02://j
            out += "J";
            fwd_EX_DM_rs = false;
            fwd_DM_WB_rs = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        case 0x03://jal
            out += "JAL";
            fwd_EX_DM_rs = false;
            fwd_DM_WB_rs = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            ex_dm[31] = id_ex_31;
            EX_DM_TOBEavailable[31] = true;
            break;
            /////////////halt
        case 0x3F:
            out += "HALT";
            fwd_EX_DM_rs = false;
            fwd_DM_WB_rs = false;
            fwd_EX_DM_rt = false;
            fwd_DM_WB_rt = false;
            break;
        default:
            //no such instructions.
            break;
    }
    if (fwd_EX_DM_rs) {
        out += " fwd_EX-DM_rs_$"+to_string(rs[2]);
    }
    else if (fwd_DM_WB_rs){
        out += " fwd_DM-WB_rs_$"+to_string(rs[2]);
    }
    if (fwd_EX_DM_rt) {
        out += " fwd_EX-DM_rt_$"+to_string(rt[2]);
    }
    else if (fwd_DM_WB_rt){
        out += " fwd_DM-WB_rt_$"+to_string(rt[2]);
    }
    return out;
}


string stage::DM(Reg *r, Memory *m, int cycle){
    string out="";
    for (int i = 0; i < 32; i++) {
        dm_wb[i] = ex_dm[i];//may cause problems
        if (DM_WB_available[i]) {
            DM_WB_available[i] = false;
        }
        if (DM_WB_TOBEavailable[i]) {
            DM_WB_TOBEavailable[i] = false;
            DM_WB_available[i] = true;
        }
        if (EX_DM_TOBEavailable[i]) {
            DM_WB_TOBEavailable[i] = true;
        }
        if (TOBE_done_DM[i]){
            done[i] = true;
            TOBE_done_DM[i] = false;
        }
    }
    if (IsNOP(opcode[3], rd[3], rt[3], shamt[3],funct[3])) {
        return "NOP";
    }
    switch (opcode[3]) {
        case 0x00: //R-type Instructions
            switch (funct[3]) {//select input first and know about fwd
                case 0x20://add
                    out += "ADD";
                    break;
                case 0x21://addu
                    out += "ADDU";
                    break;
                case 0x22://sub
                    out += "SUB";
                    break;
                case 0x24://and
                    out += "AND";
                    break;
                case 0x25://or
                    out += "OR";
                    break;
                case 0x26://xor
                    out += "XOR";
                    break;
                case 0x27://nor
                    out += "NOR";
                    break;
                case 0x28://nand
                    out += "NAND";
                    break;
                case 0x2A://slt
                    out += "SLT";
                    break;
                case 0x00://sll
                    out += "SLL";
                    break;
                case 0x02://srl
                    out += "SRL";
                    break;
                case 0x03://sra
                    out += "SRA";
                    break;
                case 0x08://jr
                    out += "JR";
                    break;
                case 0x18:
                    out += "MULT";
                    break;
                case 0x19:
                    out += "MULTU";
                    break;
                case 0x10:
                    out += "MFHI";
                    break;
                case 0x12:
                    out += "MFLO";
                    break;
                default:
                    // no such Instructions.
                    // halt.
                    break;
            }
            break;
            //I instructions start from here.
        case 0x08:
            out += "ADDI";
            break;
        case 0x09:
            out += "ADDIU";
            break;
        case 0x23://lw
            out += "LW";
            r->DM_lw(accessAdr, &dm_wb[rt[3]], m);
            if(rt[3]!=0){
                TOBE_done_DM[rt[3]] = true;
                DM_WB_TOBEavailable[rt[3]] = true;
            }
            break;
        case 0x21://lh
            out += "LH";
            r->DM_lh(accessAdr, &dm_wb[rt[3]], m);
            if(rt[3]!=0){
                TOBE_done_DM[rt[3]] = true;
                DM_WB_TOBEavailable[rt[3]] = true;
            }
            break;
        case 0x25:
            out += "LHU";
            r->DM_lhu(accessAdr, &dm_wb[rt[3]], m);
            if(rt[3]!=0){
                TOBE_done_DM[rt[3]] = true;
                DM_WB_TOBEavailable[rt[3]] = true;
            }
            break;
        case 0x20://lb
            out += "LB";
            r->DM_lb(accessAdr, &dm_wb[rt[3]], m);
            if(rt[3]!=0){
                TOBE_done_DM[rt[3]] = true;
                DM_WB_TOBEavailable[rt[3]] = true;
            }
            break;
        case 0x24://lbu
            out += "LBU";
            r->DM_lbu(accessAdr, &dm_wb[rt[3]], m);
            if(rt[3]!=0){
                TOBE_done_DM[rt[3]] = true;
                DM_WB_TOBEavailable[rt[3]] = true;
            }
            break;
        case 0x2B://sw
            out += "SW";
            r->DM_sw(accessAdr, dm_wb[rt[3]], m);
            break;
        case 0x29://sh
            out += "SH";
            r->DM_sh(accessAdr, dm_wb[rt[3]], m);
            break;
        case 0x28://sb
            out += "SB";
            r->DM_sb(accessAdr, dm_wb[rt[3]], m);
            break;
        case 0x0F://lui
            out += "LUI";
            break;
        case 0x0C://andi
            out += "ANDI";
            break;
        case 0x0D://ori
            out += "ORI";
            break;
        case 0x0E://nori
            out += "NORI";
            break;
        case 0x0A://slti
            out += "SLTI";
            break;
        case 0x04://beq
            out += "BEQ";
            break;
        case 0x05://bne
            out += "BNE";
            break;
        case 0x07://bgtz
            out += "BGTZ";
            break;
            //////////J-type Inst.
        case 0x02://j
            out += "J";
            break;
        case 0x03://jal
            out += "JAL";
            break;
        case 0x3F:
            out += "HALT";
            break;
        default:
            //no such instructions.
            break;
    }
    return out;
}
string stage::WB(Reg *r, int cycle){
    string out="";
    if (IsNOP(opcode[4], rd[4], rt[4], shamt[4],funct[4])) {
        return "NOP";
    }
    switch (opcode[4]) {
        case 0x00: //R-type Instructions
            switch (funct[4]) {//select input first and know about fwd
                case 0x20://add
                    out += "ADD";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x21://addu
                    out += "ADDU";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x22://sub
                    out += "SUB";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x24://and
                    out += "AND";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x25://or
                    out += "OR";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x26://xor
                    out += "XOR";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x27://nor
                    out += "NOR";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x28://nand
                    out += "NAND";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x2A://slt
                    out += "SLT";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x00://sll
                    out += "SLL";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x02://srl
                    out += "SRL";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x03://sra
                    out += "SRA";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x08://jr
                    out += "JR";
                    break;
                case 0x18:
                    out += "MULT";
                    break;
                case 0x19:
                    out += "MULTU";
                    break;
                case 0x10:
                    out += "MFHI";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                case 0x12:
                    out += "MFLO";
                    r->WB_WriteReg(dm_wb[rd[4]], rd[4]);
                    if(rd[4]!=0) done[rd[4]] = true;
                    break;
                default:
                    // no such Instructions.
                    // halt.
                    break;
            }
            break;
            //I instructions start from here.
        case 0x08:
            out += "ADDI";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x09:
            out += "ADDIU";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x23://lw
            out += "LW";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x21://lh
            out += "LH";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x25:
            out += "LHU";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x20://lb
            out += "LB";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x24://lbu
            out += "LBU";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x2B://sw
            out += "SW";
            break;
        case 0x29://sh
            out += "SH";
            break;
        case 0x28://sb
            out += "SB";
            break;
        case 0x0F://lui
            out += "LUI";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x0C://andi
            out += "ANDI";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x0D://ori
            out += "ORI";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x0E://nori
            out += "NORI";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x0A://slti
            out += "SLTI";
            r->WB_WriteReg(dm_wb[rt[4]], rt[4]);
            if(rt[4]!=0) done[rt[4]] = true;
            break;
        case 0x04://beq
            out += "BEQ";
            break;
        case 0x05://bne
            out += "BNE";
            break;
        case 0x07://bgtz
            out += "BGTZ";
            break;
            //////////J-type Inst.
        case 0x02://j
            out += "J";
            break;
        case 0x03://jal
            out += "JAL";
            r->WB_WriteReg(dm_wb[31], 31);
            done[31] = true;
            break;
            /////////////halt
        case 0x3F:
            out += "HALT";
            break;
        default:
            //no such instructions.
            break;
    }
    //DO stall depend on inst.(我要的東西還沒好)
    return out;
}