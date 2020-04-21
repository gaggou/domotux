#! /usr/bin/python -u
import sys
import os
import requests
odd=True
while True:
  line = sys.stdin.readline()
  if line == '':
    exit(0)
  if line == '449775a\n':
    print os.popen("curl -X POST https://maker.ifttt.com/trigger/suspend_alarm/with/key/d9z64lBOBRi4KkrFduEYH5").read()
  elif line == 'b218cf21\n':
    print os.popen("curl -X POST https://maker.ifttt.com/trigger/arm_alarm/with/key/d9z64lBOBRi4KkrFduEYH5").read()
  elif line == '439775a\n':
    if odd:
      requests.get('http://192.168.1.48/set?c=580001&m=0')
    else:
      requests.get('http://192.168.1.48/set?c=003d09&m=4')
    odd = not odd
  elif line == '441775a\n':
    if odd:
      requests.get('http://192.168.1.48/set?m=33')
    else:
      requests.get('http://192.168.1.48/set?m=39')
    odd = not odd
  else:
    print os.popen("~/domotux/frontend/command_line/diego.py action=act id=3 state=OFF").read()

