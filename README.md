# Media Foundation Basic Playback application with the custom EVR presenter

A Windows classic sample, EVRPresenter (https://learn.microsoft.com/en-us/windows/win32/medfound/evrpresenter-sample), 
recommends testing this DLL with the MFPlayer Sample. Least to say, this recommendation 
is outdated. The project MFEVRPlayback of this repository demonstrates the use of a custom 
EVR presenter with a media foundation playback application.

The player is based off the basic playback class CPlayer with the Media Session Playback 
Example (https://learn.microsoft.com/en-us/windows/win32/medfound/media-session-playback-example). 
The class is made to work with an Activate object, which is not implemented in the EVRPresenter 
example. To minimize the changes in EVRPresenter, I transferred the activation routine to the 
caller site, into the function CreateMediaSinkActivate of the player.cpp file, with a necessary 
signature change: 
_CreateMediaSinkActivate(IMFStreamDescriptor* pSourceSD, HWND hVideoWindow, IMFActivate** ppActivate)_ 
becomes 
_CreateMediaSink(IMFStreamDescriptor* pSourceSD, HWND hVideoWindow, IMFMediaSink** ppMediaSink)_. 
Respective changes are made in functions AddOutputNode and AddBranchToPartialTopology.

I found a bug in EVRPresenter, file Presenter.cpp, the method 
_EVRCustomPresenter::CreateOptimalVideoType(...)_. To fix this bug I added the call 
_mtOptimal.CreateEmptyType();_ after line 1411 (_VideoType mtOptimal;_). One can easily see what 
it is done for by compiling the project with my added line commented.

Also, I made EVRPrenter.dll registration-free by adding and exporting the function 
_CreateEVRCustomPresenter_ (dllmain.cpp).