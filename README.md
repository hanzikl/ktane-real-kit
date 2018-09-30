## Real life Kit for game Keep Talking and Nobody Explodes

Keep Talking and Nobody Explodes (KTaNE) game in physical realm using Arduino.

### Features

 - completely inspired by original game
 - almost all features of original modules preserved
 - complete physical modularity
 - realistic look
 - beeper included

### Planned features

 - special new modules with advantages of physical world
 - bluetooth configuration upload
 - API for external devices connected via serial/bluetooth (sounds & lights)
 - automatic configuration generation (see [ktane-controller](https://github.com/hanzikl/ktane-controller) project)

### Modules implemented
 - Display (output only)
 - Keypad
 - Simon Says
 - Maze
 - Wires
 - Button

### Repository content

 - `arduino` folder with a arduino programs.
 - `fritzing` folder contains DPS for modules IO board.
 - `models` folder contains some early models for 3d printing of modules (in special branch called `models` :-) )

### Technical details
 - Arduino is connected to modules, which are made of stripboard or manufactured DPS (see `fritzing` folder),
  two cheap chips and some input/output devices (buttons, LEDs, displays).

### Modules waiting to be implemented:
 - Morse (comming soon)
 - Complicated wires
 - Memory
 - Needy capacitor
 - Needy venting

Modules too difficult to create now:
 - Who's on first
 - Wire sequence
 - Needy knob
 - Passwords

If you have any question, ask me on [reddit](https://www.reddit.com/r/ktane/comments/6cxbbc/ktane_real_kit_work_in_progress_wanna_see_more/) or just mail me (look for adress in my commits).
