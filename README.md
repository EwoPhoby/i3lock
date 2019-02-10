i3lock
======

This is a fork of [lixxia/i3lock](https://github.com/lixia/i3lock), adding the following features:

- Configuration file, reducing the need for overly long i3lock command lines.
- Keybindings to run custom commands even when the screen is locked, e.g. for music playback control.
- Customizable (see `man 3 strftime`) format for the clock in the unlock indicator.
  - Second line (the one that shows hints like `Caps Lock` when mistyping the password) usable for e.g. date display.
- Customizable alpha channel of the unlock indicator background.
- Redraw the screen on `SIGUSR1`, useful to immediately update the clock after resuming from suspend.

## Example config file

~/.config/i3lock.conf:
```
color = #101010
verify-color = #00dd00
wrong-color = #dd0000
idle-color = #aaaaaa
circle-alpha = 0
# Supported values: default, win, none
pointer = win
time-format = %H:%M
date-format = %d.%m.%Y
unlock-indicator = true
ignore-empty-password = true
image=~/.wallpaper.png

# The commands on the right will be run in a forked shell when the key
command = XF86AudioPlay = playerctl play-pause
command = XF86AudioPrev = playerctl previous
command = XF86AudioNext = playerctl next
# You can basically execute everything. Just make sure it doesn't kill i3lock :)
command = super+alt+r = ~/.local/bin/remove-current-track.sh
```

## Screenshots

![Idle state](/images/forest.png?raw=true "")

# Lixxia/i3lock README

This is my own copy of i3lock, consisting of the following tweaks: 
- Display changes on key-strokes and escape/backspace.
- Added 12-hour clock to the unlock indicator and periodic updater so time stays relevant. 
- The unlock indicator will always be displayed, regardless of state. (Originally it was only shown after initial keypress) 
- Command line arguments to customize colors. Each (optional) argument will accept a color in hexadecimal format. 
  - `-o color` Specifies verification color
  - `-w color` Specifies wrong password/backspace color
  - `-l color` Specifies default/idle color
  - The given colors will be used as-is for the lines and text for their respective states. The colors will automatically be lightened slightly and used with lower opacity (20%) for the circle fill. 
  - If no colors are specified it defaults to green/red/black for verify/wrong/idle respectively.

## Example Usage
`i3lock -i ~/.i3/background.png -c '#000000' -o '#191d0f' -w '#572020' -l '#ffffff' -e`

## Screenshots

#### No configuration specified
![Default](/images/defaultlock.png?raw=true "")
#### Error Color
![Error](/images/defaulterror.png?raw=true "")

### Example Configuration

#### Idle
![Idle state](/images/lockscreen.png?raw=true "")
#### Key Press
![On key press](/images/lockscreenkeypress.png?raw=true "")
#### Escape/Backspace
![On escape or backspace](/images/lockscreenesc.png?raw=true "")

Background in above screenshots can be found in images/background.jpg

<p>

## Install

### Dependencies

Make sure you have the following libraries installed in addition to the packages in the requirements section below.

`libxkbcommon-dev libxkbcommon-x11-dev libpam0g-devl`

### Build

Run the following commands: 
```
git clone https://github.com/Lixxia/i3lock.git
cd i3lock
autoreconf -fi
mkdir -p build && cd build
../configure
make && sudo make install
```

---

### Original README

i3lock - improved screen locker
===============================
i3lock is a simple screen locker like slock. After starting it, you will
see a white screen (you can configure the color/an image). You can return
to your screen by entering your password.

Many little improvements have been made to i3lock over time:

- i3lock forks, so you can combine it with an alias to suspend to RAM
  (run "i3lock && echo mem > /sys/power/state" to get a locked screen
   after waking up your computer from suspend to RAM)

- You can specify either a background color or a PNG image which will be
  displayed while your screen is locked.

- You can specify whether i3lock should bell upon a wrong password.

- i3lock uses PAM and therefore is compatible with LDAP etc.
  On OpenBSD i3lock uses the bsd_auth(3) framework.

Requirements
------------
- pkg-config
- libxcb
- libxcb-util
- libpam-dev
- libcairo-dev
- libxcb-xinerama
- libxcb-randr
- libev
- libx11-dev
- libx11-xcb-dev
- libxkbcommon >= 0.5.0
- libxkbcommon-x11 >= 0.5.0

Running i3lock
-------------
Simply invoke the 'i3lock' command. To get out of it, enter your password and
press enter.

On OpenBSD the `i3lock` binary needs to be setgid `auth` to call the
authentication helpers, e.g. `/usr/libexec/auth/login_passwd`.

Upstream
--------
Please submit pull requests to https://github.com/i3/i3lock
