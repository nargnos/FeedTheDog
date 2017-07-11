package Config

import "time"

const DefaultServerKey = "123456"

// 编译时编进去，固定
const Salt = "hello world"
const MagicNumber = 10101010101

const ServerName = "(ง •_•)ง"

const BufferSize = 204800

var TimeoutSecond = 5 * time.Second
