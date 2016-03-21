#include "redis_cli.h"

#include <stdio.h>
#include <errno.h>
#include "xdebug.h"

using namespace pink;

int main() {
  std::string str;
  int i = 5;

  int ret = RedisCli::SerializeCommand(&str, "HSET %s %d", "key", i);
  printf ("1. Serialize by va return %d, (%s)\n", ret, str.c_str());

  RedisCmdArgsType argv;
  argv.push_back("hset");
  argv.push_back("key");
  argv.push_back(std::to_string(5));

  ret = RedisCli::SerializeCommand(argv, &str);
  printf ("2. Serialize by argv return %d, (%s)\n", ret, str.c_str());

  RedisCli *rcli = new RedisCli();
  rcli->set_connect_timeout(8000);

  Status s = rcli->Connect("127.0.0.1", 9824);

  // Test connect timeout with a non-routable IP
  //Status s = rcli->Connect("10.255.255.1", 9824);

  log_info(" RedisCli Connect return %s", s.ToString().c_str());
  if (!s.ok()) {
      printf ("Connect failed, %s\n", s.ToString().c_str());
      exit(-1);
  }

  ret = rcli->set_send_timeout(8000);
  if (ret < 0) {
    log_warn("set send timeout 8000 ms, return %d", ret);
  } else {
    log_info("set send timeout 8000 ms, return %d", ret);
  }

  std::string ping = "*1\r\n$4\r\nping\r\n";
  for (int i = 0; i < 10; i++) {
    s = rcli->Send(&ping);
    log_info("Send %d: %s", i, s.ToString().c_str());

    s = rcli->Recv(NULL);
    log_info("Recv %d: return %s, reply is (%s)", i, s.ToString().c_str(), rcli->argv_[0].c_str());
  }

  return 0;
}
