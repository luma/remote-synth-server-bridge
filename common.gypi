{
  'variables': {
    'project_root%': '<!(node -p -e "require(\'fs\').realpathSync(\'./\')")',
    'extdir%': '<(project_root%)/ext',
    #'libwebrtc%': '<(project_root%)/ext/webrtc/osx/x64/include',
    #'libwebrtc_out%': '<(project_root%)/ext/webrtc/osx/x64/lib',
    'libwebrtc%': '<(project_root%)/ext/webrtc_src',
    'libwebrtc_out%': '<(project_root%)/ext/webrtc_src/out_osx/$(BUILDTYPE)/',
  },
  "target_defaults": {
    "default_configuration": "Debug",
    "configurations": {
      "Debug": {
        "defines": [
          "DEBUG",
          "_DEBUG",
        ],
        "xcode_settings": {
          "GCC_OPTIMIZATION_LEVEL": "0",
          "GCC_GENERATE_DEBUGGING_SYMBOLS": "YES"
        }
      },
      "Release": {
        "xcode_settings": {
          "GCC_OPTIMIZATION_LEVEL": "3",
          "GCC_GENERATE_DEBUGGING_SYMBOLS": "NO",
          "DEAD_CODE_STRIPPING": "YES",
          "GCC_INLINES_ARE_PRIVATE_EXTERN": "YES"
        }
      }
    }
  }
}