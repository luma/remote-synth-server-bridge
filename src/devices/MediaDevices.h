#ifndef SYNTH_BRIDGE_MEDIA_DEVICES_H_
#define SYNTH_BRIDGE_MEDIA_DEVICES_H_
#pragma once

#include <string>
#include <vector>
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/test/fakeconstraints.h"
#include "talk/media/devices/devicemanager.h"
#include "devices/MediaDeviceInfo.h"
#include "common/EventLoop.h"

typedef std::vector<cricket::Device> DeviceCollection;
typedef std::map<std::string, std::string> MediaConstraints;

class MediaDevices {
 public:
  explicit MediaDevices();
  ~MediaDevices();

  enum DeviceType {
    VIDEO,
    AUDIO
  };

  bool GetCaptureDevices(DeviceType type, DeviceCollection* devices);
  rtc::scoped_refptr<webrtc::MediaStreamInterface> GetMedia(MediaConstraints constraints);

 private:

  int32_t GetAudioDeviceIndex(std::string audioDeviceId);
  cricket::VideoCapturer* GetVideoCapturerById(std::string deviceId);

  rtc::scoped_ptr<cricket::DeviceManagerInterface> deviceManager_;
  rtc::scoped_refptr<webrtc::AudioDeviceModule> audioDevice_;
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pcFactory_;

  rtc::Thread *pcWorkerThread_;
  rtc::Thread *pcSignalingThread_;
};

#endif    // SYNTH_BRIDGE_MEDIA_DEVICES_H_
