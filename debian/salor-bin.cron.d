#
# Regular cron jobs for the salor-bin package
#
0 4	* * *	root	[ -x /usr/bin/salor-bin-maintenance ] && /usr/bin/salor-bin-maintenance
