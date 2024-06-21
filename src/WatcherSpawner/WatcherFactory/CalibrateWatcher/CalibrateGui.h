#ifndef CALIBRATE_GUI_H
#define CALIBRATE_GUI_H

#include "Gui.h"

class CalibrateGui : public Gui
{
public:
    void initialize(const std::string& streamName,
                    const std::string& calibName) override;
};

#endif
