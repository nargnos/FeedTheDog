package Utils

import (
	"errors"
	"io"
	"net"
)

var ErrSizeTooLarge = errors.New("too large")

type ConnReader struct {
	R    io.Reader
	Conn net.Conn
}

func (r *ConnReader) Read(buf []byte) (int, error) {
	return r.R.Read(buf)

}
func (r *ConnReader) Close() error {
	return r.Conn.Close()
}

type ConnWriter struct {
	W    io.Writer
	Conn net.Conn
}

func (w *ConnWriter) Close() error {
	return w.Conn.Close()
}
func (w *ConnWriter) Write(buf []byte) (int, error) {
	return w.W.Write(buf)

}
func Trans(r *ConnReader, w *ConnWriter, buf []byte) {
	for {
		reads, err := r.Read(buf)
		if err != nil {
			// log.Println(err)
			w.Close()
			return
		}
		_, err = w.Write(buf[:reads])

		if err != nil {
			// log.Println(err)
			r.Close()
			return
		}
	}
}

func CheckError(err error) {
	if err != nil {
		panic(err)
	}
}
