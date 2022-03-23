#!/usr/bin/env python3

import re
import sys
import socket
from http import client as httplib
import unittest

arg_host = "localhost"
arg_port = 80
if len(sys.argv) > 1:
  arg_host = sys.argv[1]
if len(sys.argv) > 2:
  arg_port = int(sys.argv[2])

class HttpServer(unittest.TestCase):
  host = arg_host
  port = arg_port

  def setUp(self):
    self.conn = httplib.HTTPConnection(self.host, self.port, timeout=10)

  def tearDown(self):
    self.conn.close()

  def test_head_method(self):
    """head method support"""

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((self.host, self.port))
    s.send(b"HEAD /httptest/dir2/page.html HTTP/1.0\r\n\r\n")
    data = b""
    while 1:
      buf = s.recv(1024)
      if not buf:
        break
      data += buf
    s.close()

    self.assertTrue(data.find(b"\r\n\r\n") > 0, "no empty line with CRLF found")
    (head, body) = re.split(b"\r\n\r\n", data, 1)
    headers = head.split(b"\r\n")
    self.assertTrue(len(headers) > 0, "no headers found")
    status_line = headers.pop(0)
    proto, code, status = status_line.split(b" ")
    h = {}
    for k, v in enumerate(headers):
      name, value = re.split(b'\\s*:\\s*', v, 1)
      h[name.lower()] = value
    if int(code) == 200:
      self.assertEqual(int(h[b"content-length"]), 38)
      self.assertEqual(len(body), 0)
    else:
      self.assertIn(int(code), (400, 405))

loader = unittest.TestLoader()
suite = unittest.TestSuite()
a = loader.loadTestsFromTestCase(HttpServer)
suite.addTest(a)


class NewResult(unittest.TextTestResult):
  @staticmethod
  def get_description(test):
    doc_first_line = test.shortDescription()
    return doc_first_line or ""


class NewRunner(unittest.TextTestRunner):
  resultclass = NewResult


runner = NewRunner(verbosity=2)
runner.run(suite)