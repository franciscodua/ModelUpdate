import sys
import time

from log import Log
from haproxyConn import HAProxyConnector

ha_conn = HAProxyConnector("/home/ubuntu/haproxy.sock")
log = Log("/home/ubuntu/log.txt")

log.log("Active, Req_rate, Rsp_time")

while True:
    try:
        stats = ha_conn.get_stats()
        log.log(stats["act"] + "," + stats["req_rate"] + "," + stats["rtime"])
        time.sleep(1)
    except KeyboardInterrupt:
        log.write()
        sys.exit(0)