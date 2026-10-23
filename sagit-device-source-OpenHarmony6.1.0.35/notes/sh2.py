#!/usr/bin/env python3
import sys, time, serial, select

port = sys.argv[1] if len(sys.argv) > 1 else "/dev/ttyACM1"
baud = int(sys.argv[2]) if len(sys.argv) > 2 else 115200


def sh(cmd, timeout=30.0):
    s = None
    for _ in range(40):
        try:
            s = serial.Serial(port, baud, timeout=0.2, exclusive=True)
            break
        except Exception:
            time.sleep(0.3)
    if s is None:
        raise SystemExit("port busy")
    s.reset_input_buffer()
    s.write(("\r" + cmd + "\r").encode())
    end = time.time() + timeout
    buf = b""
    while time.time() < end:
        chunk = s.read(4096)
        if chunk:
            buf += chunk
            end = time.time() + 0.6
    s.close()
    return buf.decode("utf-8", "replace")


if len(sys.argv) > 3:
    print(sh(" ".join(sys.argv[3:])))
else:
    import os
    os.execv(sys.executable, sys.argv[:3] + [os.readlink("/proc/self/fd/0").read().strip()])
