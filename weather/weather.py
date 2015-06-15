import urllib2, urllib, json
baseurl = "https://query.yahooapis.com/v1/public/yql?"
#baseurl = "http://weather.yahooapis.com/forecastrss?"
yql_query = "select item.condition from weather.forecast where woeid=598429"
yql_query += " and u=\"c\""
yql_url = baseurl + urllib.urlencode({'q':yql_query}) + "&format=json"
result = urllib2.urlopen(yql_url).read()
data = json.loads(result)
code= int(data['query']['results']['channel']['item']['condition']['code'])
cond_text= data['query']['results']['channel']['item']['condition']['text']
rain_translator= { # Simplifies results
  0: True, # tornado
  1: False, # tropical storm
  2: True, # hurricane
  3: False, # severe thunderstorms
  4: False, # thunderstorms
  5: False, # mixed rain and snow
  6: False, # mixed rain and sleet
  7: False, # mixed snow and sleet
  8: False, # freezing drizzle
  9: True, # drizzle
  10: False, # freezing rain
  11: False, # showers
  12: False, # showers
  13: False, # snow flurries
  14: False, # light snow showers
  15: False, # blowing snow
  16: False, # snow
  17: False, # hail
  18: False, # sleet
  19: True, # dust
  20: True, # foggy
  21: True, # haze
  22: True, # smoky
  23: True, # blustery
  24: True, # windy
  25: True, # cold
  26: True, # cloudy
  27: True, # mostly cloudy (night)
  28: True, # mostly cloudy (day)
  29: True, # partly cloudy (night)
  30: True, # partly cloudy (day)
  31: True, # clear (night)
  32: True, # sunny
  33: True, # fair (night)
  34: True, # fair (day)
  35: False, # mixed rain and hail
  36: True, # hot
  37: False, # isolated thunderstorms
  38: False, # scattered thunderstorms
  39: False, # scattered thunderstorms
  40: False, # scattered showers
  41: False, # heavy snow
  42: False, # scattered snow showers
  43: False, # heavy snow
  44: True, # partly cloudy
  45: False, # thundershowers
  46: False, # snow showers
  47: False, # isolated thundershowers
  3200: True, # not available
}
negation = ""
if not rain_translator[code]:
  negation = "not "

print "As forecast is " + cond_text + ", I will " + negation + "switch the sprinklers on"

if rain_translator[code]:
  exit(0)
else:
  exit(1)
