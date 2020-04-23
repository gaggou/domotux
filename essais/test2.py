#! /usr/bin/python -u
import sys
import os
import requests
import paho.mqtt.subscribe as subscribe


def callback_user1(client, userdata, message):
    if not hasattr(callback_user1, 'odd'):
        callback_user1.odd = True
    odd = callback_user1.odd = not callback_user1.odd
    line = message.payload
    print('received "{}"'.format((line)))
    if line == '439775a':
      if odd:
        requests.get('http://192.168.1.48/set?c=580001&m=0')
      else:
        requests.get('http://192.168.1.48/set?c=003d09&m=4')
    elif line == '441775a':
      if odd:
        requests.get('http://192.168.1.48/set?m=33')
      else:
        requests.get('http://192.168.1.48/set?m=39')


#line=subscribe.simple('/esp/sensor', hostname="localhost", retained=False).payload
subscribe.callback(callback_user1, '/esp/sensor')


