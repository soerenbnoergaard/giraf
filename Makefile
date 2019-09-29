.PHONY: all build install uninstall sim

all: build

RATE_US = 50_000
sim: build
	@perl -e '$$|++;use Time::HiRes qw(usleep);while (1) {print("0,".(rand())."\n"); usleep($(RATE_US))}' | ./giraf

build:
	@go build giraf.go

install:
	@go install giraf.go
