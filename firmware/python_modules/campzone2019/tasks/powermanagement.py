# File: powermanagement.py
# Version: 2
# Description: Power management task, puts the badge to sleep when idle
# License: MIT
# Authors: Renze Nicolai <renze@rnplus.nl>

import virtualtimers, deepsleep, machine, sys

requestedStandbyTime = 0
onSleepCallback = None

userResponseTime = machine.nvs_getint('splash', 'urt') or 6000

disabled = False
enabled = False

def disable():
    global disabled, enabled
    disabled = True
    enabled = False
    kill()

def enable():
    global disabled, enabled
    disabled = False
    enabled = True
    feed()

def resume():
    global disabled, enabled
    if disabled and enabled:
        enable()

def state():
    if disabled:
        return -1
    return userResponseTime

def usb_attached():
    pass
    # return badge.usb_volt_sense() > 4500

def pm_task():
    ''' The power management task [internal function] '''
    global requestedStandbyTime
    global userResponseTime
    global disabled

    if disabled:
        return userResponseTime

    idleTime = virtualtimers.idle_time()
    if not ((usb_attached() and machine.nvs_getint('badge', 'usb_stay_awake') or 0) != 0 ):
        global onSleepCallback
        if not onSleepCallback==None:
            print("[Power management] Running onSleepCallback...")
            try:
                onSleepCallback(0)
            except BaseException as e:
                print("[ERROR] An error occured in the on sleep callback.")
                sys.print_exception(e)
        deepsleep.start_sleeping(0)

    return userResponseTime

def feed():
    ''' Start / resets the power management task '''
    global userResponseTime, enabled
    if not virtualtimers.update(userResponseTime, pm_task):
        enabled = True
        virtualtimers.new(userResponseTime, pm_task, True)

def kill():
    ''' Kills the power management task '''
    virtualtimers.delete(pm_task)
    global disabled, enabled
    disabled = True
    enabled = False

def callback(cb):
    ''' Set a callback which is run before sleeping '''
    global onSleepCallback
    onSleepCallback = cb

def set_timeout(t):
    ''' Set timeout '''
    global userResponseTime
    userResponseTime = t
