
{
  'target_defaults': {
    'default_configuration': 'Debug',
  },
  'variables': {
    'libwebrtc%': './ext/webrtc/src',
    'libwebrtc_out%': '<(libwebrtc%)/out/$(BUILDTYPE)',
  },
  "targets": [
    {
      "target_name": "SynthBridge",
      "options": {
        "debug": true,
      },
      "include_dirs": [
        'src/include',
        '<(libwebrtc)',
        '<(libwebrtc)/webrtc',
        '<(libwebrtc)/third_party/webrtc',
        '<(libwebrtc)/third_party/webrtc/system_wrappers/interface',
        '<(libwebrtc)/third_party',
      ],
      'cflags': [
        '-pthread',
        '-fno-exceptions',
        '-fno-strict-aliasing',
        '-Wall',
        '-Wno-unused-parameter',
        '-Wno-missing-field-initializers',
        '-Wextra',
        '-Wno-unused-local-typedefs',
        '-Wno-uninitialized',
        '-Wno-unused-variable',
        '-Wno-unused-but-set-variable',
        '-pipe',
        '-fno-ident',
        '-fdata-sections',
        '-ffunction-sections',
        '-fPIC',
        '-fpermissive',
        "-std=c++11"
      ],
      'xcode_settings': {
        'OTHER_CFLAGS': [
          '-std=gnu++0x',
          '-stdlib=libc++',
          '-Wno-c++0x-extensions',
          '-Wno-c++11-extensions',
        ],
        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
        'OTHER_LDFLAGS': ['-stdlib=libc++'],
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'defines': [
#        'TRACING',
        'LARGEFILE_SOURCE',
        '_FILE_OFFSET_BITS=64',
        'WEBRTC_TARGET_PC',
        'WEBRTC_LINUX',
        'WEBRTC_THREAD_RR',
        'EXPAT_RELATIVE_PATH',
        'GTEST_RELATIVE_PATH',
        'JSONCPP_RELATIVE_PATH',
        'WEBRTC_RELATIVE_PATH',
        'POSIX',
        '__STDC_FORMAT_MACROS',
        'DYNAMIC_ANNOTATIONS_ENABLED=0',
        'WEBRTC_POSIX=1'
      ],
      'link_settings': {
        'ldflags': [
        ],
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '<(libwebrtc_out)/libjingle_peerconnection.a',
              '<(libwebrtc_out)/libjingle_p2p.a',
              '<(libwebrtc_out)/libjingle_media.a',
              '<(libwebrtc_out)/librtc_p2p.a',
              '<(libwebrtc_out)/librtc_base.a',
              '<(libwebrtc_out)/librtc_base_approved.a',
              '<(libwebrtc_out)/libcrssl.a',
              '<(libwebrtc_out)/libusrsctplib.a',
              '-lnss3',
            ]
          }],
          ['OS=="mac"', {
            'libraries': [
              "../<(libwebrtc_out)/libexpat.a",
              "../<(libwebrtc_out)/libprotobuf_lite.a",
              "../<(libwebrtc_out)/libyuv.a",
              "../<(libwebrtc_out)/libjpeg_turbo.a",
              "../<(libwebrtc_out)/libsrtp.a",
              "../<(libwebrtc_out)/libvpx.a",
              "../<(libwebrtc_out)/libvpx_intrinsics_avx2.a",
              "../<(libwebrtc_out)/libvpx_intrinsics_mmx.a",
              "../<(libwebrtc_out)/libvpx_intrinsics_sse2.a",
              "../<(libwebrtc_out)/libvpx_intrinsics_sse4_1.a",
              "../<(libwebrtc_out)/libvpx_intrinsics_ssse3.a",
              "../<(libwebrtc_out)/libbitrate_controller.a",
              "../<(libwebrtc_out)/libsystem_wrappers.a",
              # "../<(libwebrtc_out)/rbe_components.a",
              "../<(libwebrtc_out)/libremote_bitrate_estimator.a",
              "../<(libwebrtc_out)/libpaced_sender.a",
              "../<(libwebrtc_out)/librtp_rtcp.a",
              "../<(libwebrtc_out)/libcommon_audio.a",
              "../<(libwebrtc_out)/libcommon_audio_sse2.a",
              "../<(libwebrtc_out)/libcommon_video.a",
              "../<(libwebrtc_out)/libG711.a",
              "../<(libwebrtc_out)/libG722.a",
              "../<(libwebrtc_out)/libiLBC.a",
              "../<(libwebrtc_out)/libiSAC.a",
              "../<(libwebrtc_out)/libiSACFix.a",
              "../<(libwebrtc_out)/libPCM16B.a",
              "../<(libwebrtc_out)/libwebrtc_opus.a",
              "../<(libwebrtc_out)/libopus.a",
              "../<(libwebrtc_out)/libCNG.a",
              "../<(libwebrtc_out)/libneteq.a",
              "../<(libwebrtc_out)/libaudio_decoder_interface.a",
              "../<(libwebrtc_out)/libaudio_coding_module.a",
              "../<(libwebrtc_out)/libvideo_coding_utility.a",
              "../<(libwebrtc_out)/libaudio_encoder_interface.a",
              "../<(libwebrtc_out)/libvideo_processing.a",
              "../<(libwebrtc_out)/libvideo_processing_sse2.a",
              "../<(libwebrtc_out)/libaudio_processing.a",
              "../<(libwebrtc_out)/libaudio_processing_sse2.a",
              "../<(libwebrtc_out)/libaudioproc_debug_proto.a",
              "../<(libwebrtc_out)/libaudio_conference_mixer.a",
              "../<(libwebrtc_out)/libaudio_device.a",
              "../<(libwebrtc_out)/libvoice_engine.a",
              "../<(libwebrtc_out)/libvideo_engine_core.a",
              "../<(libwebrtc_out)/libvideo_capture_module.a",
              "../<(libwebrtc_out)/libvideo_capture_module_internal_impl.a",
              "../<(libwebrtc_out)/libvideo_render.a",
              "../<(libwebrtc_out)/libvideo_render_module.a",
              "../<(libwebrtc_out)/libvideo_render_module_internal_impl.a",
              "../<(libwebrtc_out)/libjingle_media.a",
              "../<(libwebrtc_out)/libjingle_p2p.a",
              "../<(libwebrtc_out)/libjingle_peerconnection.a",
              "../<(libwebrtc_out)/libmedia_file.a",
              "../<(libwebrtc_out)/libwebrtc_utility.a",
              "../<(libwebrtc_out)/libwebrtc_video_coding.a",
              "../<(libwebrtc_out)/libwebrtc_i420.a",
              "../<(libwebrtc_out)/libwebrtc_vp8.a",
              "../<(libwebrtc_out)/libwebrtc_vp9.a",
              "../<(libwebrtc_out)/librtc_p2p.a",
              "../<(libwebrtc_out)/librtc_base.a",
              "../<(libwebrtc_out)/librtc_base_approved.a",
              "../<(libwebrtc_out)/libusrsctplib.a",
              '../<(libwebrtc_out)/libboringssl.a',
              "../<(libwebrtc_out)/librtc_xmllite.a",
              "../<(libwebrtc_out)/libwebrtc.a",
              "../<(libwebrtc_out)/libwebrtc_common.a",
              "../<(libwebrtc_out)/libfield_trial.a",
              '-framework AppKit',
              '-framework QTKit',
            ]
          }],
        ],
        'libraries': []
      },
      "sources": [
        "src/Negotiator.cpp",
        "src/Peer.cpp",
        "src/PeerWrapper.cpp",
        "src/SynthBridge.cpp"
      ],
    }
  ]
}