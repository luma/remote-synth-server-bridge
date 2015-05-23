# remote-synth-server-bridge
A native bridge based on WebRTC for remote sync server


### Prerequisites

You'll need XCode 3.0 or higher. You'll also need to install the [Depot Tools](http://dev.chromium.org/developers/how-tos/install-depot-tools).

### Managing WebRTC

**Note:** When you run `npm install` WebRTC will be retrieved and built. This will take some time when it's first run. Go get yourself a coffee. You've earned it!

#### Get/Update Webrtc

```
./webrtc fetch
```

Or to fetch a specific branch
```
./webrtc fetch fooBranch
```

#### Build Webrtc

Debug mode
```
./webrtc build
```

Release mode
```
./webrtc build Release
```

### Build the Node module

```
npm build
```

