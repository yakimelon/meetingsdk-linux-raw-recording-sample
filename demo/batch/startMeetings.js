require('dotenv').config();
const { exec } = require('child_process');
const jwt = require('jsonwebtoken');
const path = require('path');

// 環境変数
const APP_KEY = process.env.APP_KEY;
const APP_SECRET = process.env.APP_SECRET;

if (!APP_KEY || !APP_SECRET) {
  console.error("❌ APP_KEY または APP_SECRET が .env に設定されていません");
  process.exit(1);
}

// モックAPI（例）
function fetchMockMeetings() {
  return {
    meetings: [
      { meeting_number: "123456789", zak: "abcde" },
      { meeting_number: "987654321", zak: "vwxyz" }
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

// 実行
(async () => {
  const data = fetchMockMeetings();

  for (const meeting of data.meetings) {
    const { meeting_number, zak } = meeting;
    const token = generateToken(APP_KEY, APP_SECRET);

    const execPath = path.join(__dirname, '../bin/meetingSDKDemo');
    const cmd = `${execPath} "${meeting_number}" "${zak}" "${token}"`;

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
  }
})();
