require('dotenv').config();
const { exec } = require('child_process');
const jwt = require('jsonwebtoken');
const path = require('path');
const fs = require('fs');
const https = require('https');
const { v4: uuidv4 } = require('uuid');

// 環境変数
const APP_KEY = process.env.APP_KEY;
const APP_SECRET = process.env.APP_SECRET;

if (!APP_KEY || !APP_SECRET) {
  console.error("❌ APP_KEY または APP_SECRET が .env に設定されていません");
  process.exit(1);
}

// モックAPI（動画URLも含む）
function fetchMockMeetings() {
  return {
    meetings: [
      {
        meeting_number: "123456789",
        zak: "abcde",
        url: "https://storage.googleapis.com/staging.webinar-salesbranding.appspot.com/movies/SAMPLE01.mp4"
      },
      {
        meeting_number: "987654321",
        zak: "vwxyz",
        url: "https://storage.googleapis.com/staging.webinar-salesbranding.appspot.com/movies/SAMPLE02.mp4"
      }
    ]
  };
}

// JWT生成
function generateToken(appKey, secret) {
  const iat = Math.floor(Date.now() / 1000);
  const exp = iat + 60 * 60 * 24;

  const payload = {
    appKey,
    iat,
    exp,
    tokenExp: exp
  };

  return jwt.sign(payload, secret, { algorithm: 'HS256' });
}

// ファイルをダウンロードしてローカルに保存
function downloadFile(url, outputPath) {
  return new Promise((resolve, reject) => {
    const file = fs.createWriteStream(outputPath);
    https.get(url, (response) => {
      if (response.statusCode !== 200) {
        return reject(`Download failed: ${url}`);
      }
      response.pipe(file);
      file.on('finish', () => {
        file.close();
        resolve(outputPath);
      });
    }).on('error', (err) => {
      fs.unlink(outputPath, () => {});
      reject(err);
    });
  });
}

// 実行
(async () => {
  const data = fetchMockMeetings();

  for (const meeting of data.meetings) {
    const { meeting_number, zak, url } = meeting;
    const token = generateToken(APP_KEY, APP_SECRET);

    // ダウンロード先パス
    const downloadDir = path.join(__dirname, 'downloads');
    if (!fs.existsSync(downloadDir)) fs.mkdirSync(downloadDir);
    const filePath = path.join(downloadDir, `${uuidv4()}.mp4`);

    try {
      console.log(`⬇️ ダウンロード開始: ${url}`);
      await downloadFile(url, filePath);
      console.log(`✅ ダウンロード完了: ${filePath}`);

      // 実行ファイルパス
      const execPath = path.join(__dirname, '../bin/meetingSDKDemo');

      // 実行コマンド（第4引数にファイルパス）
      const cmd = `${execPath} "${meeting_number}" "${zak}" "${token}" "${filePath}"`;

      console.log(`▶️ 実行: ${cmd}`);
      exec(cmd, (error, stdout, stderr) => {
        if (error) {
          console.error(`❌ 実行エラー: ${error.message}`);
          return;
        }
        if (stderr) {
          console.error(`⚠️ stderr: ${stderr}`);
        }
        console.log(`✅ stdout:\n${stdout}`);
      });
    } catch (err) {
      console.error(`❌ 処理失敗 (${meeting_number}):`, err);
    }
  }
})();
