#include <memory>
#include "talk/app/webrtc/videosourceinterface.h"
#include "webrtc/modules/audio_device/audio_device_impl.h"
#include "talk/app/webrtc/test/fakeconstraints.h"
#include "devices/MediaDevices.h"
#include "common/Guid.h"

MediaDevices::MediaDevices()
  : pcWorkerThread_(new rtc::Thread()),
    pcSignalingThread_(new rtc::Thread()) {

  assert(NULL != pcSignalingThread_ && NULL != pcWorkerThread_);

  pcWorkerThread_->Start();
  pcSignalingThread_->Start();

  audioDevice_ = webrtc::CreateAudioDeviceModule(0,
                    webrtc::AudioDeviceModule::kPlatformDefaultAudio);

  if (audioDevice_->Init() < 0) {
    ERROR("Cannot create audio device module");
    // @todo handle this better
    return;
  }

  pcFactory_ = webrtc::CreatePeerConnectionFactory(
          pcWorkerThread_, pcSignalingThread_, audioDevice_, nullptr, nullptr);

  if (!pcFactory_.get()) {
    ERROR("Could not create PeerConnectionFactory");
    // @todo some crazy shit
    return;
  }

  deviceManager_.reset(cricket::DeviceManagerFactory::Create());

  if (false == deviceManager_->Init()) {
    ERROR("Cannot create device manager");
    // @todo handle this better
    return;
  }
}

MediaDevices::~MediaDevices() {
  if (deviceManager_) {
    deviceManager_->Terminate();
  }

  deviceManager_ = nullptr;
  audioDevice_ = nullptr;
  pcFactory_ = nullptr;

  pcWorkerThread_->Stop();
  pcWorkerThread_->Quit();
  pcSignalingThread_->Stop();
  pcSignalingThread_->Quit();
  delete pcWorkerThread_;
  delete pcSignalingThread_;
}

// Get the index of the audio device that matches this device id.
// We need this as we set the recording audio device based on it's
// index, rather than it's ID.
//
int32_t MediaDevices::GetAudioDeviceIndex(std::string audioDeviceId) {
  auto index = -1;
  DeviceCollection audioDevices;

  if (!deviceManager_->GetAudioInputDevices(&audioDevices)) {
    return -1;
  }

  for (auto const & device: audioDevices) {
    if (device.id == audioDeviceId) {
      break;
    }

    index++;
  }

  return index;
}

cricket::VideoCapturer* MediaDevices::GetVideoCapturerById(std::string deviceId) {
  DeviceCollection devices;

  if (!GetCaptureDevices(VIDEO, &devices)) {
    ERROR("Could not enumerate video devices!");
    // @todo probably handle this better
    return nullptr;
  }

  auto it = std::find_if(devices.begin(), devices.end(), [deviceId] (const auto& device) {
    return device.id == deviceId;
  });

  if (it == devices.end()) {
    ERROR("Could not find device with the id of %s", deviceId.c_str());
    return nullptr;
  }

  return deviceManager_->CreateVideoCapturer(*it);
}

bool MediaDevices::GetCaptureDevices(DeviceType type, DeviceCollection* devices) {
  switch (type) {
    case VIDEO:
      if (!deviceManager_->GetVideoCaptureDevices(devices)) {
        ERROR("Could not get media capturers for audio devices");
        return false;
      }
      break;

    case AUDIO:
      if (!deviceManager_->GetAudioInputDevices(devices)) {
        ERROR("Could not get media capturers for video devices");
        return false;
      }
      break;

    default:
      ERROR("Could not get media capturers for unknown device type: %d", static_cast<int>(type));
      return false;
  }

  return true;
}

// @todo implement an object that uses the correct MediaConstraints interface
//
rtc::scoped_refptr<webrtc::MediaStreamInterface> MediaDevices::GetMedia(MediaConstraints constraints) {
  INFO("MediaDevices::GetMedia");

  auto stream = pcFactory_->CreateLocalMediaStream(NewGuidStr());
  auto audioDevice = constraints.find("audio");
  auto videoDevice = constraints.find("video");

  if (audioDevice != constraints.end()) {
    auto audioDeviceId = audioDevice->second;
    auto micIndex = GetAudioDeviceIndex(audioDeviceId);

    INFO("Creating Audio track with %s device (index %d)",
                                  audioDeviceId.c_str(), micIndex);

    if (audioDevice_->SetRecordingDevice(micIndex) < 0) {
      ERROR("Could not select the recording device!");
      return nullptr;
    }

    audioDevice_->InitRecording();
    audioDevice_->StartRecording();

    webrtc::FakeConstraints audioConstraints;

    stream->AddTrack(pcFactory_->CreateAudioTrack(NewGuidStr(),
                          pcFactory_->CreateAudioSource(&audioConstraints)));
  }

  if (videoDevice != constraints.end()) {
    auto videoDeviceId = videoDevice->second;
    INFO("Creating Video track with %s device", videoDeviceId.c_str());

    auto capturer = GetVideoCapturerById(videoDeviceId);
    if (capturer == nullptr) {
      // @todo handle error
      return nullptr;
    }

    webrtc::FakeConstraints videoConstraints;

    rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack(
      pcFactory_->CreateVideoTrack(NewGuidStr(),
        pcFactory_->CreateVideoSource(capturer, &videoConstraints)));

    stream->AddTrack(videoTrack);
  }

  return stream;
}

