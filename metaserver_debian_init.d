#! /bin/sh
#
# metaserver	Start/Stop metaserver
#
#		Based on /etc/init.d/skeleton
#		Written by Miquel van Smoorenburg <miquels@cistron.nl>.
#		Modified for Debian GNU/Linux
#		by Ian Murdock <imurdock@gnu.ai.mit.edu>.
#
# Version:	@(#)metaserver  1.0  20-Feb-2004  dragonm@hypercubepc.com
#

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
DAEMON=/usr/local/bin/metaserver
DAEMONARGS='-d -q'
NAME=metaserver
DESC="WorldForge Metaserver"

test -x $DAEMON || exit 0

set -e

case "$1" in
  start)
	echo -n "Starting $DESC: $NAME"
	start-stop-daemon --start --quiet --pidfile /var/run/$NAME.pid \
		--chuid metaserver --exec $DAEMON -- $DAEMONARGS
	echo "."
	;;
  stop)
	echo -n "Stopping $DESC: $NAME "
	start-stop-daemon --stop --quiet --pidfile /var/run/$NAME.pid \
		--exec $DAEMON
	echo "."
	;;
  #reload)
	#
	#	If the daemon can reload its config files on the fly
	#	for example by sending it SIGHUP, do it here.
	#
	#	If the daemon responds to changes in its config file
	#	directly anyway, make this a do-nothing entry.
	#
	# echo -n "Reloading $DESC configuration..."
	# start-stop-daemon --stop --signal 1 --quiet --pidfile \
	#	/var/run/$NAME.pid --exec $DAEMON
	# echo "done."
  #;;
  restart|force-reload)
	#
	#	If the "reload" option is implemented, move the "force-reload"
	#	option to the "reload" entry above. If not, "force-reload" is
	#	just the same as "restart".
	#
	echo -n "Restarting $DESC: $NAME"
	start-stop-daemon --stop --quiet --pidfile /var/run/$NAME.pid \
		--exec $DAEMON
	sleep 1
	start-stop-daemon --start --quiet --pidfile /var/run/$NAME.pid \
		--chuid metaserver --exec $DAEMON -- $DAEMONARGS
	echo "."
	;;
  *)
	N=/etc/init.d/$NAME
	# echo "Usage: $N {start|stop|restart|reload|force-reload}" >&2
	echo "Usage: $N {start|stop|restart|force-reload}" >&2
	exit 1
	;;
esac

exit 0
