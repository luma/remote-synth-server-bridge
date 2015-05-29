# remote-synth-server-bridge
A native bridge based on WebRTC for remote sync server


### Prerequisites

You'll need XCode 3.0 or higher. Install the lumbergh-cli tool:
```
npm install -g opentok/lumbergh-cli
```

### Managing WebRTC

**Note:** When you run `npm install` WebRTC will be retrieved and built. This will take some time when it's first run. Go get yourself a coffee. You've earned it!

#### Get/Update Webrtc

```
lumbergh install
```

### Build the Node module

```
npm build
```


## TIPs

### Forcing node-gyp to generate an XCode project

The xcodeproj will be created in build/ when you run the following:
```
node-gyp configure -- -f xcode
xcodebuild -project ./build/binding.xcodeproj
```
