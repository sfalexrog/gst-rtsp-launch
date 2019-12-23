# gst-rtsp-launch

A minimalistic, `gst-launch-1.0`-like RTSP server. Based on the [gst-rtsp-server `test-launch` example](https://github.com/GStreamer/gst-rtsp-server/blob/master/examples/test-launch.c).

## Building

You need to have [CMake](https://cmake.org/) and [gst-rtsp-server](https://github.com/GStreamer/gst-rtsp-server) (and its build dependencies) built and installed. If you are running Debian or Ubuntu, install the following packages (provided as a bash command for copy-paste convenience):

```bash
sudo apt install -y cmake libgstrtspserver-1.0-dev libglib2.0-dev libgstreamer1.0-dev
```

Go to the source directory and run:

```bash
mkdir build
cd build
cmake ..
make
```

You may run the server from the build directory:

```bash
cd src
./gst-rtsp-launch --help
```

...or install the binary in your system:

```bash
sudo make install
gst-rtsp-launch --help
```

## Running

`gst-rtsp-launch` expects the following parameters:

```bash
gst-rtsp-launch [-p|--port=8554] [-e|--endpoint=video] [<various GStreamer options>] "( <GStreamer pipeline> )"
```

* `-p`/`--port=` - specify a different RTSP port (default is 8554);
* `-e`/`--endpoint=` - specify a different RTSP URL ending (default is `video`);
* `"(<GStreamer pipeline>)"` is your GStreamer pipeline, ending in `rtp*pay` (typically `rtph264pay`). Note that it must be provided in quotes and with brackets.

Your rtp payload must have a name of `pay0`, otherwise it won't be transmitted.

### Examples

Video test source -> x264 encoder, URL ends with `/test`:

```bash

gst-rtsp-launch --endpoint=test "( videotestsrc ! x264enc ! rtph264pay name=pay0 pt=96 )"
```

Video4Linux2 source (320x240, 30 fps) -> Video4Linux2 encoder (with extra parameters):

```bash
gst-rtsp-launch "( v4l2src device=/dev/video0 ! video/x-raw,framerate=30/1,width=320,height=240 ! v4l2h264enc output-io-mode=4 extra-controls=\"encode,frame_level_rate_control_enable=1,h264_profile=4,h264_level=13,video_bitrate=300000,h264_i_frame_period=5;\" ! rtph264pay name=pay0 pt=96 )"
```

## Troubleshooting

Note that your pipeline may silently fail (or only fail when a client is connected to the RTSP server). The pipeline is being run lazily, so an erroneous pipeline won't crash until a client connects. Be sure to check your pipelines with `gst-launch-1.0`. Use `autovideosink` before any encoders with a monitor connected to your streaming device to make sure your video capture works. Use `udpsink` after `rtph264pay` and send your video to a remote machine to make sure your encoder works. All typical GStreamer-related advices apply.
