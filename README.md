# PURPOSES AND FEATURES:
- Runtime external window creation with UMG support.
- Runtime **Save and Select File Dialogs**
- File drag drop supports for main game window and external windows. It returns, content paths and content locations.
- If operating system is **Windows 11**, created windows will have **rounded corners**.
- Hover, window move and close detection events.

# USAGE:
- Place **BP_Template_WinMan** to world. This is your window manager.
	- It is in **Plugins/WindowSystem/Content/BPs**
	- You can create a new one based on your needs.
	- It has to be only **one manager actor** in scene.

- Create and construct your widget.

- **Spawn Actor From Class** (use **EachWindow** class)

- Connect your manager and widget to **Spawn Actor From Class**

- Set your settings
	- You have to give a unique tag **(optionally meaningful)** to your window.

- All window control functions are virtual functions of spawned EachWindow class object.

- You can access all created windows with **Manager->MAP_Windows (FName, AEachWindow)**. If you know the tag of window which you want to control, you can access it from this map.

- If you want to enable file drag drop feature for created window, you have two options.
	- You can enable it when set enable **bIsFileDropEnabled** boolean on **Spawn Actor from Class**
	- You can enable it in the future with **Set File Drag Drop Support** function.
	
- If you want to enable file drag drop feature for main window, just enable **bAllowMainWindow** boolean anytime.

# DISCLAIMER
- If you close your window accidentally, you will lost your widget as well. This can trigger errors and even crashes with other mechanics which depends on these widgets.
- If you use **HideFromTaskBar** features and accidentally throw your window to the backward, you will have to bring it back with **Bring Window Front**

# ADDITIONAL CONTROL FUNCTIONS FOR WINDOWS
- Take Screenshot of Window

- **(Variable)** WindowTag
- **(UMG Widget Variable)** ContentWidget

- Set Window Opacity
- Set Window State **(Minimmized / Restored / Maximized)
- Set Window Shape **(Give new position and or size with an animation)**
- Set Window Position
- Set Window Title

- Get Window State **(returns enums Minimized / Restored / Maximized)**
- Get Window Position **(return screen position as FVector2D)**
- Get Window Title

- Is Window Top Most
- Bring Window Front

- Toggle Top Most Option
- Toggle Show On Task Bar
- Toggle Opacity

- Set File Drag Drop Support

- **(Event)** On Window Hovered
- **(Event)** On Window Moved
- **(Event)** On Window Closed

# ADDITINAL FUNCTIONS FOR GENERAL USAGE
- Get Main Window Title	
- Set Main Window Title
- Select File Dialog
- Save File Dialog

# WINDOW VARIABLES:
- Is Top Most

- Has Close
	- We recommend to set this false. Because if user close a runtime generated window, all contents and its referances will be gone. So, use it with cautious.

- Force Volatile

- Preserve Aspect Ratio
	- When user change size of that window, should window preserve it's aspect ratio or not.

- Minimized
	- Create window as minimized or not.

- Supports Maximize

- Supports Minimize

- Set Mirror Window

- In Window Tag (You should absolutely set this with a meaningful and unique tag without space. Because we use this to record and get windows)

- In Window Title
	- If you give a title, it will be visible on window's title bar.
	- It is different than WindowTag as variable aspect but you can use same value.
	- You can use more readable values with it.

- In Tooltip
	- When user hovered that window, there will be additional information as tooltip.

- Window Size

- Minimum Size
	- Minimum acceptable size for that window.

- Window Position

- Border Thick
	- Thickness between frame and UMG content.

- In Opacity
	- Initial opacity value of that window.

# PLATFORM:
- This plugin created only for **Windows** operating system and **Unreal Engine 5.**