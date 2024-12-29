## メモ

- docker関連 
  - docker compose up --build 
  - docker compose run app /bin/bash 
- ビルド 
  - cmake -DCMAKE_BUILD_TYPE=Debug -B build -DCMAKE_PREFIX_PATH="$(pkg-config --variable=prefix gstreamer-1.0)"
  - make
  - gdb ./bin/meetingSDKDemo
- オーディオデバイスの初期化 
  - sudo alsa force-reload 
  - pulseaudio --kill 
  - pulseaudio --start

## 参考URL

- https://developers.zoom.us/docs/meeting-sdk/linux/add-features/raw-data/
- https://devforum.zoom.us/t/meeting-sdk-sending-audio-raw-data/100571
- ビデオ送信: https://devforum.zoom.us/t/syncing-send-video-and-send-audio-in-meeting-sdk/110080/2

## JWTトークン生成

```
import jwt
import datetime

# キーとシークレット
key = "xxx"
secret = "xxx"

# 現在の時刻（発行時間）と24時間後の有効期限を設定
iat = datetime.datetime.utcnow()
exp = iat + datetime.timedelta(hours=24)

# JWTのペイロードを設定して署名（iatとexpをUNIXタイムに変換）
payload = {
    "appKey": key,
    "iat": int(iat.timestamp()),  # UNIXタイムスタンプ
    "exp": int(exp.timestamp()),  # UNIXタイムスタンプ
    "tokenExp": int(exp.timestamp())  # UNIXタイムスタンプ
}

# JWTの生成
token = jwt.encode(payload, secret, algorithm="HS256")

# Python 3.6以上の場合、jwt.encodeはbytesを返すので、文字列に変換
if isinstance(token, bytes):
    token = token.decode("utf-8")

token
```