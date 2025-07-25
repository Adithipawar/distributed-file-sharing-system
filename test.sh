#!/bin/bash

SERVER="http://localhost:8080"
FILENAME="testfile.txt"
SHARED_USER="alice"

echo "🔹 Creating test file..."
echo "This is a test file." > $FILENAME

echo "🔹 Uploading file..."
curl -s -X POST -F "file=@$FILENAME" $SERVER/upload
echo -e "\n✅ Upload complete"

echo "🔹 Downloading file..."
curl -s "$SERVER/download?filename=$FILENAME" -o downloaded_$FILENAME
echo "✅ Download complete: downloaded_$FILENAME"

echo "🔹 Sharing file with user '$SHARED_USER'..."
curl -s -X POST "$SERVER/share?filename=$FILENAME&shared_with=$SHARED_USER"
echo -e "\n✅ Share complete"

echo "🔹 Getting files shared with '$SHARED_USER'..."
curl -s "$SERVER/shared-with?username=$SHARED_USER"
echo -e "\n✅ Listed shared files"

echo "🧹 Cleaning up test file"
rm -f $FILENAME downloaded_$FILENAME
