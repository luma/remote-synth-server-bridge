{
  'includes': [
      'common.gypi'
  ],
  'targets': [
    {
      'target_name': 'SynthBridge',
      'include_dirs': [
        'src',
        '<(libwebrtc)',
#        '<(libwebrtc)/webrtc',
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
      'xcode_settings': {
        'OTHER_CPLUSPLUSFLAGS' : ['-std=c++14','-stdlib=libc++'],
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
        'library_dirs': [
          '<(libwebrtc_out)/'
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
              '-lfattycakes',
              '-framework AppKit',
              '-framework QTKit',
            ]
          }],
        ],
        'libraries': []
      },
      'sources': [
        'src/Negotiator.cpp',
        'src/Peer.cpp',
#        'src/PeerWrapper.cpp',
        'src/SynthBridge.cpp'
      ],
    }
  ]
}