#!/usr/bin/python3

import pycurl
import json
from io import BytesIO
from urllib.parse import urlencode

def diego_request(**post_data):
  """docstring for diego_request"""
  buffer = BytesIO()
  c = pycurl.Curl()
  c.setopt(c.URL, 'http://localhost/diego/diego.php')

# Form data must be provided already urlencoded.
  postfields = urlencode(post_data)
# Sets request method to POST,
# Content-Type header to application/x-www-form-urlencoded
# and data to send in request body.
  c.setopt(c.POSTFIELDS, postfields)

  c.setopt(c.WRITEDATA, buffer)
  c.perform()
  c.close()

  body = buffer.getvalue()
# Body is a byte string.
# We have to know the encoding in order to print it to a text file
# such as standard output.

  return json.loads(body.decode('iso-8859-1'))

if __name__ == '__main__':
  from pprint import pprint
  from argparse import ArgumentParser
  parser = ArgumentParser(description='Send command to Diego.')
  parser.add_argument('-v', dest='verbose', action='store_true',
                                          help='Set verbose')

  parser.add_argument('params', nargs='+', help='other params, in format a=b')

  args = parser.parse_args()
  diego_params = {}
  for parameter in args.params:
    key, value = parameter.split("=")
    diego_params[key] = value
  if args.verbose:
    pprint(diego_params)

  result = diego_request(**diego_params)
  pprint(result)
