package main

// 弄个简单的测试，只测正确性，否则每次都开个jm太费时间
// 有点乱，不管了

import (
	"encoding/binary"
	"flag"
	"fmt"
	"io"
	"math/rand"
	"net"
	"os"
	"sync"
	"sync/atomic"
	"time"
)

const defaultCount = 50000
const defaultTimeout = 5000

var ep = flag.String("a", "localhost", "address")

// 最大运行数量，到这个数就停止
var count = flag.Uint64("c", defaultCount, "max count")

// 连接超时时间
var timeout = flag.Int("t", defaultTimeout, "timeout(ms)")

var addr *net.TCPAddr
var succCount uint64
var errCount uint64
var echoCount uint64
var createCount uint64
var stop int64

func main() {
	checkFlags()
	doEchoTest()
}

func checkFlags() {
	flag.Parse()
	var err error
	addr, err = net.ResolveTCPAddr("tcp", *ep)
	if err != nil {
		fmt.Println(err)
		os.Exit(-1)
	}
	fmt.Printf("addr: %s\r\n", addr.String())

	if *timeout <= 0 {
		*timeout = defaultTimeout
	}
}
func durConv(val int) (ret time.Duration) {
	ret = time.Millisecond * (time.Duration)(val)
	return
}
func doEchoTest() {
	conn, err := net.DialTCP("tcp", nil, addr)
	if err != nil {
		fmt.Println(err)
		os.Exit(-1)
	}
	defer conn.Close()
	buf := make([]byte, 8)
	i := rand.Uint64()
	binary.LittleEndian.PutUint64(buf, i)
	if err = doEcho(conn, buf, i); err != nil {
		fmt.Println("echo err")
		fmt.Println(err)
		os.Exit(-1)
	}
	// 一些可以扩展的参数
	connPerSec := 32
	sleepDur := (time.Duration)((int)(time.Second) / connPerSec)

	fmt.Println("Start")
	var wg sync.WaitGroup
	t := time.Now()
	for isStop() {
		wg.Add(connPerSec)
		for i := 0; i < connPerSec; i++ {
			go func() {
				// 连接创建后不关闭一直累积直到结束, 直到停止，过程中发生错误算失败
				atomic.AddUint64(&createCount, 1)
				defer wg.Done()
				for isStop() {
					if e := echo(); e == nil {
						atomic.AddUint64(&succCount, 1)
					} else {
						once.Do(func() {
							// 首次出现错误
							fmt.Printf("error: conn:%d echo:%d\r\n", createCount, echoCount)
							fmt.Println(e)
						})
						if e == io.EOF {
							stopLoop()
							return
						}
						atomic.AddUint64(&errCount, 1)
					}
				}
			}()
			time.Sleep(sleepDur)
		}
	}
	wg.Wait()
	dur := time.Now().Sub(t)
	avg := echoCount / (uint64)(dur.Seconds())

	// 统计
	fmt.Println("============")
	fmt.Printf("> create:%d succEnd:%d errEnd:%d echo:%d time:%s avg:%d",
		succCount+errCount, succCount, errCount, echoCount, dur, avg)
}
func stopLoop() {
	atomic.StoreInt64(&stop, 1)
}
func isStop() bool {
	return atomic.LoadInt64(&stop) == 0
}

var once sync.Once

func echo() (err error) {
	conn, err := net.DialTCP("tcp", nil, addr)
	if err != nil {
		return
	}
	defer conn.Close()

	buf := make([]byte, 8)
	i := rand.Uint64()
	binary.LittleEndian.PutUint64(buf, i)
	for isStop() {
		if err = doEcho(conn, buf, i); err != nil {
			return
		}
		atomic.AddUint64(&echoCount, 1)
		if atomic.LoadUint64(&echoCount) >= *count {
			stopLoop()
		}
	}
	return nil
}

func doEcho(conn *net.TCPConn, buf []byte, i uint64) (err error) {
	conn.SetWriteDeadline(time.Now().Add(durConv(*timeout)))
	_, err = conn.Write(buf)
	if err != nil {
		return
	}
	conn.SetReadDeadline(time.Now().Add(durConv(*timeout)))
	_, err = io.ReadFull(conn, buf)
	if err != nil {
		return
	}
	rdata := binary.LittleEndian.Uint64(buf)
	if rdata != i {
		panic("echo error: not equal")
	}
	return nil
}
