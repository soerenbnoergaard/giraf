package main

import (
    "net/http"
    "fmt"
    "log"

    "github.com/gorilla/websocket"
)

func server(svgchannel chan string) {
    var upgrader = websocket.Upgrader{
        ReadBufferSize:  1024,
        WriteBufferSize: 1024,
    }

    http.HandleFunc("/comm", func(w http.ResponseWriter, r *http.Request) {
        conn, err := upgrader.Upgrade(w, r, nil)

        if err != nil {
            log.Fatal("Error running Upgrade")
            return
        }

        for {
            err = conn.WriteMessage(websocket.TextMessage, []byte(<-svgchannel))
            if  err != nil {
                log.Fatal(err)
                return
            }
        }
    })

    http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
        fmt.Fprintf(w, `
<html>
    <head>
    <script>
        var socket = new WebSocket("ws://localhost:8080/comm");
        socket.onmessage = function (e) {
            document.getElementById("out").innerHTML = e.data;
            console.log(e.data);
        };
    </script>
    </head>
    <body>
    <span id="out">
    </span>
    </body>
</html>`)
    })

    http.ListenAndServe(":8080", nil)
}

