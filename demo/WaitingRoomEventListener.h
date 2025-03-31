#pragma once
#include <meeting_service_components/meeting_waiting_room_interface.h>
#include <iostream>

class WaitingRoomEventListener : public ZOOM_SDK_NAMESPACE::IMeetingWaitingRoomEvent {
public:
    ZOOM_SDK_NAMESPACE::IMeetingService* m_pMeetingService_;

    WaitingRoomEventListener(ZOOM_SDK_NAMESPACE::IMeetingService* meetingService)
        : m_pMeetingService_(meetingService) {}

    void onWaitingRoomUserJoin(unsigned int userID) override {
        auto* ctrl = m_pMeetingService_->GetMeetingWaitingRoomController();
        if (ctrl) {
            std::cout << "[待機室] ユーザー " << userID << " を許可します" << std::endl;
            ctrl->AdmitToMeeting(userID);
        }
    }

    void onWaitingRoomUserLeft(unsigned int userID) override {}
    void onWaitingRoomPresetAudioStatusChanged(bool bAudioCanTurnOn) override {}
    void onWaitingRoomPresetVideoStatusChanged(bool bVideoCanTurnOn) override {}
    void onCustomWaitingRoomDataUpdated(CustomWaitingRoomData& bData, IWaitingRoomDataDownloadHandler* bHandler) override {}
    void onWaitingRoomUserNameChanged(unsigned int userID, const zchar_t* userName) override {}
    void onWaitingRoomEntranceEnabled(bool bIsEnabled) override {}
};
