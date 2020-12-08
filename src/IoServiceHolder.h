//
// Created by Ivan Kishchenko on 08.12.2020.
//

#ifndef BOOST_IOSERVICEHOLDER_H
#define BOOST_IOSERVICEHOLDER_H

#include "UserDef.h"
#include <boost/serialization/singleton.hpp>

typedef boost::serialization::singleton<IoService> IoServiceHolder;

#endif //BOOST_IOSERVICEHOLDER_H
