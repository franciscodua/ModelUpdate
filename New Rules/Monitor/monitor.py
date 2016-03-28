import sys
import time

from haproxyConn import HAProxyConnector
from log import Log

ha_conn = HAProxyConnector("/home/ubuntu/haproxy.sock")
log = Log("/home/ubuntu/log.txt")

log.log("Active, Req_rate, Rsp_time")

while True:
    try:
        backend_stats, frontend_stats = ha_conn.get_stats()
        log.log(backend_stats["act"] + "," + frontend_stats["req_rate"] + "," + backend_stats["rtime"])
        time.sleep(1)
    except KeyboardInterrupt:
        log.write()
        sys.exit(0)