Penumbra Overture Rebuild
=======================
This is fork of the original Penumbra Overture and HPL1 Engine code with some modifications to make the game
playable on modern Linux distributions. The changes include:
- Updating the OpenGL extension loading mechanism to use glew insteand of the ancient Glee header
- Updating the anglescript scripting library to version 2.8.1
- Some minor fixes to make the project build on a modern version of gcc

I've also included a binary that should work. Just replace the penumbra.bin in your Steam installation with the new one.

Troubleshooting:
=======================
- Black Screen: Try disabling anti-aliasing in the settings
- Audio popping: Try increasing the "StreamUpdateFreq" value in .frictionalgames/Penumbra/Overture/settings.cfg (e.g. 100 seems to work for me)

License Information
-------------------
All code is under the GPL Version 3 license. Read the COPYING file for terms of use of the license.
