.PHONY: all build install uninstall sim

TARGET = "github.com/soerenbnoergaard/giraf"
SIM_RATE_US = 25_000

all: build

sim: build
	@perl -e '$$|++;use Time::HiRes qw(usleep);while (1) {print("0,".(rand()-0.5)."\n"); usleep($(SIM_RATE_US))}' | ./giraf

build:
	@go build $(TARGET)

install:
	@go install $(TARGET)
