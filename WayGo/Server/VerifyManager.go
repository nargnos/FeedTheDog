package main

import (
	"log"
	"sync"
)

type darkRoom struct {
	names map[string]int
	mtx   sync.RWMutex
}

// 在小黑屋里返回true
func (d *darkRoom) Check(ip string) bool {
	d.mtx.RLock()
	defer d.mtx.RUnlock()
	return d.names[ip] > 5
}
func (d *darkRoom) Add(ip string) {
	d.mtx.Lock()
	defer d.mtx.Unlock()
	if n, exist := d.names[ip]; exist {
		if n < 10 {
			d.names[ip] = n + 1
			if n+1 >= 5 {
				defer log.Println(ip, " in dark room")
			}
		}
		return
	}
	d.names[ip] = 1
}
func (d *darkRoom) LockIn(ip string) {
	defer log.Println(ip, " in dark room")
	d.mtx.Lock()
	defer d.mtx.Unlock()
	d.names[ip] = 10
}

type keyPair struct {
	R []byte
	W []byte
}
type accountMap struct {
	accounts map[string]keyPair
	mtx      sync.RWMutex
}

func (a *accountMap) GetKeys(username string) *keyPair {
	a.mtx.RLock()
	defer a.mtx.RUnlock()
	if val, ok := a.accounts[username]; ok {
		return &val
	}
	return nil
}
func (a *accountMap) Add(username string, val keyPair) {
	a.mtx.Lock()
	defer a.mtx.Unlock()
	a.accounts[username] = val
}

type verifyManager struct {
	DarkRoom darkRoom
	Account  accountMap
}

func createVerifyManager() *verifyManager {
	ret := verifyManager{}
	ret.Account.accounts = make(map[string]keyPair)
	ret.DarkRoom.names = make(map[string]int)
	return &ret
}
