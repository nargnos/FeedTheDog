package main

// 用作性能参考
import (
	"flag"
	"fmt"
	"net"
	"os"
)

// 用作性能参考
var ep = flag.String("a", "localhost", "address")

func main() {
	flag.Parse()
	addr, err := net.ResolveTCPAddr("tcp", *ep)
	if err != nil {
		fmt.Println(err)
		os.Exit(-1)
	}
	act, err := net.ListenTCP("tcp", addr)
	for {
		conn, err := act.AcceptTCP()
		if err != nil {
			fmt.Println(err)
			break
		}
		go func(c *net.TCPConn) {
			buf := make([]byte, 8192)
			for {
				reads, err := c.Read(buf)
				if err != nil {
					return
				}
				_, err = c.Write(buf[:reads])
				if err != nil {
					return
				}
			}
		}(conn)
	}
}
