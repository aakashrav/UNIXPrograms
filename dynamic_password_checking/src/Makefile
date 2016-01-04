CC=gcc
CFLAGS=-Wall -std=c99 -fPIC -shared -I../include/
LDLFLAGS=-ldl

all: password_checker common_passwords

password_checker: password_checker.c
	$(CC) $(CFLAGS) $(LDLFLAGS) -o ../lib/libpassword_checker.so $<

common_passwords: common_passwords.c
	$(CC) $(CFLAGS) -o ../lib/plugins/libdict_plugin.so $<

clean:
	rm ../lib/plugins/dict_plugin.so
	rm ../lib/password_checker.so

.PHONY: all clean