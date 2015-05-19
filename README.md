# remote-synth-server-bridge
A native bridge based on WebRTC for remote sync server


### Prerequisites

You'll need XCode 3.0 or higher. You'll also need to install the [Depot Tools](http://dev.chromium.org/developers/how-tos/install-depot-tools).

### Managing WebRTC

**Note:** When you run `npm install` WebRTC will be retrieved and built. This will take some time when it's first run. Go get yourself a coffee. You've earned it!

#### Get/Update Webrtc

```
npm run pullWebrtc
```

#### Build Webrtc

Debug mode
```
npm run buildWebrtcDebug
```

Release mode
```
npm run buildWebrtcRelease
```

### Build the Node module

```
npm build
```

