This package provides the precompiled library for NetMQ the ZeroMQ port for .Net, with examples.

NetMQ is an open source project licensed under GPL v3, you can obtain original copies of the source code at: https://github.com/zeromq/netmq/
Open the NetMQ3.5.csproj file, build. Compiling may be difficult under certian platforms and configurations, I am not offering support for this.

All credit for getting this working with Unity goes out to the open source community.

Reference: https://github.com/zeromq/netmq/issues/98

Do not terminate the context, Unity will hang. Closing sockets causes high CPU when using the editor. Unbind and Disconnect should be fine.