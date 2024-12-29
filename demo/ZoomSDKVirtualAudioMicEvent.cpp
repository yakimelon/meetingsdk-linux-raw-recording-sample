//SendAudioRawData
#include <iostream>
#include <cstdint>
#include <fstream>
#include <cstring>
#include <cstdio>
#include <vector>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include "rawdata/rawdata_audio_helper_interface.h"
#include "ZoomSDKVirtualAudioMicEvent.h"
#include "zoom_sdk_def.h" 

#include <thread>
#include <chrono>  // for sleep

using namespace std;
using namespace ZOOM_SDK_NAMESPACE;

int audio_play_flag = -1;



void PlayAudioFileToVirtualMic(GstElement* audio_sink, IZoomSDKAudioRawDataSender* audio_sender)
{
	GstSample* audio_sample = nullptr;

	// 音声フレームのループ処理
	while (audio_play_flag > 0 && audio_sender) {
		// appsink から音声サンプルを取得
		audio_sample = gst_app_sink_try_pull_sample(GST_APP_SINK(audio_sink), GST_SECOND / 100);
		if (audio_sample) {
			// サンプルからバッファを取得
			GstBuffer* audio_buffer = gst_sample_get_buffer(audio_sample);
			GstMapInfo audio_map;

			// バッファをマップして音声データを取得
			if (gst_buffer_map(audio_buffer, &audio_map, GST_MAP_READ)) {
				// Zoom SDK に音声データを送信
				SDKError audio_err = audio_sender->send(reinterpret_cast<char*>(audio_map.data), audio_map.size, 44100);
				if (audio_err != SDKERR_SUCCESS) {
					std::cerr << "Failed to send audio data: " << audio_err << std::endl;
				}
				gst_buffer_unmap(audio_buffer, &audio_map);
			}

			gst_sample_unref(audio_sample);
		}
	}
}

/// \brief Callback for virtual audio mic to do some initialization.
/// \param pSender, You can send audio data based on this object, see \link IZoomSDKAudioRawDataSender \endlink.
void ZoomSDKVirtualAudioMicEvent::onMicInitialize(IZoomSDKAudioRawDataSender* pSender) {
	pSender_ = pSender;
	printf("pSender_: %p\n", pSender_);
	printf("ZoomSDKVirtualAudioMicEvent OnMicInitialize, waiting for turnOn chat command2\n");
}

/// \brief Callback for virtual audio mic can send raw data with 'pSender'.
void ZoomSDKVirtualAudioMicEvent::onMicStartSend() {
	printf("onMicStartSend\n");
	std::cout << "onStartSend" << std::endl;
	printf("audio_play_flag: %d\n", audio_play_flag);
	printf("pSender_: %p\n", pSender_);
	if (pSender_ && audio_play_flag != 1) {
		printf("音声送信処理前: #1\n");
		while (audio_play_flag > -1) {}
		printf("音声送信処理前: #2\n");
		audio_play_flag = 1;
		thread(PlayAudioFileToVirtualMic, audio_sink_, pSender_).detach();
	}
}

/// \brief Callback for virtual audio mic should stop send raw data.
void ZoomSDKVirtualAudioMicEvent::onMicStopSend() {
	printf("onMicStopSend\n");
	audio_play_flag = 0;
}
/// \brief Callback for virtual audio mic is uninitialized.
void ZoomSDKVirtualAudioMicEvent::onMicUninitialized() {
	std::cout << "onUninitialized" << std::endl;
	pSender_ = nullptr;
}

ZoomSDKVirtualAudioMicEvent::ZoomSDKVirtualAudioMicEvent(GstElement* audio_sink)
{
	audio_sink_ = audio_sink;
}
