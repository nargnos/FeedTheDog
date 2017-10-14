package common

import (
	"github.com/golang/glog"
)

func WarningPanic() {
	if err := recover(); err != nil {
		glog.Warning(err)
	}
}
func PanicIfErr(err error) {
	if err != nil {
		panic(err)
	}
}

// TODO: 全局TODO，先用go实现，因为好写一些，之后替换为cpp
