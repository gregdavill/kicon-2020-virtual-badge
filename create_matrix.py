# This file is Copyright (c) 2020 Gregory Davill <greg.davill@gmail.com>
# License: MIT

# Simple script that makes use of the python interface in KiCad to position and route a LED matrix.

import sys
from pcbnew import *
pcb = GetBoard()

from math import sin, acos, cos, asin, pi
from random import randint

# Determine the PCB width at y-offset (measured in mm from center)
def width(y):
    # The PCB edges are formed using a circular tool, with radius around 148mm
    theta = asin(y/148.0)
    c = cos(theta) * 148.0

    # The circular cutout tool is 177mm away from the PCB center line, calculate half_width of the PCB.
    d = (177.0 - c)

    # We want to fit 32 LEDs across, which is 31 equally-spaced gaps. Also add 1 gap to each side for a margin. (/33)
    spacing = (d*2.0)/33.0

    # Some debug info
    #print(c, theta,w,s)
    return (d, spacing)

col_pads = [[] for _ in range(32)]

for y in range(16):

    # Start array 26mm up from center line, use fixed y-spacing of 2mm.
    w,s = width(26 - y*2.0)

    # store an array of via positions along each row, this is a crude way to automatically draw tracks between these.
    row = []

    for x in range(32):
        # Pick out our designators, we expect these to be in order, left to right then top to bottom, D1-D513
        Ref = 'D' + str(1 + x+(y*32))
        nPart = pcb.FindModuleByReference(Ref)

        # Set part X/Y + Rotation
        nPart.SetPosition(wxPoint(FromMM(180 - w +(x+1)*s), FromMM(10+y*2)))
        # Yes there is some randomness in the rotation, because I'm evil
        deg = randint(410, 490)/10.0
        nPart.SetOrientationDegrees(deg)

        # Store the pad cathode pad of each LED, this is used to crudely draw tracks later.
        col_pads[x] += [nPart.Pads()[0]]

        # Place a Via
        deg = -deg - 14.0
        v = VIA(pcb)
        pcb.Add(v)
        v.SetWidth(FromMM(0.6))
        v.SetDrill(FramMM(0.3))

        # Some trig here that is basically just polar->cartesian conversion.
        via_point = wxPoint(FromMM(180 - w +(x+1)*s + 1.2*cos(deg * pi/180.0)), FromMM(10+y*2 + 1.2*sin(deg * pi / 180.0)))
        v.SetPosition(via_point)

        # You'll notice that we are not setting any NETs here, when a track overlaps it will be assiged to a NET automatically by KiCad.

        # Draw track from LED Anode to Via
        t = TRACK(pcb)
        pcb.Add(t)
        t.SetWidth(FromMM(0.25))
        t.SetStart(nPart.Pads()[1].GetPosition())
        t.SetEnd(v.GetPosition())

        row += [via_point]
        

    # Draw a track segment between every Via in a row, place these on Layer:In_2
    for r in range(len(row)-1):
        t = TRACK(pcb)
        pcb.Add(t)
        t.SetWidth(FromMM(0.25))
        t.SetStart(row[r])
        t.SetEnd(row[r+1])
        t.SetLayer(In2_Cu)

# After placing all the LEDs and drawing Vias/Tracks we can draw the vertical tracks connecting cols.
for c in col_pads:
    for i in range(len(c)-1):
        
        t = TRACK(pcb)
        pcb.Add(t)
        t.SetWidth(FromMM(0.25))

        # We ofset the track to the left by 0.1mm, this is just to add some clearance.
        start = c[i].GetPosition()
        start.x -= FromMM(0.1)
        t.SetStart(start)
        
        end = c[i+1].GetPosition()
        end.x -= FromMM(0.1)
        t.SetEnd(end)



# KiCad used to let you run scripts with the command below, but that wasn't working for me in the Nightly builds, 
# the scripting console doesn't seem to be operatig in the same working directory as your KiCad project file.
# `exec(open('leds.py').read())`

# So I just copied this script to the clipboard, then used `Ctrl + Shift + V` to `Paste and Run` inside the internal scripting console in KiCad.
# Once the script has run you will likely not see any change, there is a bug/quirk in rendering, basically the GUI doesn't think anything has changed, so it won't redraw the screen.
# I was moving an unrelated via, then saving and restarting pcbnew. This then shows the updated tracks/vias/component placements.