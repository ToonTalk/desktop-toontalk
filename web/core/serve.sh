#!/bin/bash
# Simple HTTP server to test WASM
cd "$(dirname "$0")"
echo "🌐 Starting test server at http://localhost:8000"
echo "📄 Open http://localhost:8000/test.html in your browser"
echo ""
python3 -m http.server 8000
