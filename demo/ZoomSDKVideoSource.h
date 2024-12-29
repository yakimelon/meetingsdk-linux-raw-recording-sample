//SendVideoRawData


#include <string>
#include <gst/gst.h>
#include "rawdata/rawdata_video_source_helper_interface.h"

constexpr auto WIDTH = 640;
constexpr auto HEIGHT = 480;

using namespace std;
using namespace ZOOMSDK;

class ZoomSDKVideoSource :
	public IZoomSDKVideoSource
{
private:
	IZoomSDKVideoSender* video_sender_;
	GstElement* video_sink_;
protected:
	virtual	void onInitialize(IZoomSDKVideoSender* sender, IList<VideoSourceCapability >* support_cap_list, VideoSourceCapability& suggest_cap);
	virtual void onPropertyChange(IList<VideoSourceCapability >* support_cap_list, VideoSourceCapability suggest_cap);
	virtual void onStartSend();
	virtual void onStopSend();
	virtual void onUninitialized();
public:
	ZoomSDKVideoSource(GstElement* video_sink);
};

