CC=gcc
CFLAGS=-shared -I../include/

all: password_checker common_passwords

password_checker: password_checker.c
	$(CC) $(CFLAGS) -o ../lib/password_checker.so $<

common_passwords: common_passwords.c
	$(CC) $(CFLAGS) -o ../lib/plugins/dict_plugin.so $<

clean:
	rm ../lib/plugins/dict_plugin.so
	rm ../lib/password_checker.so

.PHONY: all clean