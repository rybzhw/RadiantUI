# RadiantUI

CEF Based Web Browser Integration into Unreal Engine 4

Released under the MIT license.

[![Demo](http://img.youtube.com/vi/g6X76aFz2BA/0.jpg)](https://www.youtube.com/watch?v=g6X76aFz2BA)

https://www.youtube.com/watch?v=g6X76aFz2BA

# Key Features:

* Render HTML/CSS/JavaScript to any surface/material in the game.
* No C++ coding required, everything is done through blueprints and components.
* Automatic Input handling: Walk up to any WebActor and use the mouse and keyboard automatically (Doom3 style interfaces).
* Supports non-flat surfaces: You can map a WebActor onto any mesh and still interact with it (Curved computer screens etc).
* Networked gameplay support: Multiple players can interact with the same WebActor and see the same output.

# About this Fork

This Fork uses **CEF-Branch 2623**. **GPU/WebGL is disabled**.

CEF-Debugfiles are too large for Github (>100MB). You can still download the CEF-Binaries on https://cefbuilds.com/ (Branch 2623).

# When using as a HUD

Press Shift+F1 to get the focus to the HUD-Elements.
If you want to get the Gameinput again, you can do the following.

1. Use the following javascript-code to detect html/body-clicks:
```javascript
$(document).on('click', function(event)
{
	if (event.target.tagName === 'HTML')
	{
		RadiantUI.TriggerEvent('CloseHUD');
	}
});
```
2. Add a CustomEvent-Node and force the focus to the game again - using a Set Input Mode Game Only-Node
