# TrafficEZ

Smart Traffic Solution using Computer Vision for Edge devices.

Author: Isidro Butaslac

## Installation

Check if `curl` is installed:

```bash
curl --version
```

If not, install with:

```bash
sudo apt-get install curl
```

Download the installer:

```bash
curl -L -o install.sh $(curl -s https://api.github.com/repos/sid410/TrafficEZ/releases/latest | grep browser_download_url | grep install.sh | cut -d '"' -f 4)
```

Make it executable then run:

```bash
chmod +x ./install.sh
./install.sh
```

## Documentation

<https://imb410.github.io/TrafficEZ-Documentation/>

## Environments guide

- Require **OpenCV 4.9.0** version or above.
- Code should be built on **Ubuntu 22.04.3 LTS x86_64** (headless server to be deployed).
- Use **CMake 3.22.1** or above (latest stable version from apt in the above Ubuntu version).
- For Linux toolchain, use the following versions or above:
  - GNU Make 4.3
  - gcc, g++ version 11.4.0
  - gdb version 12.1

## Useful Supplementary tools

RTSP streamer for debugging:

<https://github.com/sid410/RTSP-streamer>

Covert YOLO pytorch to ONNX models:

<https://github.com/sid410/ConvertYOLO-Pytorch-Onnx>

Instance Segmentation using ORT:

<https://github.com/sid410/YOLOv8-Instance-Segmentation-CPP>
