//
//  Types.h
//  move_creek
//
//  Created by a1091793 on 23/01/2015.
//  Copyright (c) 2015 University of Adelaide. All rights reserved.
//

#ifndef move_creek_Types_h
#define move_creek_Types_h

#include <vector>
#include <tuple>
#include <boost/shared_ptr.hpp>
#include "Map_Matrix.h"

template <typename DataType>
using Map_Matrix_SPtr = boost::shared_ptr<Map_Matrix<DataType> >;

typedef std::pair<int, int> Position;
typedef std::vector<Position> Set;

typedef Map_Matrix<double> Map_Double;
typedef boost::shared_ptr<Map_Double> Map_Double_SPtr;

typedef Map_Matrix<int32_t> Map_Int;
typedef boost::shared_ptr<Map_Int> Map_Int_SPtr;

typedef std::vector<std::pair<Position, Position> > Move_Log;

#endif
