//SendAudioRawData

#include <iostream>
#include <cstdint>
#include <gst/gst.h>
#include "rawdata/rawdata_audio_helper_interface.h"
#include "zoom_sdk.h"
#include "zoom_sdk_raw_data_def.h"


using namespace std;
using namespace ZOOMSDK;

class ZoomSDKVirtualAudioMicEvent :
	public IZoomSDKVirtualAudioMicEvent
{

private:
	IZoomSDKAudioRawDataSender* pSender_;
	GstElement* audio_sink_;
protected:

	/// \brief Callback for virtual audio mic to do some initialization.
/// \param pSender, You can send audio data based on this object, see \link IZoomSDKAudioRawDataSender \endlink.
	virtual void onMicInitialize(IZoomSDKAudioRawDataSender* pSender);

	/// \brief Callback for virtual audio mic can send raw data with 'pSender'.
	virtual void onMicStartSend();

	/// \brief Callback for virtual audio mic should stop send raw data.
	virtual void onMicStopSend();

	/// \brief Callback for virtual audio mic is uninitialized.
	virtual void onMicUninitialized();

public:
	ZoomSDKVirtualAudioMicEvent(GstElement* audio_sink);
};