add_library(OrtApiWrapper AutoBackendOnnx.cpp OnnxModelBase.cpp YoloUtils.cpp)

setup_currdir_opencv(OrtApiWrapper)
setup_onnxruntime(OrtApiWrapper)
