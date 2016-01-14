#
# SMTP server with odd behaviour
#
import smtpd
import random
import time
import socket
import asyncore

#
# HellSocket -- socket with odd behaving send()/recv()
#
class HellSocket(object):
	def __init__(self, s):
		self._s = s

	def __getattr__(self, name):
		return getattr(self._s, name)

	def send(self, buf):
		rv = 0
		i = 0
		upper = len(buf)
		while i < upper:
			x = random.randint(i, upper)
			sent = self._s.send(buf[i:x])
			if sent == 0:
				break
			rv += sent
			i += sent

			if random.randint(0, 3) == 0:
				time.sleep(1)
		return rv

	def recv(self, bufsize, flags = 0):
		if random.randint(0, 4) == 0:
			time.sleep(2)

		return self._s.recv(bufsize, flags)

class HellSMTP(smtpd.SMTPServer):
	def __init__(self, local_addr, remote_addr, known_rcpts = None):
		smtpd.SMTPServer.__init__(self, local_addr, remote_addr)
		random.seed()
		self.error = \
		"451  Requested action aborted: local error in processing"
		if known_rcpts == None:
			self.known_rcpts = []
		else:
			self.known_rcpts = known_rcpts

	def process_message(self, peer, mailfrom, rcpttos, data, **kwargs):
		rv = None	# None is success here
		for i in self.known_rcpts:
			rv = self.error	# assume recipient is unknown
			for j in rcpttos:
				found = i.toupper() == j.toupper()
				if found:
					# recepient found, pick up victim
					# with 20% probability to send
					# error
					if random.randint(0, 4) == 0:
						return self.error
					else:
						return None
		# no list of recipients to match rcpttos list
		# then pick up victim with 50% chance. victim
		# will see error message after final CRLF.CRLF
		if random.randint(0, 1) == 0:
			rv = self.error
		return rv

	def accept(self):
		sock, addr = smtpd.SMTPServer.accept(self)
		# pick up victim with 1/3 probability
		if random.randint(0, 2) == 0:
			return (HellSocket(sock), addr)
		else:
			return (sock, addr)

if __name__ == "__main__":

	servers = []	# bind 2525 - 2529 ports
	# for i in [ 2525, 2526, 2527, 2528, 2529 ]:
	for i in [2525,2526]:
		# make sure hostname is correct
		# server = HellSMTP(("u2-0.ms.mff.cuni.cz", i), None)
		server = HellSMTP(("127.0.0.1", i), None)
		servers.append(server)

	smtpd.asyncore.loop()