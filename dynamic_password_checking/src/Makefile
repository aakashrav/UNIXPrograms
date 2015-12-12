CC=gcc
CFLAGS=-dynamiclib -I../include/

all: password_checker common_passwords

password_checker: password_checker.c
	$(CC) $(CFLAGS) -o ../lib/password_checker.dylib $<

common_passwords: common_passwords.c
	$(CC) $(CFLAGS) -o ../lib/plugins/dict_plugin.dylib $<

clean:
	rm ../lib/plugins/dict_plugin.dylib
	rm ../lib/password_checker.dylib

.PHONY: all clean