#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiredtiger.h>
static const char *home;

void
CheckOp(const int ret, const char* funcname, const char* home) {
  if (ret != 0) {
    fprintf(stderr, "%s -- Error connecting to %s: %s\n",
          funcname, home, wiredtiger_strerror(ret));
  }
}

int
main(void)
{
  WT_CONNECTION *conn;
  WT_SESSION *session;
  WT_CURSOR *cursor;
  int ret;

  /* 打开一个wiredtiger要操作的db目录*/
  if (getenv("WIREDTIGER_HOME") == NULL) {
    home = "WT_HOME";
    ret = system("rm -rf WT_HOME && mkdir WT_HOME");
  } else {
    home = NULL;
  }

  /* 打开或者创建数据库 */
  CheckOp(wiredtiger_open(home, NULL, "create", &conn), "wiredtiger_open", home);
  /* 打开1个数据库连接 */
  CheckOp(conn->open_session(conn, NULL, NULL, &session), "open_session", home);
  /* 创建一张名字为access的表，后续的db文件会以表的名字来命名 */
  CheckOp(session->create(session, "table:access", "key_format=S,value_format=S"), "create", home);
  /* 创建一个cursor，所有的请求都通过cursor来访问 */
  CheckOp(session->open_cursor(session, "table:access", NULL, NULL, &cursor), "open_cursor", home);

  /* 向一个cursor 插入一个KV */
  cursor->set_key(cursor, "key1");
  cursor->set_value(cursor, "value1");

  char* key = NULL;
  /* 获取key , 当然，获取value也是同样的方式，通过get_value即可*/
  CheckOp(ret = cursor->get_key(cursor, &key),"get_key", home);
  printf("wt key is : %s", key);

  CheckOp(ret = conn->close(conn, NULL),"close", home);
  return (ret);
}
