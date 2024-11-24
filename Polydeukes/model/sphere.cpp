//
//  sphere.cpp
//  Polydeukes
//
//  Created by Lawrence Berardelli on 2024-05-19.
//

#include <stdio.h>
#include "sphere.h"

SphereFactory* SphereBuilder::sphereFactory = 0;
SphereBuilder* SphereBuilder::instance = 0;
