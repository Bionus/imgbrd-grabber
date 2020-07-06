import { Parser } from "xml2js";

export function search(api: IApi, query: string, page: number = 1) {
    return api.search.url(
        { search: query, page },
        { limit: 10, loggedIn: false, baseUrl: "/" },
        undefined
    );
}

export function gallery(api: IApi, id: string) {
    return api.gallery!.url(
        { id, md5: "", page: 1 },
        { limit: 10, loggedIn: false, baseUrl: "/" },
    );
}

function GrabberRegexMatches(regex: string, txt: string): any[] {
    const rx = new RegExp(regex, "g");
    const ret = [];
    let val: any;
    while ((val = rx.exec(txt)) !== null) {
        ret.push({ ...val, ...val.groups });
    }
    return ret;
}

function syncXml2jsParseString(parser: any, xml: string): any {
    var error = null;
    var json = null;
    parser.parseString(xml, (innerError: any, innerJson: any) => {
        error = innerError;
        json = innerJson;
    });
    if (error) {
        throw error;
    }
    if (!error && !json) {
        throw new Error('The callback was suddenly async or something.');
    }
    return json;
}

function GrabberParseXML(txt: string) {
    const parser = new Parser({
        attrkey: "@attributes",
        charkey: "#text",
        explicitCharkey: true,
        explicitArray: false,
    });
    try {
        return syncXml2jsParseString(parser, txt);
    } catch (e) {
        return null;
    }
}

export function makeGrabber() {
    (global as any).Grabber = {
        htmlDecode(txt: string) { return txt },
        regexMatches: GrabberRegexMatches,
        parseXML: GrabberParseXML,
    };

    require("./helper");
}
