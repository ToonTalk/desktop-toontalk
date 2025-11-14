#!/bin/bash
# Windows-compatible test server using Node.js
cd "$(dirname "$0")"
echo "🌐 Starting test server at http://localhost:8000"
echo "📄 Open http://localhost:8000/test.html in your browser"
echo ""

# Use Node.js http-server if available, otherwise use Python
if command -v npx &> /dev/null; then
    npx http-server -p 8000
elif command -v python3 &> /dev/null; then
    python3 -m http.server 8000
elif command -v python &> /dev/null; then
    python -m http.server 8000
else
    # Use Node.js that comes with Emscripten
    ~/emsdk/node/*/bin/node -e "
const http = require('http');
const fs = require('fs');
const path = require('path');

const server = http.createServer((req, res) => {
    const filePath = '.' + (req.url === '/' ? '/test.html' : req.url);
    const extname = path.extname(filePath);
    const contentType = {
        '.html': 'text/html',
        '.js': 'application/javascript',
        '.wasm': 'application/wasm',
        '.css': 'text/css'
    }[extname] || 'application/octet-stream';

    fs.readFile(filePath, (err, content) => {
        if (err) {
            res.writeHead(404);
            res.end('Not found');
        } else {
            res.writeHead(200, { 'Content-Type': contentType });
            res.end(content);
        }
    });
});

server.listen(8000);
console.log('Server running at http://localhost:8000/');
"
fi
