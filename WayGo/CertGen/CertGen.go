package main

import (
	"WayGo/Config"
	"crypto/ecdsa"
	"crypto/elliptic"
	rand "crypto/rand"
	"crypto/x509"
	"crypto/x509/pkix"
	"encoding/binary"
	"flag"
	"fmt"
	"io/ioutil"
	"math/big"
	mathrand "math/rand"
	"net"
	"time"
)

func NewCert(name string, ipAddr []net.IP) *x509.Certificate {
	subjectKeyIdBuf := make([]byte, 8)
	binary.LittleEndian.PutUint64(subjectKeyIdBuf, mathrand.Uint64())

	return &x509.Certificate{
		SerialNumber: big.NewInt(mathrand.Int63()),
		Subject: pkix.Name{
			// 其它信息如国家名什么的无所谓
			CommonName: name,
		},
		// 这里用来做服务器验证的
		//	IPAddresses: ipAddr,
		NotBefore: time.Now(),
		NotAfter:  time.Now().AddDate(10+mathrand.Intn(100), mathrand.Intn(12), mathrand.Intn(30)),
		// 使用者密钥标识
		SubjectKeyId:          subjectKeyIdBuf,
		BasicConstraintsValid: true,
		IsCA: true,
		// 增强型密钥用法
		ExtKeyUsage: []x509.ExtKeyUsage{x509.ExtKeyUsageServerAuth},
		KeyUsage:    x509.KeyUsageDigitalSignature | x509.KeyUsageCertSign,
	}
}

var ipCfg = flag.String("h", "", "host")
var nameCfg = flag.String("n", Config.ServerName, "name")

func getIP() []net.IP {
	var ipAddr []net.IP
	if len(*ipCfg) == 0 {
		ip, err := net.InterfaceAddrs()
		if err != nil {
			panic(err)
		}
		for _, v := range ip {
			if i, ok := v.(*net.IPNet); ok {
				ipAddr = append(ipAddr, i.IP)
			}
		}
	} else {
		ipAddr = append(ipAddr, net.ParseIP(*ipCfg))
	}
	return ipAddr
}

func getName() string {
	if len(*nameCfg) == 0 {
		return defaultName
	}
	return *nameCfg
}

func main() {
	flag.Parse()

	ip := getIP()
	name := getName()

	cert := NewCert(name, ip)

	priv, _ := ecdsa.GenerateKey(elliptic.P256(), rand.Reader)
	pub := &priv.PublicKey
	certBin, err := x509.CreateCertificate(rand.Reader, cert, cert, pub, priv)
	if err != nil {
		fmt.Println(err)
		return
	}
	certPath := "cert.cer"
	fmt.Println("out: ", certPath)
	ioutil.WriteFile(certPath, certBin, 0777)

	privPath := "private.key"
	privBin := x509.MarshalPKCS1PrivateKey(priv)
	fmt.Println("out: ", privPath)
	ioutil.WriteFile(privPath, privBin, 0777)
}
