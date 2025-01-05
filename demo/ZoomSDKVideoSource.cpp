//SendVideoRawData

#include "ZoomSDKVideoSource.h"
#include <iostream>
#include <thread> 
#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include <vector>
#include <fstream>

using namespace std;

int video_play_flag = -1;
int width = WIDTH;
int height = HEIGHT;

void PlayVideoFileToVirtualCamera(GstElement* video_sink, IZoomSDKVideoSender* video_sender)
{
	// フレームサイズを計算（I420 フォーマットの場合）
	const int frameLen = height / 2 * 3 * width;
	char* frameBuffer = (char*)malloc(frameLen);

	if (!frameBuffer) {
		std::cerr << "Failed to allocate video frame buffer" << std::endl;
		return;
	}

	std::cout << "Video frame buffer allocated successfully." << std::endl;

	GstSample* video_sample = nullptr;

	// ローカルファイルに保存するためのバッファ
	const int fps = 60;
	const int seconds_to_record = 5;
	const int total_frames_to_record = fps * seconds_to_record;
	std::vector<char> recordingBuffer(frameLen * total_frames_to_record);

	int frame_count = 0;

	// 映像フレームのループ処理
	while (video_play_flag > 0 && video_sender) {
		// appsink から映像サンプルを取得
		video_sample = gst_app_sink_try_pull_sample(GST_APP_SINK(video_sink), GST_SECOND / fps);
		if (!video_sample) {
			std::cerr << "No video sample available. Retrying..." << std::endl;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps)); // 短時間待機して再試行
			continue;
		}

		// サンプルからバッファを取得
		GstBuffer* video_buffer = gst_sample_get_buffer(video_sample);
		GstMapInfo video_map;

		// バッファをマップしてフレームデータを取得
		if (!gst_buffer_map(video_buffer, &video_map, GST_MAP_READ)) {
			std::cerr << "Failed to map video buffer." << std::endl;
			gst_sample_unref(video_sample);
			continue;
		}

		// デバッグ: バッファサイズの確認
		std::cout << "Video buffer mapped. Size: " << video_map.size << std::endl;

		memcpy(frameBuffer, video_map.data, frameLen);

		// Zoom SDK に映像フレームを送信
		SDKError video_err = video_sender->sendVideoFrame(frameBuffer, width, height, frameLen, 0);
		if (video_err != SDKERR_SUCCESS) {
			std::cerr << "Failed to send video frame: " << video_err << std::endl;
		} else {
			std::cout << "Video frame sent successfully." << std::endl;
		}

		gst_buffer_unmap(video_buffer, &video_map);
		gst_sample_unref(video_sample);

		// フレーム送信間隔を調整 (60fps)
		std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
	}

	// フレームバッファの解放
	free(frameBuffer);
	std::cout << "Video frame buffer freed." << std::endl;

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

// SDKの仕様でカメラを Unmute すると onStartSend が呼ばれる
void ZoomSDKVideoSource::onStartSend()
{
    std::cout << "onVideoStartSend" << endl;
    printf("video_play_flag: %d\n", video_play_flag);
    printf("video_sender_: %p\n", video_sender_);
    if (video_sender_ && video_play_flag != 1) {
        printf("映像送信処理前 #1\n");
        while (video_play_flag > -1) {}
		printf("映像送信処理前 #2\n");
        video_play_flag = 1;
        thread(PlayVideoFileToVirtualCamera, video_sink_, video_sender_).detach();
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

ZoomSDKVideoSource::ZoomSDKVideoSource(GstElement* video_sink)
{
	video_sink_ = video_sink;
}

