//
//  reg.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.
//
#include <iostream>
#include "register.hpp"
string BinaryToHex(bitset<32>a){
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
bitset<64> operator+(bitset<64>r,bitset<64>temp){
    bool carry = false;
    bitset<64> result;
    for (int i = 0; i < 64; i++) {
        result[i] = r[i]^temp[i]^carry;
        carry = (r[i]&temp[i]) | (r[i]&carry) | (temp[i]&carry);
    }
    return result;
}
bitset<32> operator+(bitset<32>r,bitset<32>temp){
    bool carry = false;
    bitset<32> result;
    for (int i = 0; i < 32; i++) {
        result[i] = r[i]^temp[i]^carry;
        carry = (r[i]&temp[i]) | (r[i]&carry) | (temp[i]&carry);
    }
    return result;
}
Reg::Reg(bitset<32> SP,bitset<32> PC){
    std::fill(reg, reg+34, 0);
    reg[29] = SP;
    reg[34] = PC;
    ismult = false;
    PCisadjusted = false;
    halt = false;
    write0 = false;
    overWriteHILO = false;
    d_MemAddressOverflow = false;
    d_MemMissalign = false;
    numberOverflow = false;
    pre_PC = this->reg[34];
}
long long Reg::BitsetToSigned32(bitset<32>data){
    bitset<31>temp;
    for (int i = 0; i < 31; i++) {
        temp[i] = data[i];
    }
    return temp.to_ulong()-data[31]*pow(2, 31);
}
long Reg::BitsetToSigned16(bitset<16>data){
    bitset<15>temp;
    for (int i = 0; i < 15; i++) {
        temp[i] = data[i];
    }
    return temp.to_ulong()-data[15]*pow(2, 15);
}
void Reg::PCmoveForward(){
    this->reg[34] = this->reg[34]+bitset<32>(4);
}
//operations define below
//error def :
//(1)write$0 (2)Overwrite HI-LO registers(3)Memory address overflow
//(4)Data misaligned (5)Number Overflow

//signed,(1)(2)
void Reg::EX_add(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
    }//still need to operate below to know if errors occur.
    int rs_is_pos=zero,rt_is_pos=zero,rd_is_pos=zero;
    
    if(BitsetToSigned32(rs)>0)rs_is_pos=pos;
    else if (BitsetToSigned32(rs)< 0)rs_is_pos=neg;
    if(BitsetToSigned32(rt)>0)rt_is_pos=pos;
    else if(BitsetToSigned32(rt)<0)rt_is_pos=neg;

    if (rd!=0){
        *ex_dm_rd = rs+rt;
        if(BitsetToSigned32(*ex_dm_rd)>0)rd_is_pos = pos;
        else if(BitsetToSigned32(*ex_dm_rd)<0)rd_is_pos = neg;
        if (rt_is_pos==rs_is_pos &&rs_is_pos!=rd_is_pos) {//means overflow
            //print out sth and cont. the program.
//            this->Error+="In cycle "+to_string(cycle)+": Number Overflow\n";
            numberOverflow = true;
        }
    }
    else
    {
        bitset<32>temp = rs+rt;
        if(BitsetToSigned32(temp)>0)rd_is_pos = pos;
        else if(BitsetToSigned32(temp)<0)rd_is_pos = neg;
        if (rt_is_pos==rs_is_pos &&rs_is_pos!=rd_is_pos) {//means overflow
            //print out sth and cont. the program.
//            this->Error+="In cycle "+to_string(cycle)+": Number Overflow\n";
            numberOverflow = true;
        }
    }
}
//unsigned,(1)
void Reg::EX_addu(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = rs+rt;
}
//signed,(1)(2)
void Reg::EX_sub(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    //A+(-B) using two's com to get -B first
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
    }
    int rs_is_pos=zero,minus_rt_is_pos=zero,rd_is_pos=zero;
    bitset<32>minus_rt = rt.flip()+bitset<32>(1);//two's complemet-->flip back
    rt.flip();
    if(BitsetToSigned32(minus_rt)>0)minus_rt_is_pos=pos;
    else if(BitsetToSigned32(minus_rt)<0)minus_rt_is_pos=neg;
    if(BitsetToSigned32(rs)>0)rs_is_pos=pos;
    else if(BitsetToSigned32(rs)<0)rs_is_pos=neg;
    if (rd!=0) {
        *ex_dm_rd = rs+minus_rt;
        if(BitsetToSigned32(*ex_dm_rd)>0)rd_is_pos=pos;
        else if(BitsetToSigned32(*ex_dm_rd)<0)rd_is_pos=neg;
        if (minus_rt_is_pos==rs_is_pos &&rs_is_pos!=rd_is_pos) {//means overflow
//            this->Error+="In cycle "+to_string(cycle)+": Number Overflow\n";
            numberOverflow = true;
        }
    }
    else{
        bitset<32>temp = rs+minus_rt;
        if(BitsetToSigned32(temp)>0)rd_is_pos=pos;
        else if(BitsetToSigned32(temp)<0)rd_is_pos=neg;
        if (minus_rt_is_pos==rs_is_pos &&rs_is_pos!=rd_is_pos) {//means overflow
//            this->Error+="In cycle "+to_string(cycle)+": Number Overflow\n";
            numberOverflow = true;
        }
    }
}
void Reg::EX_And(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = rs&rt;
}
void Reg::EX_Or(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = rs|rt;
}
void Reg::EX_Xor(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = rs^rt;
}
void Reg::EX_Nor(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = ~(rs|rt);
}
void Reg::EX_Nand(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = ~(rs&rt);
}

