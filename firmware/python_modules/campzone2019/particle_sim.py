import psim
import buttons, defines, system, time
psim.init(32)

direction = 0

def input_B(pressed):
    global direction
    direction = defines.BTN_B
	
buttons.register(defines.BTN_B, input_B)

while direction != defines.BTN_B:
	time.sleep(0.2)
system.reboot()