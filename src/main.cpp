#include <opencv2/opencv.hpp>
#include <iostream>
#include "version_config.h"

int main()
{
    // Print these to inform the project and opencv version instance
    std::cout << "Project version: " << PROJECT_NAME_VER << std::endl;
    std::cout << "OpenCV version: " << CV_VERSION << std::endl;

    return 0;
}