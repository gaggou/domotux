#! /usr/bin/python -u
import requests
import paho.mqtt.subscribe as subscribe
import json

class MessageHandler:
  def __init__(self, action_file, verbose = False):
    self.index = 0
    self.verbose = verbose
    with open(action_file, 'r') as f:
        self.actions = json.load(f)

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

subscribe.callback(MessageHandler('actions.json'), '/esp/sensor')

