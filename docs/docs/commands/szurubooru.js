// Usage: ORDER IS DIFFERENT FROM ORIGINAL szurubooru.js!!! - Update line 35 with your sites url/domain prefixed with /api.  
// node /full/path/to/grabber/szurubooru.js "username" "token" "%path:nobackslash%" "%all:includenamespace,unsafe,underscores%" "%rating%" "%source:raw%"

const axios = require("axios");
const fs = require("fs");
const FormData = require("form-data");

async function getTag(name) { try { const res = await axios.get(`/tag/${encodeURIComponent(name)}`); return res.data; } catch (e) { if (e.response.status === 404) { return null; } throw e; } }
async function createTag(name, category) { try { const data = { names: [name], category }; await axios.post("/tags", data); console.log(`Tag "${name}" created`); } catch (e) { console.error("Error creating tag: " + e.message); console.error(e.response.data); } }
async function updateTag(name, version, category) { try { const data = { version, category }; await axios.put(`/tag/${encodeURIComponent(name)}`, data); console.log(`Tag "${name}" updated`); } catch (e) { console.error("Error updating tag: " + e.message); console.error(e.response.data); } }
async function setTagCategory(name, category) { const tag = await getTag(name); if (tag === null) { await createTag(name, category); } else if (tag.category !== category) { await updateTag(name, tag.version, category); } }

(async () => {
    // Ratings map updated to include szurubooru default ratings, and anime-pictures default 'unknown' safety, which defaults to 'safe'
    const ratingsMap = {
        "": "safe",
        "g": "safe",
        "general": "safe",
        "safe": "safe",
        "s": "sketchy",
        "sensitive": "sketchy",
        "q": "sketchy",
        "questionable": "sketchy",
        "sketchy": "sketchy",
        "e": "unsafe",
        "explicit": "unsafe",
        "unsafe": "unsafe",
        "unknown": "safe", 
    };

    const argv = process.argv.slice(2);
    const username = argv.shift();
    const token = argv.shift();

    axios.defaults.baseURL = "https://your_booru.tld/api";
    axios.defaults.headers.common["Authorization"] = "Token " + Buffer.from(username + ":" + token).toString("base64");
    axios.defaults.headers.common["Accept"] = "application/json";

    const filePath = argv[0];
    const rawTags = argv[1] || "";
    const rating = argv[2] || "safe";
    const source = argv[3] || undefined;

    const tags = rawTags.split(" ");
    for (let i = 0; i < tags.length; ++i) { if (!tags[i]) continue; const parts = tags[i].split(":"); const category = parts.shift(); const name = parts.join(":"); void setTagCategory(name, category); tags[i] = name; }

    const data = {
        tags,
        safety: ratingsMap[rating.toLowerCase()],
        source: source,
    };

    const form = new FormData();
    form.append("content", fs.createReadStream(filePath), "image.tmp");
    form.append("metadata", JSON.stringify(data));

    try {
        const config = { headers: form.getHeaders(), maxContentLength: 999999999999, maxBodyLength: 999999999999 }
        await axios.post("/posts/", form, config);
        console.log("Post created successfully!");
    } catch (e) {
        console.error("Error creating post: " + e.message);
        if (e.response) { console.error(e.response.data); }
        process.exit(1);
    }
})();
