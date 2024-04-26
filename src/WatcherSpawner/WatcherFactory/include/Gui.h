#ifndef GUI_H
#define GUI_H

#include <string>

class Gui
{
public:
    virtual ~Gui() {}

    virtual void display(const std::string& streamName,
                         const std::string& calibName) = 0;
};

#endif
