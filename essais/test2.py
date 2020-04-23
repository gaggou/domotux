#! /usr/bin/python -u
import sys
import os
import requests
import paho.mqtt.subscribe as subscribe


class MessageHandler:
  def __init__(self, verbose = False):
    self.odd = True
    self.verbose = verbose
  def __call__(self, client, userdata, message):
    self.odd = not self.odd
    line = message.payload
    if self.verbose:
      print('received "{}"'.format((line)))
    if line == '439775a':
      if self.odd:
        self.request('http://192.168.1.48/set?c=580001&m=0')
      else:
        self.request('http://192.168.1.48/set?c=003d09&m=4')
    elif line == '441775a':
      if self.odd:
        self.request('http://192.168.1.48/set?m=33')
      else:
        self.request('http://192.168.1.48/set?m=39')
    elif line == 'toggleVerbose':
      self.verbose = not self.verbose
      if self.verbose:
        print('Verbose on')
  def request(self, url):
    if self.verbose:
      print('Request {}'.format(url))
    requests.get(url)


#line=subscribe.simple('/esp/sensor', hostname="localhost", retained=False).payload
on_call = MessageHandler()
subscribe.callback(on_call, '/esp/sensor')


