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
  result = diego_request(action='get')
  if result['9']['status'] == 'ON':
    print ('Le mode debug est actif')

  import pprint
  pprint.pprint(diego_request(action='act', id=9, state='OFF'))
