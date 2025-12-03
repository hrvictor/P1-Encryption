You need openssl to run this program.

To compile the program, use the following command:
gcc -o p1program main.c \
  -I/opt/homebrew/opt/openssl/include \
  -L/opt/homebrew/opt/openssl/lib \
  -lcrypto