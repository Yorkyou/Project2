//
//  memory.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.

using namespace std;
#include "memory.hpp"

Memory::Memory(string Dbuffer,string Ibuffer){
    AdrToMem.clear();
    InstToMem.clear();
    int it=0;//go through buffer
    for (int index = 0; index < 1023; index+=4) {
        AdrToMem[4*index] = 0;
        InstToMem[4*index] = 0;
    }
    
    bitset<32> data(Dbuffer.substr(it,32));
    initialSP = data;//SP done!
    it+= 32;
    data=bitset<32>(Dbuffer.substr(it,32));
    d_size = data.to_ulong();//d_size done

    //up to 1023****
    for (int index=0; index < d_size ;index++) {
        it+=32;
        data=bitset<32>(Dbuffer.substr(it,32));
        AdrToMem[4*index] = data;//注意bigendian!!!
    }
    //Instruction set initialization
    it = 0;
    data=bitset<32>(Ibuffer.substr(it,32));
    initialPC = data;//unsigned
    it+=32;

    data=bitset<32>(Ibuffer.substr(it,32));
    i_size = data.to_ulong();//i_size
    //up to 1023-->資料從initial pc開始填寫
    int index=int(initialPC.to_ulong());
    for (int i = 0; i < i_size; i++) {
        it+=32;
        data=bitset<32>(Ibuffer.substr(it,32));
        InstToMem[index] = data;//270,274,...
        index+=4;
    }
}

bitset<32> Memory::getiSP(){return initialSP;}
bitset<32> Memory::getiPC(){return initialPC;}
long Memory::getIsize(){return i_size;}
