#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#include "common/NamedRecord.h"
#include "test_checks.h"

struct Info
{
  int a;
  int b;
};

int test_constructor()
{
  int errors = 0;

  NamedRecord<Info> records;

  TEST_INT(records.count(), 0);

  return errors;
}

int test_basic()
{
  int errors = 0;

  NamedRecord<Info> records;

  Info r;

  r.a = 10;
  r.b = 11;
  records.append("mike", r);

  r.a = 100;
  r.b = 101;
  records.append("kohn", r);

  TEST_INT(records.count(), 2);

  Info *data;

  data = records.find("mike");
  TEST_BOOL((data == nullptr), false);
  TEST_INT(data->a, 10);
  TEST_INT(data->b, 11);

  data = records.find("kohn");
  TEST_BOOL((data == nullptr), false);
  TEST_INT(data->a, 100);
  TEST_INT(data->b, 101);

  data = records.find("roar");
  TEST_BOOL((data == nullptr), true);

  data = records.find_or_append("roar");
  TEST_BOOL((data == nullptr), false);
  data->a = 600;
  data->b = 700;

  TEST_INT(records.count(), 3);

  data = records.find("roar");
  TEST_BOOL((data == nullptr), false);
  TEST_INT(data->a, 600);
  TEST_INT(data->b, 700);

  data->a = 999;
  data->b = 998;

  data = records.find_or_append("roar");
  TEST_BOOL((data == nullptr), false);
  data->a = 999;
  data->b = 998;

  TEST_INT(records.count(), 3);

  Info &info = records.append("ekim");
  info.a = 1999;
  info.b = 2000;

  data = records.find("roar");
  TEST_BOOL((data == nullptr), false);
  TEST_INT(data->a, 999);
  TEST_INT(data->b, 998);

  data = records.find("ekim");
  TEST_BOOL((data == nullptr), false);
  TEST_INT(data->a, 1999);
  TEST_INT(data->b, 2000);

  TEST_INT(records.count(), 4);

  records.dump();

  return errors;
}

int test_iterator()
{
  int errors = 0;

  NamedRecord<Info> records;

  Info r;

  r.a = 0;
  r.b = 0;

  records.append("mike", r);
  r.a += 1;
  records.append("kohn", r);
  r.a += 1;
  records.append("roar", r);
  r.a += 1;
  records.append("ekim", r);
  r.a += 1;
  records.append("22227", r);
  r.a += 1;
  records.append("72222", r);

  TEST_INT(records.count(), 6);

  int i = 0;

  const char *names[] = { "mike", "kohn", "roar", "ekim", "22227", "72222" };

  for (auto record : records)
  {
    printf("%s  %d\n", record->name, record->data.a);

    const int a = record->data.a;
    TEST_BOOL((strcmp(record->name, names[a]) == 0), true);

    i++;
  }

  TEST_INT(i, 6);

  return errors;
}

int test_clear()
{
  int errors = 0;

  NamedRecord<Info> records;

  Info r;

  r.a = 0;
  r.b = 0;

  records.append("mike", r);
  records.append("kohn", r);
  records.append("roar", r);
  records.append("ekim", r);

  TEST_INT(records.count(), 4);

  records.clear();

  TEST_INT(records.count(), 0);

  int i = 0;
  for (auto iter = records.begin(); iter != records.end(); iter++)
  {
    printf("%s  %d\n", iter.record->name, iter.record->data.a);
    i++;
  }

  TEST_INT(i, 0);

  return errors;
}

int test_remove()
{
  int errors = 0;

  NamedRecord<Info> records;

  Info info;

  info.a = 0;
  info.b = 0;

  records.append("kohn", info);
  records.append("mike", info);
  records.append("ekim", info);
  records.append("1222", info);
  records.append("2122", info);
  records.append("2212", info);

  TEST_INT(records.count(), 6);

  records.remove("kohn");
  TEST_INT(records.count(), 5);
  TEST_BOOL((records.find("kohn") == nullptr), true);
  TEST_BOOL((records.find("mike") == nullptr), false);
  TEST_BOOL((records.find("ekim") == nullptr), false);
  TEST_BOOL((records.find("1222") == nullptr), false);
  TEST_BOOL((records.find("2122") == nullptr), false);
  TEST_BOOL((records.find("2212") == nullptr), false);

  records.remove("mike");
  TEST_INT(records.count(), 4);
  TEST_BOOL((records.find("kohn") == nullptr), true);
  TEST_BOOL((records.find("mike") == nullptr), true);
  TEST_BOOL((records.find("ekim") == nullptr), false);
  TEST_BOOL((records.find("1222") == nullptr), false);
  TEST_BOOL((records.find("2122") == nullptr), false);
  TEST_BOOL((records.find("2212") == nullptr), false);

  records.remove("2122");

  TEST_INT(records.count(), 3);
  TEST_BOOL((records.find("kohn") == nullptr), true);
  TEST_BOOL((records.find("mike") == nullptr), true);
  TEST_BOOL((records.find("ekim") == nullptr), false);
  TEST_BOOL((records.find("1222") == nullptr), false);
  TEST_BOOL((records.find("2122") == nullptr), true);
  TEST_BOOL((records.find("2212") == nullptr), false);

  records.remove("2212");

  TEST_INT(records.count(), 2);
  TEST_BOOL((records.find("kohn") == nullptr), true);
  TEST_BOOL((records.find("mike") == nullptr), true);
  TEST_BOOL((records.find("ekim") == nullptr), false);
  TEST_BOOL((records.find("1222") == nullptr), false);
  TEST_BOOL((records.find("2122") == nullptr), true);
  TEST_BOOL((records.find("2212") == nullptr), true);

  records.remove("1222");

  TEST_INT(records.count(), 1);
  TEST_BOOL((records.find("kohn") == nullptr), true);
  TEST_BOOL((records.find("mike") == nullptr), true);
  TEST_BOOL((records.find("ekim") == nullptr), false);
  TEST_BOOL((records.find("1222") == nullptr), true);
  TEST_BOOL((records.find("2122") == nullptr), true);
  TEST_BOOL((records.find("2212") == nullptr), true);

  records.append("mike", info);

  TEST_INT(records.count(), 2);
  TEST_BOOL((records.find("mike") == nullptr), false);
  TEST_BOOL((records.find("ekim") == nullptr), false);

  records.remove("mike");

  TEST_INT(records.count(), 1);
  TEST_BOOL((records.find("mike") == nullptr), true);
  TEST_BOOL((records.find("ekim") == nullptr), false);

  return errors;
}

int test_empty_iterator()
{
  int errors = 0;

  NamedRecord<Info> records;

  int i = 0;
  for (auto iter = records.begin(); iter != records.end(); iter++)
  {
    printf("%s  %d\n", iter.record->name, iter.record->data.a);
    i++;
  }

  TEST_INT(i, 0);

  return errors;
}

int main(int argc, char *argv[])
{
  int errors = 0;

  printf("Testing NamedRecord.h\n");

  errors += test_constructor();
  errors += test_basic();
  errors += test_iterator();
  errors += test_clear();
  errors += test_remove();
  errors += test_empty_iterator();

  if (errors != 0) { printf("NamedRecord.h ... FAILED.\n"); return -1; }

  printf("NamedRecord.h ... PASSED.\n");

  return 0;
}

