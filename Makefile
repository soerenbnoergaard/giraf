.PHONY: all build install uninstall sim

all: build

sim: build
	@perl -e '$$|++;use Time::HiRes qw(usleep);while (1) {print("0,".(rand())."\n"); usleep(100_000)}' | ./giraf

build:
	@go build giraf.go

install:
	@go install giraf.go
