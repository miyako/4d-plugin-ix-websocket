# 4d-plugin-ix-websocket
Simple websocket based on [IXWebSocket](https://github.com/machinezone/IXWebSocket)

### Platform

| carbon | cocoa | win32 | win64 |
|:------:|:-----:|:---------:|:---------:|
||<img src="https://cloud.githubusercontent.com/assets/1725068/22371562/1b091f0a-e4db-11e6-8458-8653954a7cce.png" width="24" height="24" /> | |

### Version

<img src="https://user-images.githubusercontent.com/1725068/41266195-ddf767b2-6e30-11e8-9d6b-2adf6a9f57a5.png" width="32" height="32" />

### Syntax

[miyako.github.io](https://miyako.github.io/2019/07/11/4d-plugin-ix-websocket.html)

### Discussion

``PA_NewProcess`` is now thread safe. However, it seems like a process launched using this API has some significant limitations.

* ``PA_ObjectRef`` and ``PA_CollectionRef`` are not allowed (always ``0x0``)
* ``PA_ExecuteCommandByID`` is not allowed

**Note**: In previous versions, it was possible to invoke a shared component method with ``PA_ExecuteCommandByID`` and ``EXECUTE METHOD:C1007``. This is no longer possible. Now, only a method in the host database can be invoked (``PA_ExecuteMethodByID`` is allowed).
