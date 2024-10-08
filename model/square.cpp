//
//  square.cpp
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2024-04-29.
//

#include <stdio.h>
#include "square.h"

SquareFactory* SquareBuilder::squareFactory = 0;
SquareFactory* IconBuilder::squareFactory = 0;
int Icon::count = 0;
