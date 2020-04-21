#! /usr/bin/python -u
import sys
import os
import requests
import paho.mqtt.subscribe as subscribe

odd=True
while True:
  line=subscribe.simple('/esp/sensor', hostname="localhost", retained=False).payload
  #line = sys.stdin.readline().rstrip()
  print('received {}\n'.format((line)))
  if line == '':
    exit(0)
  if line == '449775a':
    print os.popen("curl -X POST https://maker.ifttt.com/trigger/suspend_alarm/with/key/d9z64lBOBRi4KkrFduEYH5").read()
  elif line == 'b218cf21':
    print os.popen("curl -X POST https://maker.ifttt.com/trigger/arm_alarm/with/key/d9z64lBOBRi4KkrFduEYH5").read()
  elif line == '439775a':
    if odd:
      requests.get('http://192.168.1.48/set?c=580001&m=0')
    else:
      requests.get('http://192.168.1.48/set?c=003d09&m=4')
    odd = not odd
  elif line == '441775a':
    if odd:
      requests.get('http://192.168.1.48/set?m=33')
    else:
      requests.get('http://192.168.1.48/set?m=39')
    odd = not odd
  else:
    print os.popen("~/domotux/frontend/command_line/diego.py action=act id=3 state=OFF").read()

