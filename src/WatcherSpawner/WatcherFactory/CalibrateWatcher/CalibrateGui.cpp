#include "CalibrateGui.h"
#include "CalibrateVideoStreamer.h"
#include "TrimPerspective.h"
#include "WarpPerspective.h"

void CalibrateGui::display(const std::string& streamName,
                           const std::string& calibName) const
{
    CalibrateVideoStreamer calibrateStreamer;
    TransformPerspective* transformPerspective; // to point between warp/trim
    WarpPerspective warpPerspective;
    TrimPerspective trimPerspective;

    cv::Mat inputFrame;
    cv::Mat previewFrame;

    cv::String calibWindow = "Calibration Window";
    cv::String previewWindow = "Preview Perspective";

    bool previewToggle = false;
    bool perspectiveToggle = false;

    if(!calibrateStreamer.openVideoStream(streamName))
        return;

    calibrateStreamer.constructStreamWindow(calibWindow);
    calibrateStreamer.initCalibrationPoints(calibWindow);

    while(calibrateStreamer.settingCalibrationPoints(inputFrame))
    {
        // always show calib points and frame
        calibrateStreamer.showCalibrationPoints(inputFrame);
        cv::imshow(calibWindow, inputFrame);

        // only show preview based on logic in 'p' switch
        if(previewToggle &&
           calibrateStreamer.applyFrameRoi(
               inputFrame, previewFrame, *transformPerspective))
            cv::imshow(previewWindow, previewFrame);

        // key press logic for calibration
        int key = cv::waitKey(30);
        switch(key)
        {
        case 27: // 'Esc' key to exit by interruption
            std::cout << "Calibration interrupted.\n";
            cv::destroyAllWindows();
            return;

        case 'r': // 'r' key to reset points
        case 'R':
            calibrateStreamer.resetCalibrationPoints();
            break;

        case 's': // 's' key to save and exit successfully
        case 'S':
            calibrateStreamer.saveCalibrationData(calibName);
            break;

        case 'p': // 'p' key to preview between perspectives
        case 'P':
            if(!calibrateStreamer.haveSetFourPoints())
                break;

            previewToggle = !previewToggle;
            if(previewToggle)
            {
                perspectiveToggle = !perspectiveToggle;

                transformPerspective =
                    perspectiveToggle
                        ? static_cast<TransformPerspective*>(&warpPerspective)
                        : static_cast<TransformPerspective*>(&trimPerspective);

                calibrateStreamer.initializePreview(inputFrame,
                                                    *transformPerspective);
            }
            else
                cv::destroyWindow(previewWindow);
            break;
        }
    }

    cv::destroyAllWindows();
}
