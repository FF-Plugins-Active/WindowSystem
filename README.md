# WindowSystem

PURPOSE:
- This plugin allows to create a external windows in "runtime" with UMG support.

USAGE:
- Place "BP_Template_WinMan" to world (it is in Plugins/WindowSystem Content/BPs)
- Use "Get All Actors of Class -> Get Index 0 -> Create a variable for that (BP_Template_WinMan Object Referance)
 - We need this in only Event Begin Play and there will be only one actor with that class. So, Get All Actors of Class won't impact your project's performance.
 - We will use created variable as referance.
- Add "Create New Window" node from BP_Template_WinMan Object Referance.
- To enable file drag drop for main window, set true to "Allow Main Window" boolean on BP_Template_WinMan (Default is true and it exposed as instanced variable.)
- To enable file drag drop for created windows, use Set File Drag Drop Support after window creation.
- Drop operation will trigger an event as OnFileDrop. This event will give you file path, drop location on screen and dropped window's name. So, you need to implement your import mechanics with these informations.

FEATURES:
- Created windows and main game window can support runtime file drag drop.
- User can drop multiple files at once. It gives an array.
- User can get drop location on screen. You can use it to calculate world location for importing and spawning 3D assets to specific location.
- It records all generated windows in a "MAP_Windows" map variable. (Key = Window Tag / Value = Window Object). If you have access BP_Template_WinMan Object Referance, you can get your window object referance with only its tag.
- User can take screenshot of visible window or whole widget.
- If target platform is Windows 11, created windows will have rounded corners.

WINDOW VARIABLES:
- Is Top Most
- Has Close (We recommend to set this false. Because if user close a runtime generated window, all contents and its referances will be gone. So, use it for if window's widget is manually created and does not important for mechanics.
- Force Volatile
- Preserve Aspect Ratio (When user change size of that window, should window preserve it's aspect ratio or not)
- Minimized (Should window initially minimized or not)
- Supports Maximize
- Supports Minimize
- Set Mirror Window (True if the window is a mirror window for HMD content)
- In Window Tag (You should absolutely set this with a meaningful and unique tag without space. Because we use this to record and get windows)
- In Window Title (Title will be visible on window's titlebar if you give a title. This is different than tag. It can be a sentence and more readable.)
- In Tooltip (When user hovered that window, there will be additional information as tooltip.)
- Window Size
- Minimum Size (Minimum acceptable size for that window.)
- Window Position
- In Border (Border size between frame and its UMG content)
- In Opacity (Initial opacity value of that window)

ADDITIONAL FUNCTIONS (Static Functions. Should be called without Window Manager):
- Take Screenshot of Widget
- Take Screenshot of Window

- Get Desktop Resolution
- Get Main Window Title

- Get Window Tag
- Get Window Title
- Get Window Position (return screen position as FVector2D)
- Get Window State (returns enums Minimized / Restored / Maximized)
- Get Window Widget (return UUSerWidget)

- Set Window Title
- Set Window State (Minimmized / Restored / Maximized)
- Set Window Shape (Give new position and or size with an animation)
- Set Window Opacity

- Is Window Top Most
- Is Window Hovered
- Bring Window Front

ADDITIONAL FUNCTIONS (Virtual Function. Should be called with Window Manager Referance):
- Set All Windows Opacities
- Detect Hovered Window
- Set File Drag Drop Support
- Create New Window
- Close Window
- Close All Windows

ON PURPOSED LIMITATION:
- Created windows won't be appeared on taskbar. Because even if we disable "close" button in titlebar, Windows Operating System does not disable "close" button in task bar and user can accidentally close it. So, hide it from taskbar. If a creted window goes back, you need to use an UMG button to and "Bring Window Front" function.

PLATFORM:
- This platform is created only for Windows operating system and Unreal Engine 5. Other platforms such as Linux, Mac OS and Unreal Engine 4 won't be supported.
