.PHONY: all build install uninstall

TARGET = "github.com/soerenbnoergaard/giraf"

all: build

build:
	@go build $(TARGET)

install:
	@go install $(TARGET)
