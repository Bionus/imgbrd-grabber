// Usage:
// node szurubooru.js "username" "token" "tag1 tag2" "safe" "http://source" "path/to/file.jpg"

const axios = require("axios");
const fs = require("fs");
const FormData = require("form-data");

(async () => {
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

    // Axios settings
    axios.defaults.baseURL = "http://localhost:8080/api";
    axios.defaults.headers.common["Authorization"] = "Token " + Buffer.from(username + ":" + token).toString("base64");
    axios.defaults.headers.common["Accept"] = "application/json";

    // Actual data to send to the server
    const data = {
        "tags": argv[0].split(" "),
        "safety": ratingsMap[argv[1]],
        "source": argv[2] || undefined,
    };

    // Create a multipart form data request
    const form = new FormData();
    form.append("content", fs.createReadStream(argv[3]), "test.tmp");
    form.append("metadata", JSON.stringify(data));

    // Create image post
    try {
        const config = {
            headers: form.getHeaders(),
            maxContentLength: 999999999999,
            maxBodyLength: 999999999999,
        }
        await axios.post("/posts/", form, config);
    } catch (e) {
        console.error("Error creating post: " + e.message);
        console.error(e.response.data);
    }
})();
