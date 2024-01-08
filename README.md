# TrafficEZ

Checklist in the project progress below are automatically updated in the [Public Repository](https://github.com/sid410/TrafficEZ-Updates) (everytime there is a push on dev/\* branch).

**Warning:** Do not clone the public repo as it will periodically rebase and force push.

---

Early development version.

Author: Isidro Butaslac

Affiliation: USTP-RSPOT IIoT Lab

## MVP: to-do for release/v1.0.0

- [ ] main.cpp

  - [x] print opencv and project version.
  - [x] add argument parser.
    - [x] number of car ROIs.
    - [x] number of pedestrian ROIs.
    - [x] debug toggle and view help.
    - [ ] check if total ROI <= number of cores.
  - [x] create TrafficManager object.

- [ ] VideoStreamer

  - [x] read from mp4 files (for debugging).
  - [ ] read from RTSP stream (for production) by loading environment variables from rtsp_links config file.
  - [x] trim and warp frame based on the calib_points yaml.
  - [x] return successfully read and initialized before anything can be changed to warped frame.
  - [x] able to switch strategy between Warp and Trim for TransformPerspective interface.

- [ ] CalibrateVideoStreamer (Only run once during calibration phase, assuming installed cameras don't move)

  - [x] Inherit from VideoStreamer: openVideoStream, constructStreamWindow, getNextFrame, readCalibrationPoints, initializePerspectiveTransform, warpFrame.
  - [ ] show frame that can also be accessed remotely (SSH).
  - [x] setCalibrationPointsFromMouse: click four points (resettable if unhappy) to define the image transformation matrix for warping the perspective to bird's eye view.
  - [ ] input the length and width of these corresponding lane points.
  - [ ] saveCalibrationPoints: save the corresponding {stream-name, four-calibration-points} dictionary to a lane_calib config file, together with the length and width of lane.
  - [ ] (optional) add a line guide calculated from Hough line transform to snap to.

- [ ] HullRecognitionModule

  - [ ] switch between debug mode (with imshow) and release mode (no imshow).

  - [ ] create HullDetector

    - [ ] read cv_params config file to load constants of CV pipeline.
    - [ ] pre-process frame with grayscale and gaussian blur.
    - [ ] apply MOG2 background subtraction.
    - [ ] filter out shadows.
    - [ ] apply morphological operation (dilation then erosion, i.e. closing morph) with different kernels.
    - [ ] find contours and store their moments.

  - [ ] create HullTracker

    - [ ] get reference of moments of contours from HullDetector.
    - [ ] match existing tracked hulls by comparing current and previous hulls info [centroid, area] that is calculated from contour moments.
    - [ ] add new tracked hulls with unique ID, but reset count every start of green light change.
    - [ ] remove duplicates (same hull but assigned multiple IDs) and inactive tracked hulls.
    - [ ] return reliable tracked hulls.

  - [ ] create HullParametersBlender, to manually find CV parameters to get best Hull tracking results.
    - [ ] inherit from HullDetector, but with adjustable parameters.
    - [ ] create trackbar to adjust these parameters while showing all stacked frames as feedback.
    - [ ] save the adjusted parameters to the cv_params config file.

- [ ] TrafficDensityEstimator

  - [ ] return traffic density based if the state message from parent was Green, RedSet, or RedGet (IMPORTANT: this is only applicaple for car traffic, still not sure how to calculate together with the pedestrian topic).
    - [ ] Green: return nothing but confirmation message.
    - [ ] RedSet: return density of previous green light.
    - [ ] RedGet: return density of current red light.
  - [ ] calculate density of previous green light:
    - [ ] get the accumulated total hull area that crossed finish line (define this line later, maybe 80% of the length?).
    - [ ] get the start and end time of green light, then divide by the accumulated total hull area to get average traffic flow.
    - [ ] get the average speed parallel to lane by calculating a moving average, for each frame, delta centroid divided by delta frame_time. Make sure to get the vector projection parallel to lane.
    - [ ] divide average traffic flow by average speed and width of lane, and return this value as density of previous green light.
  - [ ] calculate density of current red light:
    - [ ] invoke a countStationaryCars from the StationaryDetectorModule to get the number of cars inside an ROI for one frame.
    - [ ] divide the number of cars by the length and width of lane to get density of current red light.

- [ ] StationaryDetectorModule

  - [ ] have not decided yet what to use to detect stationary cars during red light.
    - [ ] the classic Haar Cascade classifiers.
    - [ ] YOLOv8 for edge model.
  - [ ] This module should also be used for pedestrian detection/counting.

- [ ] TrafficManager

  - [x] contructor to be initialized in main (should be initialized with number of ROIs for car and for pedestrian inputted as arguments).
  - [ ] load truth_table config file mapping the logic of green/red lights to lanes.
  - [ ] spawn processes (parallelized) based on the number of car + pedestrian ROIs.
  - [ ] create pipes to communicate state variables from parent to child, and communicate traffic density information from child to parent.
  - [ ] calculate traffic light timings based on results of received density info from children.
  - [ ] block until time for state change, then broadcast to the appropriate children processes their next state.
  - [ ] output signal to outside traffic light control.
  - [ ] (optional) create a traffic light control simulator to receive the signal.

- [ ] WatcherSpawner

  - [x] Create abstract factory that handles the spawning of Watcher objects.
  - [x] Create spawner interface for client side so factory remains hidden.
  - [ ] Implement the following concerete Watchers:
    - [x] Vehicle Watcher Gui
    - [ ] Vehicle Watcher Headless
    - [ ] Pedestrian Watcher Gui
    - [ ] Pedestrian Watcher Headless
    - [x] Calibrate Watcher Gui
    - [ ] Calibrate Watcher Headless

- [x] (Optional) FPSHelper: A helper class for easily display fps in either terminal or directly overlaid in frame. There are also methods for sampling the duration of how much time it takes to execute between lines of code.

- [ ] (Optional) HullDetectionOptimizer: To automize the manually set HullParametersBlender.

- [ ] Setup CMake
- [ ] Setup branch protection for `release/`

- [ ] Setup workflows

  - [ ] [Setup OpenCV action](https://github.com/Dovyski/setup-opencv-action).
  - [ ] Build and test CMake project on multiple platforms.

- [ ] Add test edge cases not covered by defaults
- [ ] Check threads and CPU usage in release
- [ ] Documentation with Doxygen

## Environments guide

- Write in **C++11** to conform to OpenCV coding style.
- Require **OpenCV 4.8.1** version (latest as of 2023/12/1) or above.
- Code should be cross-platform, but prioritize build on **Ubuntu 22.04.3 LTS** (headless server to be deployed).
- Use **CMake 3.22.1** or above (latest stable version from apt in the above Ubuntu version).
- For Linux toolchain, use the following versions or above:
  - GNU Make 4.3
  - gcc, g++ version 11.4.0
  - gdb version 12.1
- For Windows, maybe try the following, in descending priority:
  - Ninja
  - MSYS Makefiles
  - MinGW Makefiles
  - Visual Studio 16/17 (2019/2022)

## Git convention style guide

- branch naming convention
  - long-term (M = Major, m = Minor, p = patch)
    - release/vM.m.p (protected)
    - dev/vM.m.p
  - short-term
    - feature/reference/description-in-kebab-case
    - test/...
    - bugfix/...
    - docs/...
- commits convention (please [read here](https://medium.com/@naandalist/creating-a-git-commit-message-convention-for-your-team-acb4b3edfc44) for reference)
  - commit title
    - feat: [write here descriptive subject]
    - fix: ...
    - docs: ...
    - style: ...
    - refactor: ...
    - test: ...
    - chore: ...
  - commit description (optional)
    - if title's subject is not descriptive enough, provide here more details

## Naming conventions

- Classes/Structs: PascalCase
- variables/functions: camelCase
- constants: ALL_CAPS
- namespace: lowercase, no underscores
- Class member prefixes: no need

dir names:

- Source files: PascalCase except `main.cpp`
- Header files: PascalCase
- Include Guard in Header files: ALL_CAPS separated with underscores
- Config files: snake_case
- Text files: snake_case except `CMakeLists.txt`
- Test files: start with `test_` then snake_case
- Resource files: snake_case
- Release builds: TrafficEZ-M.m.p-Linux64 or TrafficEZ-M.m.p-Win64.exe

## Styling/Formatting conventions

Please use clang formatter and load the .clang-format settings in root.
