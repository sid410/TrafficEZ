#ifndef GUI_H
#define GUI_H

#include <string>

class Gui
{
public:
    virtual void display(const std::string& streamName,
                         const std::string& calibName) const = 0;
};

#endif
