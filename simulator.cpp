//
//  main.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/6.
//  Copyright © 2017年 York You. All rights reserved.
//
#include "memory.hpp"
#include "register.hpp"
#include "instruction.hpp"
#include "stage.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>


string HextoBinary(char);
char inttohex(int i);

string ReadfileGetCode(string filename)
{
    std::ifstream file(filename.c_str());
    streampos size=0;
    char *buffer_temp;
    string buffer;
    if(file.is_open()){
        file.seekg (0, ios::end);
        size = file.tellg();
        buffer_temp = new char[size];
        file.seekg(0,ios::beg);
        file.read(buffer_temp, size);
        for (int i = 0; i < size; i++) {
            //cout << buffer_temp[i] <<" ";
            int code = buffer_temp[i];
            char x1, x2;
            if(code<0) code = 256+code;//two's complement
            x2 = inttohex(code%16); code /= 16;
            x1 = inttohex(code);
            buffer.append(HextoBinary(x1));
            buffer.append(HextoBinary(x2));
        }
        file.close();
    }
    return buffer;
}


int main(int argc, const char * argv[]) {//open data
    string Dbuffer,Ibuffer,Assembly;
    Dbuffer = ReadfileGetCode("dimage.bin");
    Ibuffer = ReadfileGetCode("iimage.bin");
    ofstream error("error_dump.rpt"),snapshot("snapshot.rpt"),Ass("Code.s");
    Memory m(Dbuffer,Ibuffer);
    Reg r(m.getiSP(),m.getiPC());
    Instruction in;
    stage s;
    in.StartInstrctuion(&r,&m,&s);
    error << in.getError();
    snapshot << in.getOut();
    //cout << "\n\nOut:\n" << in.getOut() << "\n\nError:\n" << in.getError();
    return 0;
}
string HextoBinary(char ch){
    switch (ch) {
        case '0':return "0000";
        case '1':return "0001";
        case '2':return "0010";
        case '3':return "0011";
        case '4':return "0100";
        case '5':return "0101";
        case '6':return "0110";
        case '7':return "0111";
        case '8':return "1000";
        case '9':return "1001";
        case 'a':return "1010";
        case 'b':return "1011";
        case 'c':return "1100";
        case 'd':return "1101";
        case 'e':return "1110";
        case 'f':return "1111";
        default: return "0000";
    }
}
char inttohex(int i){
    if(i>=0 && i<= 9) return i+'0';
    else return i-10+'a';
}
