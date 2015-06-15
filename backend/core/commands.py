import sqlite3

class Action:
  def __init__(self, name, command):
    self.name = name
    self.command = command
  def __conform__(self, protocol):
    if protocol is sqlite3.PrepareProtocol:
      return "%s;%s" % (self.name, self.command)


def test_routine():
  connexion = sqlite3.connect('test.sqlite3', detect_types=sqlite3.PARSE_DECLTYPES)
  c = connexion.cursor()
  c.execute("create table actions (a action)")
  
  action_l = [Action("toto", "ls"), Action("titi", "toto")]
  for action in action_l:
    c.execute("insert into actions(a) values (?)", (action,))
    print "adding action ", action
  c.execute("select a from actions")
  print("with declared types:", c.fetchone()[0])
  c.close()
  connexion.close()

test_routine()
