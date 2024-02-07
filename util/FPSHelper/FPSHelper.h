#ifndef FPS_HELPER_H
#define FPS_HELPER_H

namespace cv
{
class Mat; // Forward declaration for cv::Mat to use displayFps()
}

/**
 * @brief Helper class for measuring, displaying, or printing FPS.
 * Reference: https://stackoverflow.com/questions/21919912/getting-current-fps-of-opencv/
 */
class FPSHelper
{
public:
    FPSHelper();
    ~FPSHelper();

    void startSample();
    double endSample();
    double avgDuration(double newdur);

    double avgFps();
    void printFps() const;
    void displayFps(cv::Mat& frame, int x = 10, int y = 30) const;

private:
    int CLOCK();

    double _startdur;
    double _avgdur;

    int _fpsstart;
    double _avgfps;
    double _fps1sec;
};

#endif
