#! /usr/bin/python -u
import sys
import os
import requests
import paho.mqtt.subscribe as subscribe


class MessageHandler:
  def __init__(self, verbose = False):
    self.index = 0
    self.verbose = verbose
    self.actions = {'439775a': ('http://192.168.1.48/set?c=580001&m=0', 'http://192.168.1.48/set?c=003d09&m=4'),
      '441775a': ('http://192.168.1.48/set?m=33', 'http://192.168.1.48/set?m=39')}
  def __call__(self, client, userdata, message):
    try:
      self.index = (self.index + 1) % 2
      line = message.payload
      if self.verbose:
        print('received "{}"'.format((line)))
      if line == 'toggleVerbose':
        self.verbose = not self.verbose
        if self.verbose:
          print('Verbose on')
      else:
        self.request(self.actions[line][self.index])
    except Exception as e:
      print e
  def request(self, url):
    if self.verbose:
      print('Request {}'.format(url))
    requests.get(url)


#line=subscribe.simple('/esp/sensor', hostname="localhost", retained=False).payload
on_call = MessageHandler()
subscribe.callback(on_call, '/esp/sensor')


