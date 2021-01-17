//
// Created by Ivan Kishchenko on 17.01.2021.
//

#ifndef BOOST_COMMAND_H
#define BOOST_COMMAND_H


class Command {
public:
    virtual ErrorFuture execute() = 0;
};


#endif //BOOST_COMMAND_H
