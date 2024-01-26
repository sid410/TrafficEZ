add_library(HullTracker HullTracker.cpp HullTrackable.cpp)
setup_currdir_opencv(HullTracker)
setup_fps_helper(HullTracker)
