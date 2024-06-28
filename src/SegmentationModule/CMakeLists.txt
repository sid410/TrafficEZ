add_library(
  SegmentationModule SegmentationMask.cpp VehicleSegmentationStrategy.cpp
                     PersonSegmentationStrategy.cpp)

setup_currdir_opencv(SegmentationModule)
setup_ort_api(SegmentationModule)
