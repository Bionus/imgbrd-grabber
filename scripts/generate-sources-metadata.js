const fs = require("fs");
const crypto = require("crypto");
const cp = require("child_process");

const SITES_DIR = "src/sites";
const DISABLED_SOURCES = ["Sankaku", "Tumblr"];

function mkPath(source, filename) {
    return SITES_DIR + "/" + source + "/" + filename;
}

function parseSites(path) {
    if (!fs.existsSync(path)) {
        return [];
    }
    return fs.readFileSync(path, 'utf-8')
        .split('\n')
        .map((line) => line.trim())
        .filter((line) => line.length > 0);
}

function getLastCommit(path) {
    const cmd = `git log -n 1 --pretty=format:%H/%an/%aI/%s -- "${path}"`;
    const out = cp.execSync(cmd).toString();
    if (!out) {
        return undefined;
    }

    const parts = out.split("/");
    return {
        hash: parts[0],
        author: parts[1],
        date: parts[2],
        subject: parts[3],
    };
}

const args = process.argv.slice(2);
const branch = args.length > 0 ? args[0] : "master";
const isNightly = branch === "develop";
const output = {
    name: "Official Grabber sources" + (isNightly ? " (nightly)" : ""),
    home: "https://github.com/Bionus/imgbrd-grabber",
    url: "https://github.com/Bionus/imgbrd-grabber/releases/download/sources-" + branch + "/",
    sources: [],
};

const sources = fs
    .readdirSync(SITES_DIR)
    .filter((f) => fs.statSync(SITES_DIR + "/" + f).isDirectory());
for (const source of sources) {
    // Skip disabled sources
    if (DISABLED_SOURCES.includes(source)) {
        continue;
    }

    // Skip directories without a "model.ts" file
    const modelFile = mkPath(source, "model.ts");
    if (!fs.existsSync(modelFile)) {
        continue;
    }

    const contents = fs.readFileSync(modelFile, "utf-8");
    const name = /name:\s*"([^"]+)"/.exec(contents);
    const version = /version:\s*"([^"]+)"/.exec(contents);
    const defaultSites = parseSites(mkPath(source, "sites.txt"));
    const supportedSites = parseSites(mkPath(source, "supported.txt"));
    const lastCommit = getLastCommit(modelFile);

    output.sources.push({
        slug: source.replace(/[^a-z0-9_.-]+/gi, ".").replace(/^\.+|\.+$/g, ""),
        name: name ? name[1] : source,
        version: version ? version[1] : undefined,
        hash: crypto.createHash("sha256").update(fs.readFileSync(mkPath(source, "model.js"), "utf-8")).digest("hex"),
        lastCommit,
        defaultSites,
        supportedSites: supportedSites.length ? supportedSites : defaultSites,
    });
}

console.log(JSON.stringify(output));