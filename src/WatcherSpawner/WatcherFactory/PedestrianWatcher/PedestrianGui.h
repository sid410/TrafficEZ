#ifndef PEDESTRIAN_GUI_H
#define PEDESTRIAN_GUI_H

#include "Gui.h"

class PedestrianGui : public Gui
{
public:
    void initialize(const std::string& streamName,
                    const std::string& calibName) override;

    void display() override;
};

#endif
