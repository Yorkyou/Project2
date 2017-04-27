//
//  memory.hpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/16.
//  Copyright © 2017年 York You. All rights reserved.
//

#ifndef memory_hpp
#define memory_hpp

//
//  memory.cpp
//  Computer Architecture Project1
//
//  Created by York You on 2017/3/14.
//  Copyright © 2017年 York You. All rights reserved.

using namespace std;
#include <sstream>
#include <iostream>
#include <string>
#include <map>
#include <bitset>
namespace std{
    template < typename T > std::string to_string(const T&n)
    {
        std::ostringstream stm;
        stm << n;
        return stm.str();
    }
}
class Memory{
public:
    Memory(string Dbuffer,string Ibuffer);
    std::map< int,bitset<32> > AdrToMem;//use this map to store Data.(index-->bitset)
    std::map< int,bitset<32> > InstToMem;//use this map to store Inst.(index-->bitset)
    long d_size;//0~1023 == map.d_size()
    long i_size;

    bitset<32> initialSP,initialPC;
    bitset<32> getiSP();
    bitset<32> getiPC();
    long getIsize();
};
#endif /* memory_h */
