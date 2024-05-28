// Usage:
// node szurubooru.js "username" "token" "tagType1:tag1 tagType2:tag2" "safe" "http://source" "path/to/file.jpg"

const axios = require("axios");
const fs = require("fs");
const FormData = require("form-data");

async function getTag(name) {
    try {
        const res = await axios.get(`/tag/${encodeURIComponent(name)}`);
        return res.data;
    } catch (e) {
        if (e.response.status === 404) {
            return null;
        }
        throw e;
    }
}
async function createTag(name, category) {
    try {
        const data = { names: [name], category };
        await axios.post("/tags", data);
        console.log(`Tag "${name}" created`);
    } catch (e) {
        console.error("Error creating tag: " + e.message);
        console.error(e.response.data);
    }
}
async function updateTag(name, version, category) {
    try {
        const data = { version, category };
        await axios.put(`/tag/${encodeURIComponent(name)}`, data);
        console.log(`Tag "${name}" updated`);
    } catch (e) {
        console.error("Error updating tag: " + e.message);
        console.error(e.response.data);
    }
}
async function setTagCategory(name, category) {
    const tag = await getTag(name);
    if (tag === null) {
        await createTag(name, category);
    } else if (tag.category !== category) {
        await updateTag(name, tag.version, category);
    }
}

(async () => {
    // Szurubooru doesn't use the same ratings as most boorus, so we need to map them
    const ratingsMap = {
        "": "safe",
        "g": "safe",
        "general": "safe",
        "safe": "safe",
        "s": "sketchy",
        "sensitive": "sketchy",
        "q": "sketchy",
        "questionable": "sketchy",
        "e": "unsafe",
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

    // Parse tags and update categories
    const tags = argv[0].split(" ");
    for (let i = 0; i < tags.length; ++i) {
        const parts = tags[i].split(":");
        const category = parts.shift();
        const name = parts.join(":");
        void setTagCategory(name, category);
        tags[i] = name;
    }

    // Actual data to send to the server
    const data = {
        tags,
        safety: ratingsMap[argv[1]],
        source: argv[2] || undefined,
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
        process.exit(1);
    }
})();
