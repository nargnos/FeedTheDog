package main

import (
	"flag"

	"WayGo/Config"
	"WayGo/Encryption"
)

var skipVerify = flag.Bool("s", false, "skip verify")
var addrCfg = flag.String("h", "", "address")
var serverNameCfg = flag.String("n", Config.ServerName, "address")
var serverKeyCfg = flag.String("k", Config.DefaultServerKey, "server key")

func main() {
	flag.Parse()
	wk, rk := Encryption.GenKey("123456")
	swk, srk := Encryption.GenKey(*serverKeyCfg)
	c := Client{
		ListenPort:     1234,
		ServerAddress:  ":1230",
		Username:       "test",
		ReadKey:        rk,
		WriteKey:       wk,
		ServerReadKey:  srk,
		ServerWriteKey: swk,
	}

	c.ListenAndServe()
}
