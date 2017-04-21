//
//  MoveDown.h
//  move_creek
//
//  Created by Jeffrey Newman on 28/01/2015.
//  Copyright (c) 2015 University of Adelaide. All rights reserved.
//

#ifndef __move_creek__MoveDown__
#define __move_creek__MoveDown__

#include <stdio.h>
#include "MoveCreekTypes.h"

template <typename DataFormat1, typename DataFormat2>
std::tuple< boost::shared_ptr<Map_Matrix<DataFormat1> >, boost::shared_ptr<Map_Matrix<DataFormat1> >, std::vector<std::pair<Position, Position> > >
move_down(boost::shared_ptr<Map_Matrix<DataFormat1> > feature_map, boost::shared_ptr<Map_Matrix<DataFormat2> > dem_map, unsigned neighbour_size, unsigned adjacent_size, unsigned stackatop);




#endif /* defined(__move_creek__MoveDown__) */
