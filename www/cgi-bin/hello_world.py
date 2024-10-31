#!/usr/bin/env python3
import sys
import time
print("HTTP/1.1 200 OK\r")
print("Content-Type: text/html\r")
print("Content-Length: 64\r\n\r")
print("<html><body>")
print("<h1>Hello, World from Python CGI!</h1>")
print("</body></html>")
sys.exit(0)