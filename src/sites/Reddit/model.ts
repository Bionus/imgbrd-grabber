interface IRedditSearch {
	tags: string[];
	subreddit?: string;
	user?: string;
	sort?: string;
	since?: string;
	/** When true, do not force media-oriented Reddit operators (self:no). */
	raw?: boolean;
}

/**
 * Booru-style meta tokens that are meaningless on Reddit and poison multi-site
 * searches (e.g. rating:safe, order:score, score:>10) with junk matches.
 */
// Note: order:/sort: handled separately — not in this meta strip list
const BOORU_META_RE = /^(rating|score|width|height|filesize|filetype|status|parent|child|source|date|age|id|md5|pixiv_id|has|is|pool|ordfav|fav|approver|commenter|noter|noteupdater|artcomm|gen|copy|char|meta|limit|page):/i;

/** Map multi-site order:/sort: freestyle values onto Reddit sort= query. */
function mapRedditSort(val: string): string {
	const v = val.toLowerCase();
	const map: { [key: string]: string } = {
		"hot": "hot",
		"new": "new",
		"newest": "new",
		"recent": "new",
		"date": "new",
		"id_desc": "new",
		"id": "new",
		"descending": "new",
		"desc": "new",
		"ascending": "new",
		"asc": "new",
		"top": "top",
		"score": "top",
		"score_desc": "top",
		"popular": "top",
		"best": "top",
		"rank": "top",
		"rising": "rising",
		"relevance": "relevance",
		"comments": "comments",
	};
	return map[v] || v;
}