void Reg::EX_slt(bitset<32> rs,bitset<32> rt,bitset<32>* ex_dm_rd,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    if(BitsetToSigned32(rs)<BitsetToSigned32(rt)) *ex_dm_rd = 1;
    else *ex_dm_rd = 0;
}
void Reg::EX_sll(bitset<32>* ex_dm_rd,bitset<32> rt,long C,int rd,int cycle){
    if (rd==0) {
        if(rt==0 && C==0)return;//NOP
        else {
//            this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
            return;
        }
    }
    *ex_dm_rd = rt << C;
}
void Reg::EX_srl(bitset<32>* ex_dm_rd,bitset<32> rt,long C,int rd,int cycle){
    if (rd==0) {
        //this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = rt >> C;
}
void Reg::EX_sra(bitset<32>* ex_dm_rd,bitset<32> rt,long C,int rd,int cycle){
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    bitset<32> a(rt);
    if (a[31]) {
        a = a>>C;
        for (int i = 0; i < C; i++) {
            a[31-i].flip();
        }
    }
    else a = a>>C;
    *ex_dm_rd = a;
}

void Reg::ID_jr(bitset<32> rs,int cycle){//We will execute this at ID stage.
    this->reg[34] = rs;//pc = reg[34]
    PCisadjusted = true;
}

//operations define below
//error def :
//(1)write$0 (2)Overwrite HI-LO registers(3)Memory address overflow
//(4)Data misaligned (5)Number Overflow

//signed,(2)
void Reg::EX_mult(bitset<32> rs,bitset<32> rt,int cycle){//may cause problem
    bitset<64>a(0),b(0);//rs,rt 0 extension(a : rs/ b : rt)
    bitset<64>result(0),temp(0);
    if (ismult) {
//        Error+="In cycle "+to_string(cycle)+": Overwrite HI-LO registers\n";
        overWriteHILO = true;
    }
    for (int i = 0; i < 64; i++) {
	if(i<32){
      	  a[i] = rs[i];
      	  b[i] = rt[i];
	}
	else {
	  a[i] = a[31];
	  b[i] = b[31];
	}
    }
    for(int i = 0;i <64 ;i++){
        if (a[i]) {
            temp = b;
            temp = temp << i;
            result=result+temp;
        }
    }
    for (int i = 0; i < 32;i++) {
        this->reg[32][i] = result[32+i];
        this->reg[33][i] = result[i];
    }
    ismult = true;
    //error detections.
}

//unsigned,(3)
void Reg::EX_multu(bitset<32> rs,bitset<32> rt,int cycle){
    bitset<64>a(0),b(0);//rs,rt 0 extension(a : rs/ b : rt)
    bitset<64>result(0),temp(0);
    if (ismult) {
//        Error+="In cycle "+to_string(cycle)+": Overwrite HI-LO registers\n";
        overWriteHILO = true;
    }
    for (int i = 0; i < 32; i++) {
        a[i] = rs[i];
        b[i] = rt[i];
    }
    for(int i = 0;i <32 ;i++){
        if (a[i]) {
            temp = b;
            temp = temp << i;
            result=result+temp;
        }
    }
    for (int i = 0; i < 32;i++) {
        this->reg[32][i] = result[32+i];
        this->reg[33][i] = result[i];
    }
    ismult = true;
    //error detections.
}
//(1)
void Reg::EX_mfhi(bitset<32>* ex_dm_rd,int rd,int cycle){
    //bf detect write 0-->change ismult to false
    ismult = false;
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = this->reg[32];
}
void Reg::EX_mflo(bitset<32>* ex_dm_rd,int rd,int cycle){
    ismult = false;
    if (rd==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    *ex_dm_rd = this->reg[33];
}
// I inst.

//(1)(2)//signed
void Reg::EX_addi(bitset<32>* ex_dm_rt, bitset<32> rs, long i,int rt,int cycle){
    if (rt==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
    }

    int i_is_pos=zero,rs_is_pos=zero,rt_is_pos=zero;
    if(i > 0)i_is_pos = pos;
    else if(i < 0)i_is_pos = neg;
    if(this->BitsetToSigned32(rs)>0)rs_is_pos = pos;
    else if(this->BitsetToSigned32(rs)<0)rs_is_pos = neg;
    
    bitset<32>im(i);//signed extension

    if (rt!=0) {
        *ex_dm_rt = rs+im;
        if(this->BitsetToSigned32(*ex_dm_rt)>0)rt_is_pos=pos;
        else if(this->BitsetToSigned32(*ex_dm_rt)<0)rt_is_pos=neg;
        if (rs_is_pos==i_is_pos &&i_is_pos!=rt_is_pos) {
            //overflow
//            this->Error+="In cycle "+to_string(cycle)+": Number Overflow\n";
            numberOverflow = true;
        }
    }
    else {
        bitset<32>temp = rs+im;
        if(this->BitsetToSigned32(temp)>0)rt_is_pos=pos;
        if(this->BitsetToSigned32(temp)<0)rt_is_pos=neg;
        if (rs_is_pos==i_is_pos &&i_is_pos!=rt_is_pos) {
            //overflow
//            this->Error+="In cycle "+to_string(cycle)+": Number Overflow\n";
            numberOverflow = true;
        }
    }
}
//unsigned,(1)

void Reg::EX_addiu(bitset<32>* ex_dm_rt, bitset<32> rs, long i,int rt,int cycle){
    if (rt==0) {
//        this->Error+="In cycle "+to_string(cycle)+": Write $0 Error\n";
        return;
    }
    bitset<32>im(i);//signed extension
    *ex_dm_rt = rs+im;
}

//(1)(2)(4)(5)
//the memory index : 0,4,8,..
void Reg::EX_CalculateMemIndex(bitset<32>* accessAdr, long i, bitset<32> rs,int cycle){
    //$t = 4 bytes from Memory[$s + C(signed)]
    //check C is a multiple of 4
    //check if $s+C is not available --> don't know reg[rs] is viewed as signed or not

    int i_is_pos=zero,rs_is_pos=zero,index_is_pos=zero;
    bitset<32>im(i);//signed extension.
    if(BitsetToSigned32(rs)>0)rs_is_pos = pos;
    else if(BitsetToSigned32(rs)<0)rs_is_pos = neg;
    if(i>0)i_is_pos = pos;
    else if(i<0)i_is_pos = neg;
    bitset<32>index(rs+im);//0~1023
    if (BitsetToSigned32(index)>0)index_is_pos = pos;
    else if (BitsetToSigned32(index)<0)index_is_pos = neg;
    
    *accessAdr = index;
    if (rs_is_pos==i_is_pos &&i_is_pos!=index_is_pos) {//(2)
        numberOverflow = true;
    }
}


//(1)
void Reg::EX_lui(bitset<32>* ex_dm_rt, bitset<16> x,int rt,int cycle){
    //$t = C << 16
    // The lower bits of the register are set to 0.
    if (rt==0) {
        return;
    }
    bitset<32>rt_temp;
    for (int i = 0; i < 16; i++) {
        rt_temp[i] = 0;
    }
    for (int i = 16; i < 32; i++) {
        rt_temp[i] = x[i-16];
    }
    *ex_dm_rt = rt_temp;
}

//(1)
void Reg::EX_Andi(bitset<32>* ex_dm_rt, bitset<32> rs,long i,int rt,int cycle){//unsigned
    if (rt==0) {
        return;
    }
    bitset<32>x(i); // signed extends
    *ex_dm_rt = rs & x;
}

void Reg::EX_Ori(bitset<32>* ex_dm_rt, bitset<32> rs,long i,int rt,int cycle){//unsigned
    if (rt==0) {
        return;
    }
    bitset<32>x(i);
    *ex_dm_rt = rs | x;
}

void Reg::EX_Nori(bitset<32>* ex_dm_rt, bitset<32> rs,long i,int rt,int cycle){//unsigned
    if (rt==0) {
        return;
    }
    bitset<32>x(i);
    *ex_dm_rt = ~(rs | x);
}

//(1)
void Reg::EX_slti(bitset<32>* ex_dm_rt, bitset<32> rs,long i,int rt,int cycle){
    //    $t = ($s < C(signed) ), signed comparison
    if (rt==0) {
        return;
    }
    if (BitsetToSigned32(rs) < i) {
        *ex_dm_rt = 1;
    }
    else *ex_dm_rt = 0;
}

bool Reg::ID_beq(bitset<32> rs, bitset<32> rt, long i,int cycle){
    //    if ($s == $t) go to PC+4+4*C(signed)
    if (rs==rt) {
        this->reg[34] = pre_PC+bitset<32>(4)+(bitset<32>(i)<<2);//may cause problems
        PCisadjusted = true;
        return true;
    }
    return false;
}
bool Reg::ID_bne(bitset<32> rs, bitset<32> rt, long i,int cycle){
    //    if ($s != $t) go to PC+4+4*C(signed)
    if (rs!=rt) {
        this->reg[34] = pre_PC+bitset<32>(4)+(bitset<32>(i)<<2);
        PCisadjusted = true;
        return true;
    }
    return false;
}
bool Reg::ID_bgtz(bitset<32> rs, long i,int cycle){
    //if $s > 0 go to PC+4+4*C(signed)
    if (BitsetToSigned32(rs)> 0) {
        this->reg[34] = pre_PC+bitset<32>(4)+(bitset<32>(i)<<2);//may cause problems
        PCisadjusted = true;
        return true;
    }
    return false;
}
////////J inst.

//
void Reg::ID_j(bitset<26> adr,int cycle){
    //    PC = (PC+4)[31:28] | 4*C(unsigned)
    bitset<32> C4(0);
    bitset<32> temp(0);
    for(int i = 0 ; i < 26;i++)C4[i+2] = adr[i];
    for(int i = 0 ; i < 4 ;i++)temp[28+i]=(pre_PC+bitset<32>(4))[28+i];
    this->reg[34] = temp | C4;
    PCisadjusted = true;
}
void Reg::ID_jal(bitset<26> adr,bitset<32>* id_ex_31,int cycle){
    //$31 = PC + 4
    //    PC = (PC+4)[31:28] | 4*C(unsigned)
    *id_ex_31 = pre_PC+bitset<32>(4);
    bitset<32> C4(0);
    bitset<32> temp(0);
    for(int i = 0 ; i < 26;i++)C4[i+2] = adr[i];
    for(int i = 0 ; i < 4 ;i++)temp[28+i]=(pre_PC+bitset<32>(4))[28+i];
    this->reg[34] = temp | C4;
    PCisadjusted = true;
}

void Reg::DM_lw(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m){
    if ((unsigned long long)(index.to_ulong()+3) > 1023 || BitsetToSigned32(index) < 0)
    {
        d_MemAddressOverflow = true;
        halt = true;
    }
    if(index.to_ulong()%4!=0){
        d_MemMissalign = true;
        halt = true;
    }
    if (!d_MemAddressOverflow && !d_MemMissalign) {
        *dm_wb_rt = m->AdrToMem[int(index.to_ulong())];
    }
}
void Reg::DM_lh(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m){
    if ((unsigned long long)(index.to_ulong()+1) > 1023 || BitsetToSigned32(index) < 0)
    {
        d_MemAddressOverflow = true;
        halt = true;
    }
    if(index.to_ulong()%2!=0){
        d_MemMissalign = true;
        halt = true;
    }
    if (!d_MemAddressOverflow && !d_MemMissalign) {
        int which_data = int(index.to_ulong()/4*4);
        int begin = int(index.to_ulong()%4);//0 or 2
        bitset<32>temp = m->AdrToMem[which_data];//load half
        for (int i = 0; i < 32; i++) {
            if (i < 16) {
                (*dm_wb_rt)[i] = temp[16-begin*8+i];
            }
            else (*dm_wb_rt)[i] = (*dm_wb_rt)[15];//signed extension
        }
    }
}
void Reg::DM_lhu(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m){
    if ((unsigned long long)(index.to_ulong()+1) > 1023 || BitsetToSigned32(index) < 0)
    {
        d_MemAddressOverflow = true;
        halt = true;
    }
    if(index.to_ulong()%2!=0){
        d_MemMissalign = true;
        halt = true;
    }
    if (!d_MemAddressOverflow && !d_MemMissalign) {
        int which_data = int(index.to_ulong()/4*4);
        int begin = int(index.to_ulong()%4);//0 or 2
        bitset<32>temp = m->AdrToMem[which_data];//load half
        for (int i = 0; i < 32; i++) {
            if (i < 16) {
                (*dm_wb_rt)[i] = temp[16-begin*8+i];
            }
            else (*dm_wb_rt)[i] = 0;//signed extension
        }
    }
}
void Reg::DM_lb(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m){
    if ((unsigned long long)(index.to_ulong()) > 1023 || BitsetToSigned32(index) < 0)
    {
        d_MemAddressOverflow = true;
        halt = true;
    }
    if (!d_MemAddressOverflow) {
        int which_data = int(index.to_ulong()/4*4);
        int begin = int(index.to_ulong()%4);//0 1 2
        bitset<32>temp = m->AdrToMem[which_data];//load byte
        for (int i = 0; i < 32; i++) {
            if (i < 8) {
                (*dm_wb_rt)[i] = temp[24-begin*8+i];
            }
            else (*dm_wb_rt)[i]= (*dm_wb_rt)[7];//extension
        }
    }
}
void Reg::DM_lbu(bitset<32>index,bitset<32>* dm_wb_rt,Memory *m){
    if ((unsigned long long)(index.to_ulong()) > 1023 || BitsetToSigned32(index) < 0)
    {
        d_MemAddressOverflow = true;
        halt = true;
    }
    if (!d_MemAddressOverflow) {
        int which_data = int(index.to_ulong()/4*4);
        int begin = int(index.to_ulong()%4);//0 1 2
        bitset<32>temp = m->AdrToMem[which_data];//load byte
        for (int i = 0; i < 32; i++) {
            if (i < 8) {
                (*dm_wb_rt)[i] = temp[24-begin*8+i];
            }
            else (*dm_wb_rt)[i]= 0;//extension
        }
    }
}
void Reg::DM_sw(bitset<32>index,bitset<32> input,Memory *m){
    if ((unsigned long long)(index.to_ulong()+3) > 1023 || BitsetToSigned32(index) < 0)
    {
        d_MemAddressOverflow = true;
        halt = true;
    }
    if(index.to_ulong()%4!=0){
        d_MemMissalign = true;
        halt = true;
    }
    if (!d_MemAddressOverflow && !d_MemMissalign) {
        m->AdrToMem[int(index.to_ulong())] = input;
    }
}
void Reg::DM_sh(bitset<32>index,bitset<32> input,Memory *m){
    if ((unsigned long long)(index.to_ulong()+1) > 1023 || BitsetToSigned32(index) < 0)
    {
        d_MemAddressOverflow = true;
        halt = true;
    }
    if(index.to_ulong()%2!=0){
        d_MemMissalign = true;
        halt = true;
    }
    if (!d_MemAddressOverflow && !d_MemMissalign) {
        int which_data = int(index.to_ulong()/4*4);
        int begin = int(index.to_ulong()%4);//0 or 2
        for (int i = 0; i < 16; i++) {
            m->AdrToMem[which_data][16-begin*8+i] = input[i];
        }
    }
}
void Reg::DM_sb(bitset<32>index,bitset<32> input,Memory *m){
    if ((unsigned long long)(index.to_ulong()) > 1023 || BitsetToSigned32(index) < 0)
    {
        d_MemAddressOverflow = true;
        halt = true;
    }
    if (!d_MemAddressOverflow) {
        int which_data = int(index.to_ulong()/4*4);
        int begin = int(index.to_ulong()%4);//0 1 2 3
        for (int i = 0; i < 8; i++) {
            m->AdrToMem[which_data][24-begin*8+i] = input[i];
        }
    }
}

void Reg::WB_WriteReg(bitset<32>input, int des){
    if (des==0) {
        write0 = true;
    }
    else this->reg[des] = input;
}

