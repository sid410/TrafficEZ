#include "FPSHelper.h"
#include <opencv2/opencv.hpp>
#include <sys/timeb.h>

FPSHelper::FPSHelper()
    : _fpsstart(0)
    , _avgfps(0)
    , _fps1sec(0)
    , _startdur(0)
    , _avgdur(0)
{}

FPSHelper::~FPSHelper() {}

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) ||                  \
    defined(__WIN32__) || defined(WIN64) || defined(_WIN64) ||                 \
    defined(__WIN64__)
int FPSHelper::CLOCK()
{
    return clock();
}
#endif

#if defined(unix) || defined(__unix) || defined(__unix__) || defined(linux) || \
    defined(__linux) || defined(__linux__) || defined(sun) ||                  \
    defined(__sun) || defined(BSD) || defined(__OpenBSD__) ||                  \
    defined(__NetBSD__) || defined(__FreeBSD__) || defined __DragonFly__ ||    \
    defined(sgi) || defined(__sgi) || defined(__MACOSX__) ||                   \
    defined(__APPLE__) || defined(__CYGWIN__)
int FPSHelper::CLOCK()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec * 1e-6);
}
#endif

/**
 * @brief Call this method to start sampling a duration.
 */
void FPSHelper::startSample()
{
    _startdur = CLOCK();
}

/**
 * @brief Call this method to end sampling a duration.
 * @return the duration. Pass this new sample duration
 * to avgDuration().
 */
double FPSHelper::endSample()
{
    return CLOCK() - _startdur;
}

/**
 * @brief Averages a duration between two lines of code.
 * Need to call both startSample() and endSample() first.
 * @param newdur new sample of the duration from endSample().
 * @return an averaged duration (in milliseconds) resistant to fluctuations.
 */
double FPSHelper::avgDuration(double newdur)
{
    _avgdur = 0.98 * _avgdur + 0.02 * newdur;
    return _avgdur;
}

/**
 * @brief Calculates the average FPS.
 * Pair with printFps() or displayFps() to show in
 * the terminal or frame, respectively.
 * Alternatively, use the return value of this method.
 * @return the average fps resistant to fluctuations.
 */
double FPSHelper::avgFps()
{
    if(CLOCK() - _fpsstart > 1000)
    {
        _fpsstart = CLOCK();
        _avgfps = 0.3 * _avgfps + 0.7 * _fps1sec;
        _fps1sec = 0;
    }

    _fps1sec++;
    return _avgfps;
}

/**
 * @brief Prints the average fps in terminal.
 */
void FPSHelper::printFps() const
{
    std::cout << "FPS: " << _avgfps << "\n";
}

/**
 * @brief Overlays the average fps in the passed frame.
 * Bottom-left corner of the text string in the image.
 * @param frame the frame to put fps text.
 * @param x (optional) x-coordinate of the bottom-left corner of the text string in the image.
 * @param y (optional) y-coordinate of the bottom-left corner of the text string in the image.
 */
void FPSHelper::displayFps(cv::Mat& frame, int x, int y) const
{
    std::ostringstream fpsText;

    fpsText << "FPS: " << std::fixed << std::setprecision(0) << _avgfps;
    cv::putText(frame,
                fpsText.str(),
                cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX,
                1.0,
                cv::Scalar(0, 255, 0),
                2);
}
