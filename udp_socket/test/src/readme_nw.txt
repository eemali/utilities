(1) UDPEcho
	Assumption: local ip address : 192.168.3.22
	(a) run UDPEchoServer on terminal 
	    e.g ./UDPEchoServer 5050
		5050 is a port #
	(b) run UDPEchoClient on anoter terminal
		e.g. ./UDpEchoClient 192.169.3.22 Hello_World!! 5050
		
 (2) Multicast
	(a) run MulticastReceiver in multiple terminal on multiple system 
		e.g. ./MulticastReceiver 225.0.0.37 5050
	(b) run MulticastSender 
		e.g. ./MulticastSender 225.0.0.37 5050 Hello_World!!!
	
	command to see route table
	netstat -nr
	
	command to add entry in route table 
	ip route add 224.0.0.0/4 dev eth0
	
 (3) Broadcast
	Assumption: local ip address : 192.168.3.22
	(a) run BroadcastReceiver in multiple terminal on multiple system 
		e.g. ./BroadcastReceiver 5050
	(b) run BroadcastSender
		e.g. ./BroadcastSender 192.168.3.255 5050 Hello_World!!!