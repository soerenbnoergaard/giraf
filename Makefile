.PHONY: all install uninstall

all:
	go build giraf.go

install:
	go install giraf.go
