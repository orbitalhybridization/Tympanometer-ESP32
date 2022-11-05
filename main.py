from machine import Pin, ADC, PWM, DAC
import time
import math

#black wire: ground
# digital input GPIO39 (A23)
switch = Pin(14, Pin.IN)
# LED1 sealed GPIO33 (A19) white wire
led1=Pin(33,Pin.OUT)
# analog input microcontroller GPIO12 (A24)
time_input = ADC(Pin(12))
# analog outpuct microcontroller GPIO26 (A21)
out = DAC(Pin(26))
# user feedback LED red wire
led2 = PWM(Pin(22),freq=5000)

while True:
    on_off = switch.value()
    print(on_off)
    # switch value set to 1 for testing
    # on_off = 1
    led1.value(on_off)
    
    if on_off == 1:
        # read time duration as an input from main microcontroller
        # convert to voltage (0-3.3V)
        td = time_input.read()*3.3/4096
        if td >=0 and td <= 1:
            time_duration = 1
        elif td >1 and td <= 2:
            time_duration = 2
        elif td >2 and td <= 3.3:
            time_duration = 3
        # time_duration = 3
        
        time_duration = round(time_duration, 3)*1000
        start_time = time.ticks_add(time.ticks_ms(), 0)
        stop_time = time.ticks_add(time.ticks_ms(), int(time_duration))
        print('td', time_duration)
        
        while time.ticks_ms() <= stop_time:
            t = time.ticks_diff(time.ticks_ms(), start_time)
            print('time',t)
            # generate sound signal base on time
            sig1 = math.sin(2*math.pi*226*t/1000)+1
            variance = 1
            gau = 1/(math.sqrt(2*math.pi*variance))*(math.exp(-((t-time_duration/2)/1000)**2/(2*variance)))
            # print('com',gau*sig1)
            out_value = (sig1*gau)*256
            print('output is ', out_value)
            out.write(round(out_value))
            led2.duty(round(out_value))
            