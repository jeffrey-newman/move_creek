//
//  MoveDown_Impl.cpp
//  move_creek
//
//  Created by Jeffrey Newman on 28/01/2015.
//  Copyright (c) 2015 University of Adelaide. All rights reserved.
//

#include <stdio.h>
#include "MoveDown.cpp"


template
std::tuple< Map_Int_SPtr, Map_Int_SPtr, std::vector<std::pair<Position, Position> > >
move_down(Map_Int_SPtr feature_map, Map_Double_SPtr dem_map, unsigned neighbour_size, unsigned adjacent_size, unsigned stackatop);