# Kingdom Network Functions
# Bu dosya networkmodule.py'ye eklenecek fonksiyonları içerir

def SendCreateKingdomPacket(name, color, flag):
	"""Send create kingdom packet to server"""
	import net
	import struct
	
	# Pack kingdom data
	packet = struct.pack("!I20s3BI", 
		len(name),  # name length
		name.encode('utf-8').ljust(20, b'\0'),  # kingdom name (20 bytes)
		color[0],   # red
		color[1],   # green  
		color[2],   # blue
		flag        # flag design index
	)
	
	net.SendPacket(net.HEADER_CG_CREATE_KINGDOM, packet)

def SendJoinKingdomPacket(kingdomId):
	"""Send join kingdom packet to server"""
	import net
	import struct
	
	packet = struct.pack("!I", kingdomId)
	net.SendPacket(net.HEADER_CG_JOIN_KINGDOM, packet)

def SendRequestKingdomListPacket():
	"""Request list of available kingdoms"""
	import net
	
	net.SendPacket(net.HEADER_CG_REQUEST_KINGDOM_LIST, "")

def SendLeaveKingdomPacket():
	"""Leave current kingdom"""
	import net
	
	net.SendPacket(net.HEADER_CG_LEAVE_KINGDOM, "")

def SendKingdomInvitePacket(playerName):
	"""Invite player to kingdom"""
	import net
	import struct
	
	packet = struct.pack("!20s", playerName.encode('utf-8').ljust(20, b'\0'))
	net.SendPacket(net.HEADER_CG_KINGDOM_INVITE, packet)

def SendKingdomKickPacket(playerName):
	"""Kick player from kingdom"""
	import net
	import struct
	
	packet = struct.pack("!20s", playerName.encode('utf-8').ljust(20, b'\0'))
	net.SendPacket(net.HEADER_CG_KINGDOM_KICK, packet)

def SendKingdomRankPacket(playerName, rank):
	"""Change player rank in kingdom"""
	import net
	import struct
	
	packet = struct.pack("!20sB", 
		playerName.encode('utf-8').ljust(20, b'\0'),
		rank
	)
	net.SendPacket(net.HEADER_CG_KINGDOM_RANK, packet)

def SendKingdomSettingsPacket(settings):
	"""Update kingdom settings"""
	import net
	import struct
	
	packet = struct.pack("!20s3BI100s", 
		settings['name'].encode('utf-8').ljust(20, b'\0'),
		settings['color'][0],
		settings['color'][1], 
		settings['color'][2],
		settings['flag'],
		settings['description'].encode('utf-8').ljust(100, b'\0')
	)
	net.SendPacket(net.HEADER_CG_KINGDOM_SETTINGS, packet)

# Packet headers to be added to constants
HEADER_CG_CREATE_KINGDOM = 200
HEADER_CG_JOIN_KINGDOM = 201
HEADER_CG_LEAVE_KINGDOM = 202
HEADER_CG_REQUEST_KINGDOM_LIST = 203
HEADER_CG_KINGDOM_INVITE = 204
HEADER_CG_KINGDOM_KICK = 205
HEADER_CG_KINGDOM_RANK = 206
HEADER_CG_KINGDOM_SETTINGS = 207

HEADER_GC_KINGDOM_LIST = 200
HEADER_GC_KINGDOM_INFO = 201
HEADER_GC_KINGDOM_MEMBER_LIST = 202
HEADER_GC_KINGDOM_INVITE = 203
HEADER_GC_KINGDOM_JOIN_RESULT = 204
HEADER_GC_KINGDOM_LEAVE_RESULT = 205
