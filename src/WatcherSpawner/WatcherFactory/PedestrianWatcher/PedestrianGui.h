#ifndef PEDESTRIAN_GUI_H
#define PEDESTRIAN_GUI_H

#include "Gui.h"

class PedestrianGui : public Gui
{
public:
    void display(const std::string& streamName,
                 const std::string& calibName) const override;
};

#endif
