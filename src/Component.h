//
// Created by Ivan Kishchenko on 12.12.2020.
//

#ifndef BOOST_COMPONENT_H
#define BOOST_COMPONENT_H

#include "UserDef.h"

class Component {
private:
    std::string _name;
public:
    explicit Component(std::string_view name) : _name() {}

    const std::string& name() {
        return _name;
    }
};


#endif //BOOST_COMPONENT_H
