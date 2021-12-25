const fs = require("fs");
const crypto = require("crypto");
const cp = require("child_process");

const SITES_DIR = "src/sites";

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

const output = {
    name: "Official Grabber sources",
    home: "https://github.com/Bionus/imgbrd-grabber",
    url: "https://raw.githubusercontent.com/Bionus/imgbrd-grabber/master/src/sites/",
    sources: [],
};

const sources = fs
    .readdirSync(SITES_DIR)
    .filter((f) => fs.statSync(SITES_DIR + "/" + f).isDirectory());
for (const source of sources) {
    const modelFile = mkPath(source, "model.ts");
    if (!fs.existsSync(modelFile)) {
        continue;
    }

    const contents = fs.readFileSync(modelFile, "utf-8");
    const version = /version:\s*"([^"]+)"/.exec(contents);
    const defaultSites = parseSites(mkPath(source, "sites.txt"));
    const supportedSites = parseSites(mkPath(source, "supported.txt"));
    const lastCommit = getLastCommit(modelFile);

    output.sources.push({
        name: source,
        version: version ? version[1] : undefined,
        hash: crypto.createHash("sha256").update(fs.readFileSync(mkPath(source, "model.js"))).digest("hex"),
        lastCommit,
        defaultSites,
        supportedSites: supportedSites.length ? supportedSites : defaultSites,
    });
}

console.log(JSON.stringify(output));