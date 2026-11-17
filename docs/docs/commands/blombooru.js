// Usage:
// node blombooru.js "YOUR_API_KEY" "%path:nobackslash%" "%all:includenamespace,unsafe,underscores%" "%rating%" "%source:raw%"

// Config:
const BASE_URL = "http://localhost:8000";

const axios = require("axios");
const fs = require("fs");
const FormData = require("form-data");

// Blombooru only has these five tag categories, anything else defaults to "general"
const CATEGORY_MAP = {
    "general": "general",
    "artist": "artist",
    "character": "character",
    "copyright": "copyright",
    "series": "copyright",
    "meta": "meta",
};

// Blombooru only has three ratings, anything in between is collapsed into "questionable"
const RATING_MAP = {
    "": "safe",
    "s": "safe",
    "safe": "safe",
    "g": "safe",
    "general": "safe",
    "q": "questionable",
    "questionable": "questionable",
    "sensitive": "questionable",
    "sketchy": "questionable",
    "e": "explicit",
    "explicit": "explicit",
    "unsafe": "explicit",
};

function parseTags(rawTags) {
    const tags = [];
    const categoryHints = {};

    for (const token of rawTags.split(" ").filter(Boolean)) {
        const parts = token.split(":");
        let category = "general";
        let name = token;

        if (parts.length > 1) {
            category = parts.shift().toLowerCase();
            name = parts.join(":");
        }

        name = name.trim().toLowerCase();
        if (!name) continue;

        tags.push(name);
        categoryHints[name] = CATEGORY_MAP[category] || "general";
    }

    return { tags, categoryHints };
}

async function createPost(filePath, tags, categoryHints, rating, source) {
    try {
        const form = new FormData();
        form.append("file", fs.createReadStream(filePath));
        form.append("rating", rating);
        form.append("tags", tags.join(" "));
        form.append("category_hints", JSON.stringify(categoryHints));
        if (source) {
            form.append("source", source);
        }

        const config = {
            headers: form.getHeaders(),
            maxContentLength: 999999999999,
            maxBodyLength: 999999999999,
        }
        const response = await axios.post("/media/", form, config);
        console.log(`Post created successfully! ID: ${response.data.id}`);
    } catch (e) {
        console.error("Error creating post: " + e.message);
        if (e.response) {
            // A 409 just means the file was already uploaded before (duplicate hash)
            console.error(e.response.status, e.response.data);
        }
        process.exit(1);
    }
}

(async () => {
    const argv = process.argv.slice(2);
    const apiKey = argv.shift();
    const filePath = argv[0];
    const rawTags = argv[1] || "";
    const rawRating = (argv[2] || "safe").toLowerCase();
    const source = argv[3] || undefined;

    axios.defaults.baseURL = BASE_URL + "/api";
    axios.defaults.headers.common["Authorization"] = "Bearer " + apiKey;
    axios.defaults.headers.common["Accept"] = "application/json";
    axios.defaults.headers.common["Cookie"] = "admin_mode=true";

    const { tags, categoryHints } = parseTags(rawTags);
    const rating = RATING_MAP[rawRating] || "safe";

    await createPost(filePath, tags, categoryHints, rating, source);
})();
