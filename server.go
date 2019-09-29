package main

import (
	"fmt"
	"log"
	"net/http"

	"github.com/gorilla/websocket"
)

func server(svgchannel chan string) {
	var upgrader = websocket.Upgrader{
		ReadBufferSize:  1024,
		WriteBufferSize: 1024,
	}

    // Websocket port
	http.HandleFunc("/comm", func(w http.ResponseWriter, r *http.Request) {
        conn, err := upgrader.Upgrade(w, r, nil)

        if err != nil {
            log.Fatal("Error running upgrade")
            return
        }

        for {
            err = conn.WriteMessage(websocket.TextMessage, []byte(<-svgchannel))
            if err != nil {
                println("Connection closed")
                break
            }
        }
	})

    // HTTP port
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprintf(w, `
<html>
    <head>
    <script>
        var socket = new WebSocket("ws://localhost:8080/comm");
        socket.onmessage = function (e) {
            document.getElementById("out").innerHTML = e.data;
        };
    </script>
    </head>
    <body>
    <span id="out">
    </span>
    </body>
</html>`)
	})

    fmt.Printf("Running on:\nhttp://localhost:%d\n", settings.port)
    _ = http.ListenAndServe(fmt.Sprintf(":%d", settings.port), nil)
}
