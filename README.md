![version](https://img.shields.io/badge/version-17%2B-3E8B93)
![platform](https://img.shields.io/static/v1?label=platform&message=mac-intel%20|%20mac-arm%20|%20win-64&color=blue)
[![license](https://img.shields.io/github/license/miyako/4d-plugin-ix-websocket)](LICENSE)
![downloads](https://img.shields.io/github/downloads/miyako/4d-plugin-ix-websocket/total)

# 4d-plugin-ix-websocket
Simple websocket based on [IXWebSocket-5.1.6](https://github.com/machinezone/IXWebSocket)

### Syntax

[miyako.github.io](https://miyako.github.io/2019/07/11/4d-plugin-ix-websocket.html)

### Discussion

``PA_NewProcess`` is now thread safe. However, it seems like a process launched using this API has some significant limitations.

* ``PA_ObjectRef`` and ``PA_CollectionRef`` are not allowed (always ``0x0``)
* ``PA_ExecuteCommandByID`` is not allowed

**Note**: In previous versions, it was possible to invoke a shared component method with ``PA_ExecuteCommandByID`` and ``EXECUTE METHOD:C1007``. This is no longer possible. Now, only a method in the host database can be invoked (``PA_ExecuteMethodByID`` is allowed).

Normally, the array of ``PA_Variable`` passed to ``PA_ExecuteCommandByID`` does not have to be instantiated. 

You can call 

1. ``PA_CreateUnistring``
1. ``PA_SetStringVariable`` 
1. ``PA_ExecuteCommandByID``
1. ``PA_DisposeUnistring``

and all will be fine.

However, for paramters passed to ``PA_ExecuteMethodByID`` the variables need to be managed on the plugin side.

i.e.

1. ``PA_CreateVariable(eVK_Unistring)``
1. ``PA_CreateUnistring``
1. ``PA_SetStringVariable`` 
1. ``PA_ExecuteMethodByID``
1. ``PA_ClearVariable``

Notice there is not need to call ``PA_DisposeUnistring``. It is implied in ``PA_ClearVariable``.

#### PA_NewProcess dilenma

Normally, cleanup code specific to a plugin can be executed in ``kDeinitPlugin``. However, this event is **too late** to signal a process launched with ``PA_NewProcess``. One must signal the process during the ``On Exit`` database event phase, namely during the ``kCloseProcess`` of the process named ``$xx`` at the latest.

It seems there is a difference between tracing a process with ``TRACE`` versus break points. 

``TRACE``: If the method is aborted in the debugger, subsequent calls to ``PA_ExecuteMethodByID`` are ignored with no errors. The application can be closed even while the debugger is displayed. 

Break point: If the debugger is displayed, the application can't be closed (hang). On the other hand, aborting a method does not prevent subsequent calls to ``PA_ExecuteMethodByID``.

#### poll() replacement

On windows, a ``poll()`` shim is required to build ``ixwebsocket.lib``.

Tested the following versions:

* [libressl-portable/portable](https://github.com/libressl-portable/portable/blob/master/apps/openssl/compat/poll_win.c) cosumes too much CPU (50% for each object)

* [WSAPoll](https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsapoll) server works, client not

reading: [daniel.haxx.se](https://daniel.haxx.se/blog/2012/10/10/wsapoll-is-broken/)

* [mpv-player/mpv/](https://github.com/mpv-player/mpv/blob/master/osdep/polldev.c) this one did the trick!