const IMAGE_EXT_RE = /\.(jpe?g|png|gif|webp|bmp|mp4|webm|gifv)(\?|#|$)/i;
const IMAGE_HOST_RE = /(i\.redd\.it|preview\.redd\.it|external-preview\.redd\.it|i\.imgur\.com|imgur\.com\/[a-z0-9]+|redgifs\.com|gfycat\.com|media\.tumblr\.com|pbs\.twimg\.com)/i;

function parseSearch(search: string): IRedditSearch {
	const ret: IRedditSearch = { tags: [] };
	const parts = search.split(" ").map((p) => p.trim()).filter((p) => p.length > 0);
	for (const part of parts) {
		if (part.indexOf("subreddit:") === 0) {
			ret.subreddit = part.substr(10);
		} else if (part.indexOf("user:") === 0 || part.indexOf("author:") === 0) {
			ret.user = part.indexOf("user:") === 0 ? part.substr(5) : part.substr(7);
		} else if (part.indexOf("sort:") === 0) {
			ret.sort = mapRedditSort(part.substr(5));
		} else if (part.indexOf("order:") === 0) {
			// Multi-site: order:id_desc / order:newest / order:descending → Reddit sort
			ret.sort = mapRedditSort(part.substr(6));
		} else if (part.indexOf("since:") === 0) {
			ret.since = part.substr(6);
		} else if (part === "raw:true" || part === "raw:1") {
			ret.raw = true;
		} else if (BOORU_META_RE.test(part) || (part.charAt(0) === "-" && BOORU_META_RE.test(part.substr(1)))) {
			// Drop booru meta — would otherwise become free-text and pull irrelevant posts
			continue;
		} else {
			// Booru tags use underscores; Reddit search is closer to natural language
			ret.tags.push(part.replace(/_/g, " "));
		}
	}
	return ret;
}

function makeArgs(args: { [key: string]: any }): string {
	let ret = "";
	for (const key in args) {
		if (args[key] !== undefined && args[key] !== null && args[key] !== "") {
			ret += (ret.length === 0 ? "?" : "&") + key + "=" + encodeURIComponent(args[key]);
		}
	}
	return ret;
}

function decodeRedditUrl(url: string | undefined): string | undefined {
	if (!url) {
		return undefined;
	}
	return url.replace(/&amp;/g, "&");
}

function looksLikeMediaUrl(url: string | undefined): boolean {
	if (!url) {
		return false;
	}
	const u = url.toLowerCase();
	return IMAGE_EXT_RE.test(u) || IMAGE_HOST_RE.test(u);
}

/** Keep only posts that actually have downloadable / viewable media. */
function hasMedia(raw: any): boolean {
	if (raw.is_gallery === true) {
		return true;
	}
	if (raw.is_video === true && raw.secure_media?.reddit_video) {
		return true;
	}
	if (raw.post_hint === "image" || raw.post_hint === "hosted:video" || raw.post_hint === "rich:video") {
		return true;
	}
	if (raw.preview?.images?.length > 0) {
		return true;
	}
	if (looksLikeMediaUrl(raw.url)) {
		return true;
	}
	// Crossposts sometimes only expose media on the parent
	const cross = raw.crosspost_parent_list && raw.crosspost_parent_list[0];
	if (cross && hasMedia(cross)) {
		return true;
	}
	return false;
}

const map = {
	"md5": "id",
	"author": "author",
	"name": "title",
	"source": "url",
	"file_url": "url",
	"created_at": "created_utc",
	"preview_url": "thumbnail",
	"preview_width": "thumbnail_width",
	"preview_height": "thumbnail_height",
	"score": "score",
};

function applyPreview(img: IImage, raw: any): void {
	if (raw.preview?.images?.[0]?.source) {
		img.sample_url = decodeRedditUrl(raw.preview.images[0].source.url);
		img.sample_width = raw.preview.images[0].source.width;
		img.sample_height = raw.preview.images[0].source.height;
	}

	if (img.preview_url === "spoiler" || img.preview_url === "default" || img.preview_url === "nsfw" || img.preview_url === "self" || img.preview_url === "image") {
		const resolutions = raw.preview?.images?.[0]?.resolutions;
		const preview = resolutions && resolutions.length > 0 ? resolutions[0] : undefined;
		if (preview) {
			img.preview_url = decodeRedditUrl(preview.url);
			img.preview_width = preview.width;
			img.preview_height = preview.height;
		} else {
			img.preview_url = undefined;
		}
	}
	if (!img.preview_url && img.sample_url) {
		img.preview_url = img.sample_url;
	}
}

function applyGalleryFirstFrame(img: IImage, raw: any): void {
	if (raw.is_gallery !== true || !raw.gallery_data?.items?.length || !raw.media_metadata) {
		return;
	}
	const firstId = raw.gallery_data.items[0].media_id;
	const media = raw.media_metadata[firstId];
	if (!media || !("s" in media)) {
		return;
	}
	// Prefer full image URL from gallery metadata over the /gallery/ page link
	const full = decodeRedditUrl(media.s.u || media.s.gif || media.s.mp4);
	if (full) {
		img.file_url = full;
		img.width = media.s.x;
		img.height = media.s.y;
	}
	if (media.p && media.p.length > 0) {
		const p = media.p[Math.min(1, media.p.length - 1)];
		img.preview_url = decodeRedditUrl(p.u);
		img.preview_width = p.x;
		img.preview_height = p.y;
	}
}

function parseImage(child: any): IImage | null {
	// Ignore non-link posts
	if (child.kind !== "t3") { // 1=comment, 2=account, 3=link, 4=message, 5=subreddit, 6=award
		return null;
	}

	let raw = child.data;

	// Prefer media from crosspost parent when the child is a thin wrapper
	if (!hasMedia(raw) && raw.crosspost_parent_list && raw.crosspost_parent_list[0] && hasMedia(raw.crosspost_parent_list[0])) {
		raw = raw.crosspost_parent_list[0];
	}

	// Ignore text-only / non-media posts
	if (raw.thumbnail === "self" && !raw.is_gallery && !raw.is_video) {
		return null;
	}
	if (!hasMedia(raw)) {
		return null;
	}

	const img: IImage = Grabber.mapFields(raw, map);
	img.file_url = decodeRedditUrl(img.file_url);
	img.source = decodeRedditUrl(img.source as string | undefined);
	img.page_url = raw.permalink ? "https://www.reddit.com" + raw.permalink : undefined;

	// Useful free-form tags for filtering / filenames
	const tags: string[] = [];
	if (raw.subreddit) {
		tags.push("subreddit:" + raw.subreddit);
		tags.push(raw.subreddit);
	}
	if (raw.link_flair_text) {
		tags.push(String(raw.link_flair_text).replace(/\s+/g, "_"));
	}
	if (raw.over_18) {
		tags.push("nsfw");
	}
	if (tags.length > 0) {
		img.tags = tags;
	}

	// Galleries
	if (raw.is_gallery === true) {
		img.type = "gallery";
		if (raw.gallery_data?.items) {
			img.gallery_count = raw.gallery_data.items.length;
		}
		applyGalleryFirstFrame(img, raw);
	}

	applyPreview(img, raw);

	// Videos
	if (raw.secure_media?.oembed) {
		img.width = raw.secure_media.oembed.width;
		img.height = raw.secure_media.oembed.height;
	}
	if (raw.is_video && raw.secure_media?.reddit_video) {
		// FIXME: should use the HD playlist URL when Grabber supports m3u8
		img.file_url = decodeRedditUrl(raw.secure_media.reddit_video.fallback_url);
		img.width = raw.secure_media.reddit_video.width;
		img.height = raw.secure_media.reddit_video.height;
		img.ext = "mp4";
	}

	// Rating
	img.rating = raw.over_18 ? "explicit" : "safe";

	// Final gate: need at least one usable URL
	if (!img.file_url && !img.sample_url && !img.preview_url) {
		return null;
	}
	// Prefer sample as file when file_url is a non-media reddit page (gallery already fixed above)
	if (img.file_url && !looksLikeMediaUrl(img.file_url) && !raw.is_video && raw.is_gallery !== true && img.sample_url) {
		img.file_url = img.sample_url;
	}

	return img;
}

function buildSearchQuery(search: IRedditSearch): string {
	const parts = search.tags.slice();
	if (!search.raw) {
		// Prefer link/media posts over text discussions
		if (parts.indexOf("self:no") < 0 && parts.indexOf("self:yes") < 0) {
			parts.push("self:no");
		}
	}
	return parts.join(" ").trim();
}

export const source: ISource = {
	name: "Reddit",
	modifiers: [
		"subreddit:", "user:", "author:",
		"sort:hot", "sort:new", "sort:top", "sort:rising", "sort:relevance", "sort:comments",
		"order:newest", "order:recent", "order:descending", "order:desc", "order:id_desc", "order:date",
		"order:popular", "order:score", "order:top", "order:hot", "order:rising",
		"since:hour", "since:day", "since:week", "since:month", "since:year", "since:all",
		"raw:true",
	],
	apis: {
		json: {
			name: "JSON",
			auth: [],
			maxLimit: 100,
			search: {
				parseErrors: true,
				url: (query: ISearchQuery, opts: IUrlOptions): string | IError => {
					const search = parseSearch(query.search);
					const q = buildSearchQuery(search);

					// /user/<name>/submitted — was parsed but never wired before
					if (search.user && search.tags.length === 0 && !search.subreddit) {
						const args = {
							sort: search.sort || "new",
							t: search.since || "all",
							limit: opts.limit,
							raw_json: 1,
							type: "links",
						};
						return "/user/" + encodeURIComponent(search.user) + "/submitted.json" + makeArgs(args);
					}

					const prefix = search.subreddit
						? "/r/" + encodeURIComponent(search.subreddit)
						: (search.user ? "/user/" + encodeURIComponent(search.user) : "");

					if (q.length > 0) {
						const args = {
							q,
							// Relevance ranks keyword matches; hot/top without q dumps the front page
							sort: search.sort || "relevance",
							t: search.since || "all",
							restrict_sr: search.subreddit ? 1 : undefined,
							limit: opts.limit,
							raw_json: 1,
							type: "link",
							include_over_18: "on",
						};
						const base = prefix || "";
						return base + "/search.json" + makeArgs(args);
					}

					// No tags: listing endpoint (hot/new/top) for a subreddit or front page
					const args = {
						t: search.since || "all",
						limit: opts.limit,
						raw_json: 1,
					};
					const listing = search.sort && search.sort !== "relevance" ? "/" + search.sort : "";
					return (prefix || "") + listing + ".json" + makeArgs(args);
				},
				parse: (src: string): IParsedSearch | IError => {
					const data = JSON.parse(src);
					if (data.error || data.message) {
						return { error: String(data.message || data.error) };
					}
					if (data.kind !== "Listing") {
						return { error: "No listing found in response" };
					}

					// BUGFIX: previously assigned images = data.data.children (raw posts),
					// then pushed parsed images onto the same array → double junk + non-images.
					const images: IImage[] = [];
					for (const child of data.data.children) {
						const img = parseImage(child);
						if (img !== null) {
							images.push(img);
						}
					}

					return {
						images,
						// Reddit uses "after" cursors; expose hasNext via residual children
						imageCount: images.length,
					};
				},
			},
			gallery: {
				url: (query: IGalleryQuery): string => {
					return "/" + query.md5 + ".json?raw_json=1";
				},
				parse: (src: string): IParsedGallery | IError => {
					const data = JSON.parse(src)[0];
					if (data.kind !== "Listing") {
						return { error: "No listing found in response" };
					}

					const child = data.data.children[0];
					const gallery = parseImage(child);
					if (!gallery) {
						return { error: "Could not parse gallery" };
					}
					delete gallery.type;
					delete gallery.gallery_count;

					const images: IImage[] = [];
					const items = child.data.gallery_data && child.data.gallery_data.items;
					if (!items) {
						return { error: "No gallery items" };
					}
					for (const galleryItem of items) {
						const media = child.data.media_metadata[galleryItem.media_id];
						const img = JSON.parse(JSON.stringify(gallery));

						if (!media || !("s" in media)) {
							continue;
						}
						img.file_url = decodeRedditUrl(media.s.u || media.s.gif || media.s.mp4);
						img.width = media.s.x;
						img.height = media.s.y;

						if (!("p" in media) || !media.p || media.p.length === 0) {
							images.push(img);
							continue;
						}
						const preview = media.p.length > 1 ? media.p[1] : media.p[0];
						img.preview_url = decodeRedditUrl(preview.u);
						img.preview_width = preview.x;
						img.preview_height = preview.y;

						images.push(img);
					}

					return {
						images,
						imageCount: images.length,
					};
				},
			},
		},
	},
};
