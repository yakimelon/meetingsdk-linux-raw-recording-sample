//SendVideoRawData

#include "ZoomSDKVideoSource.h"
#include <iostream>
#include <thread> 
#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int video_play_flag = -1;
int width = WIDTH;
int height = HEIGHT;

void PlayVideoFileToVirtualCamera(IZoomSDKVideoSender* video_sender, const std::string& video_source)
{
    char* frameBuffer;
    int frameLen = height / 2 * 3 * width;
    frameBuffer = (char*)malloc(frameLen);

    const int fps = 30;
    const std::chrono::microseconds frameDuration(1000000 / fps); // 1 second = 1,000,000 microseconds

    // execute in a thread.
    while (video_play_flag > 0 && video_sender) {
        Mat frame;
        VideoCapture cap;
        cap.open(video_source);
        if (!cap.isOpened()) {
            cerr << "ERROR! Unable to open camera\n";
            video_play_flag = 0;
            break;
        }
        else {
            //--- GRAB AND WRITE LOOP
            std::cout << "Start grabbing" << endl;
            while (video_play_flag > 0)
            {

                auto start = std::chrono::high_resolution_clock::now();

                // wait for a new frame from camera and store it into 'frame'
                cap.read(frame);
                // check if we succeeded
                if (frame.empty()) {
                    cerr << "ERROR! blank frame grabbed\n";
                    break;
                }
                Mat resizedFrame;
                resize(frame, resizedFrame, Size(width, height), 0, 0, INTER_LINEAR);

                //covert Mat to YUV buffer
                Mat yuv;
                cv::cvtColor(resizedFrame, yuv, COLOR_BGRA2YUV_I420);
                char* p;
                for (int i = 0; i < height / 2 * 3; ++i) {
                    p = yuv.ptr<char>(i);
                    for (int j = 0; j < width; ++j) {
                        frameBuffer[i * width + j] = p[j];
                    }
                }
                SDKError err = ((IZoomSDKVideoSender*)video_sender)->sendVideoFrame(frameBuffer, width, height, frameLen, 0);
                if (err != SDKERR_SUCCESS) {
                    std::cout << "sendVideoFrame failed: Error " << err << endl;
                }

                auto end = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

                if (elapsed < frameDuration) {
                    std::this_thread::sleep_for(frameDuration - elapsed);
                    std::cout << "frame sleep " << endl;
                }
            }
            cap.release();
        }
    }
    video_play_flag = -1;
}

void ZoomSDKVideoSource::onInitialize(IZoomSDKVideoSender* sender, IList<VideoSourceCapability>* support_cap_list, VideoSourceCapability& suggest_cap)
{
    std::cout << "ZoomSDKVideoSource onInitialize waiting for turnOn chat command" << endl;
    video_sender_ = sender;
}

void ZoomSDKVideoSource::onPropertyChange(IList<VideoSourceCapability>* support_cap_list, VideoSourceCapability suggest_cap)
{
    std::cout << "onPropertyChange" << endl;
    std::cout << "suggest frame: " << suggest_cap.frame << endl;
    std::cout << "suggest size: " << suggest_cap.width << "x" << suggest_cap.height << endl;
    width = suggest_cap.width;
    height = suggest_cap.height;
    std::cout << "calculated frameLen: " << height / 2 * 3 * width << endl;
}

void ZoomSDKVideoSource::onStartSend()
{
    std::cout << "onVideoStartSend" << endl;
    // TODO: ここが実行されているか確認したい
    printf("video_play_flag: %d\n", video_play_flag);
    printf("video_sender_: %p\n", video_sender_);
    if (video_sender_ && video_play_flag != 1) {
        printf("映像送信処理中\n");
        while (video_play_flag > -1) {}
        video_play_flag = 1;
        thread(PlayVideoFileToVirtualCamera, video_sender_, video_source_).detach();
    }
    else {
        std::cout << "video_sender_ is null" << endl;
    }
}

void ZoomSDKVideoSource::onStopSend()
{
    std::cout << "onCameraStopSend" << endl;
    video_play_flag = 0;
}

void ZoomSDKVideoSource::onUninitialized()
{
    std::cout << "onUninitialized" << endl;
    video_sender_ = nullptr;
}

ZoomSDKVideoSource::ZoomSDKVideoSource(string video_source)
{
    video_source_ = video_source;
}

