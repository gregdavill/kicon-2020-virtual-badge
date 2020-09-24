import sys
from pcbnew import *
pcb = GetBoard()

from math import sin, acos, cos, asin, pi
from random import randint

def width(y):
    theta = asin(y/148.0)
    c = cos(theta) * 148.0
    d = (177.0 - c)
    spacing = (d*2.0)/33.0
    #print(c, theta,w,s)
    return (d, spacing)

col_pads = [[] for _ in range(32)]

for y in range(16):
    w,s = width(26 - y*2.0)

    row = []

    for x in range(32):
        Ref = 'D' + str(1 + x+(y*32))
    #    print(Ref)
        nPart = pcb.FindModuleByReference(Ref)
        nPart.SetPosition(wxPoint(FromMM(180 - w +(x+1)*s), FromMM(10+y*2)))  # Update XY
        deg = randint(410, 490)/10.0
        nPart.SetOrientationDegrees(deg)

        col_pads[x] += [nPart.Pads()[0]]

        # place_via
        deg = -deg - 14.0
        v = VIA(pcb)
        pcb.Add(v)
        v.SetWidth(600000)
        v.SetDrill(300000)
        via_point = wxPoint(FromMM(180 - w +(x+1)*s + 1.2*cos(deg * pi/180.0)), FromMM(10+y*2 + 1.2*sin(deg * pi / 180.0)))
        v.SetPosition(via_point)

        t = TRACK(pcb)
        pcb.Add(t)
        t.SetWidth(FromMM(0.25))
        t.SetStart(nPart.Pads()[1].GetPosition())
        t.SetEnd(v.GetPosition())

        row += [via_point]
        

    for r in range(len(row)-1):
        
        t = TRACK(pcb)
        pcb.Add(t)
        t.SetWidth(FromMM(0.25))
        t.SetStart(row[r])
        t.SetEnd(row[r+1])
        t.SetLayer(In2_Cu)

for c in col_pads:
    for i in range(len(c)-1):
        
        t = TRACK(pcb)
        pcb.Add(t)
        t.SetWidth(FromMM(0.25))

        start = c[i].GetPosition()
        start.x -= FromMM(0.1)
        t.SetStart(start)
        
        end = c[i+1].GetPosition()
        end.x -= FromMM(0.1)
        t.SetEnd(end)

# KiCad used to let you run scripts with this command, but taht wasn't working for me in Nightly.
# So I just copied this script to the clipboard, then used `Ctrl + Shift + V` to Paste and run in the internal scriptting console in KiCad.
# exec(open('leds.py').read())