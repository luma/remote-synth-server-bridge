{
  'includes': [
      'common.gypi'
  ],
  'targets': [
    {
      'target_name': 'crossguid',
      'type': 'none',
      'actions': [
        {
          'action_name': 'run_build',
          'inputs': [],
          'outputs': ['/dev/null'],

          'conditions': [
            ['OS=="linux"', {
              'action': ['cd', '<(extdir)/crossguid/', '&&', './linux.sh'],
            }],
            ['OS=="mac"', {
              'action': ['cd', '<(extdir)/crossguid/', '&&', './mac.sh'],
            }],
          ],
          'message': 'Run build script'
        }
      ],
    },
    {
      'target_name': 'SynthBridge',
      'dependencies': [
        'crossguid'
      ],
      'include_dirs': [
        'src',
        '<(extdir)',
        '<(libwebrtc)',
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
        '-std=c++14',
      ],
      'defines': [
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
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++14','-stdlib=libc++'],
        'OTHER_LDFLAGS': ['-stdlib=libc++'],
        'MACOSX_DEPLOYMENT_TARGET': '10.7',
      },
      'link_settings': {
        'library_dirs': [
          '<(libwebrtc_out)/',
          '<(extdir)/crossguid/',
        ],
        'ldflags': [],
        'conditions': [
          ['OS=="linux"', {
            'libraries': [
              '-lfattycakes',
              #TODO
            ]
          }],
          ['OS=="mac"', {
            'libraries': [
              '<(extdir)/crossguid/guid.o',
              '-lexpat',
              '-lprotobuf_lite',
              '-lyuv',
              '-ljpeg_turbo',
              '-lsrtp',
              '-lvpx',
              '-lvpx_intrinsics_avx2',
              '-lvpx_intrinsics_mmx',
              '-lvpx_intrinsics_sse2',
              '-lvpx_intrinsics_sse4_1',
              '-lvpx_intrinsics_ssse3',
              '-lbitrate_controller',
              '-lsystem_wrappers',
              '-lremote_bitrate_estimator',
              '-lpaced_sender',
              '-lrtp_rtcp',
              '-lcommon_audio',
              '-lcommon_audio_sse2',
              '-lcommon_video',
              '-lG711',
              '-lG722',
              '-liLBC',
              '-liSAC',
              '-liSACFix',
              '-lPCM16B',
              '-lwebrtc_opus',
              '-lopus',
              '-lCNG',
              '-lneteq',
              '-laudio_decoder_interface',
              '-laudio_coding_module',
              '-lvideo_coding_utility',
              '-laudio_encoder_interface',
              '-lvideo_processing',
              '-lvideo_processing_sse2',
              '-laudio_processing',
              '-laudio_processing_sse2',
              '-laudioproc_debug_proto',
              '-laudio_conference_mixer',
              '-laudio_device',
              '-lvoice_engine',
              '-lvideo_engine_core',
              '-lvideo_capture_module',
              '-lvideo_capture_module_internal_impl',
              '-lvideo_render_module',
              '-lvideo_render_module_internal_impl',
              '-ljingle_media',
              '-ljingle_p2p',
              '-ljingle_peerconnection',
              '-lmedia_file',
              '-lwebrtc_utility',
              '-lwebrtc_video_coding',
              '-lwebrtc_i420',
              '-lwebrtc_vp8',
              '-lwebrtc_vp9',
              '-lrtc_p2p',
              '-lrtc_base',
              '-lrtc_base_approved',
              '-lusrsctplib',
              '-lboringssl',
              '-lrtc_xmllite',
              '-lwebrtc',
              '-lwebrtc_common',
#              '-lfield_trial',
#              '-lfield_trial_default',
#              '-lfattycakes',
              '-framework AppKit',
              '-framework QTKit',
            ]
          }],
        ],
        'libraries': []
      },
      'sources': [
        'src/common/V8.cpp',
        'src/common/EventLoop.cpp',
        'src/devices/MediaDevices.cpp',
        'src/devices/MediaDeviceInfo.cpp',
        'src/negotiation/IceCandidate.cpp',
        'src/negotiation/Negotiator.cpp',
        'src/peer/DataChannel.cpp',
        'src/peer/Peer.cpp',
        'src/SynthBridge.cpp',
      ],
    }
  ]
}