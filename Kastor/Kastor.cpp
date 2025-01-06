//
//  Kastor.cpp
//  Kastor
//
//  Created by Lawrence Berardelli on 2024-12-16.
//

#include <iostream>
#include "Kastor.hpp"
#include "KastorPriv.hpp"

void Kastor::HelloWorld(const char * s)
{
    KastorPriv *theObj = new KastorPriv;
    theObj->HelloWorldPriv(s);
    delete theObj;
};

void KastorPriv::HelloWorldPriv(const char * s) 
{
    std::cout << s << std::endl;
};

