package main

import (
	"WayGo/Config"
	"WayGo/Encryption"
	"flag"
	"log"
)

var addrCfg = flag.String("h", ":1230", "listen addr")
var pkeyCfg = flag.String("p", `.\Cert\private.key`, "privateKey path")
var certCfg = flag.String("c", `.\Cert\cert.cer`, "cert path")
var serverKeyCfg = flag.String("k", Config.DefaultServerKey, "server key")

func main() {
	flag.Parse()
	rk, wk := Encryption.GenKey(*serverKeyCfg)
	s, err := CreateServer(*addrCfg, *certCfg, *pkeyCfg, rk, wk)
	if err != nil {
		log.Fatal(err)
	}
	// 测试用
	a, b := Encryption.GenKey("123456")
	p := keyPair{R: a, W: b}
	s.verifyMng.Account.Add("test", p)

	err = s.ListenAndServe()
	if err != nil {
		log.Fatal(err)
	}
}
