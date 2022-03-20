CC = gcc
CFLAGS = -Wall -Werror -Wextra -Wpedantic -O3 # -S -g

make: MemComBenchmarking

MemComBenchmarking: MemComBenchmarking.c
	$(CC) -o MemComBenchmarking MemComBenchmarking.c $(CFLAGS)