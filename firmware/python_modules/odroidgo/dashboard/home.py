import time, display, random, sndmixer
dx = 1
dy = 1
x = 0
y = 0

fg = 0xFFFFFF
bg = 0xFFFFFF

sndmixer.start()
a = open("/f.s3m", 'rb')
b = a.read()
a.close()
sndmixer.mod(b)

display.font("fairlight12")
display.fill(bg)

while 1:
  x += dx
  chg = False
  if dx == 1 and x >= 320-150:
    dx = -1
    chg = True
  elif dx == -1 and x <= 0:
    dx = 1
    chg = True
  y += dy
  if dy == 1 and y >= 240-20:
    dy = -1
    chg = True
  elif dy == -1 and y <= 0:
    dy = 1
    chg = True
  if chg:
    fg = random.randint(0x000000,0xFFFF00)
    display.fill(bg)
  display.textColor(fg)
  display.cursor(x,y)
  display.print("BADGE.TEAM")
  display.flush()
  display.textColor(0xFFFFFF)
  display.cursor(x,y)
  display.print("BADGE.TEAM")
