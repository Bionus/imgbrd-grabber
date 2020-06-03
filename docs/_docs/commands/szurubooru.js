// Usage:
// node szurubooru.js "username" "token" "tag1 tag2" "safe" "http://source" "path/to/file.jpg"

const http = require("http");
const fs = require("fs");
const FormData = require("form-data");

// Szurubooru doesn't use the same ratings as most boorus so we need to map them
const ratingsMap = {
    "safe": "safe",
    "questionable": "sketchy",
    "explicit": "unsafe",
};

// Get parameters
const argv = process.argv.slice(2);
const username = argv.shift();
const token = argv.shift();

// Actual data to send to the server
const data = {
    "tags": argv[0].split(" "),
    "safety": ratingsMap[argv[1]],
    "source": argv[2] || undefined,
};
const headers = {
    "Authorization": "Token " + Buffer.from(username + ":" + token).toString("base64"),
    "Accept": "application/json",
};

// Create a multipart form data request
const form = new FormData();
form.append("content", fs.createReadStream(argv[3]), "test.tmp");
form.append("metadata", JSON.stringify(data));

// Request settings
// See https://github.com/rr-/szurubooru/blob/master/doc/API.md#creating-post
const opts = {
    hostname: "localhost",
    port: 8080,
    path: "/api/posts/",
    method: "POST",
    headers: { ...headers, ...form.getHeaders() },
};

// Send the request
const req = http.request(opts, (res) => {
    if (res.statusCode < 200 || res.statusCode >= 300) {
        console.log("Error");
        res.on('data', (d) => {
            process.stdout.write(d);
        });
    } else {
        console.log("OK");
    }
}).on('error', function(e) {
    console.error(e);
});
form.pipe(req);
