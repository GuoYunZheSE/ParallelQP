#include <iostream>
#include <db_cxx.h>
#include <cstdlib>
#include <cstdio>


using namespace std;

Db db(NULL, 0);

int main() {
	db.open(NULL, "D.db", NULL, DB_HASH, DB_CREATE, 0);
	char a[] = "123";
	char b[] = "456";
	Dbt key(a, sizeof(a));
	Dbt data(b, sizeof(b));
	Dbt result;

	db.put(NULL, &key, &data, 0);
	db.get(NULL, &key, &result, 0);

	char* c = (char*)result.get_data();
	cout << c << endl;
}
