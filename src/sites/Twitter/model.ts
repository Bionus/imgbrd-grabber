function getExtension(url: string): string {
	const index = url.lastIndexOf(".");
	if (index >= 0 && index > url.length - 10) {
		return url.substr(index + 1);
	}
	return "";
}

function parseTweetMedia(sc: any, original: any, media: any): IImage {
	const d: IImage = {} as any;
	const sizes = media["sizes"];

	// Meta-data
	d.id = original["id_str"];
	d.author = sc["user"]["screen_name"];
	d.author_id = sc["user"]["id_str"];
	d.created_at = sc["created_at"];
	d.tags = sc["entities"]["hashtags"].map((hashtag: any) => hashtag["text"]);

	// Images
	d.width = sizes["large"]["w"];
	d.height = sizes["large"]["h"];
	if ("thumb" in sizes) {
		d.preview_url = media["media_url_https"] + ":thumb";
		d.preview_width = sizes["thumb"]["w"];
		d.preview_height = sizes["thumb"]["h"];
	}
	if ("medium" in sizes) {
		d.sample_url = media["media_url_https"] + ":medium";
		d.sample_width = sizes["medium"]["w"];
		d.sample_height = sizes["medium"]["h"];
	}

	// Full-size link
	if ("video_info" in media) {
		let maxBitrate = -1;
		for (const variantInfo of media["video_info"]["variants"]) {
			const bitrate = variantInfo["bitrate"];
			if (bitrate > maxBitrate) {
				maxBitrate = bitrate;
				d.file_url = variantInfo["url"];
				d.ext = getExtension(variantInfo["url"]);
			}
		}
	} else {
		d.file_url = media["media_url_https"] + ":orig";
		d.ext = getExtension(media["media_url_https"]);
	}

	// Additional tokens
	d.tokens = {};
	d.tokens["tweet_id"] = sc["id_str"];
	d.tokens["original_tweet_id"] = original["id_str"];
	d.tokens["original_author"] = original["user"]["screen_name"];
	d.tokens["original_author_id"] = original["user"]["id_str"];
	d.tokens["original_date"] = "date:" + original["created_at"];

	return d;
}

function parseTweet(sc: any, gallery: boolean): IImage[] | IImage | null {
	const original = sc;
	if ("retweeted_status" in sc) {
		sc = sc["retweeted_status"];
	}
	if (!("extended_entities" in sc)) {
		return null; // text-only — skip instead of empty stub rows
	}

	const entities = sc["extended_entities"];
	if (!("media" in entities)) {
		return null;
	}

	const medias: any[] = entities["media"];
	if (!medias || medias.length === 0) {
		return null;
	}

	if (medias.length > 1) {
		if (gallery) {
			return medias.map((media: any) => parseTweetMedia(sc, original, media));
		}

		const d = parseTweetMedia(sc, original, medias[0]);
		d.type = "gallery";
		d.gallery_count = medias.length;
		d.id = original["id_str"];
		return d;
	}

	return parseTweetMedia(sc, original, medias[0]);
}

/**
 * Twitter (user_timeline) only supports a screen name / user id — not booru tags.
 * Strip meta, treat remaining single token as @user, multi-token as search (v1.1).
 */
function parseTwitterSearch(raw: string): {
	screenName?: string,
	userId?: string,
	listId?: string,
	query?: string,
	retweets: boolean,
	replies: boolean,
	resultType: "recent" | "popular" | "mixed",
} {
	const meta: ISource["meta"] = {
		user_id: { type: "input" },
		list_id: { type: "input" },
		retweets: { type: "bool", default: true },
		replies: { type: "bool", default: true },
	};

	// Drop booru meta before parseSearchQuery so "rating:safe user stuff" doesn't become username
	const stripped = Grabber.stripBooruMetaTags(raw, { keepOrder: true });
	const orderRaw = stripped.order || "";
	const cleaned = Grabber.normalizeOrderTags(
		(stripped.query + (orderRaw ? " order:" + orderRaw : "")).trim(),
		"raw",
	);

	// Pull order / sort into result_type for search API
	let resultType: "recent" | "popular" | "mixed" = "recent";
	const tokens = cleaned.split(/\s+/).filter((t: string) => t.length > 0);
	const kept: string[] = [];
	for (const t of tokens) {
		const lower = t.toLowerCase();
		if (lower.indexOf("order:") === 0 || lower.indexOf("sort:") === 0) {
			const v = t.substr(t.indexOf(":") + 1).toLowerCase();
			if (v === "popular" || v === "score" || v === "best" || v === "rank" || v === "mixed") {
				resultType = v === "mixed" ? "mixed" : "popular";
			} else {
				// newest / recent / desc / id_desc / date → recent
				resultType = "recent";
			}
			continue;
		}
		if (lower.indexOf("result:") === 0) {
			const v = t.substr(7).toLowerCase();
			if (v === "popular" || v === "mixed" || v === "recent") {
				resultType = v as any;
			}
			continue;
		}
		kept.push(t);
	}

	const joined = kept.join(" ");
	const parsed = Grabber.parseSearchQuery(joined, meta);

	// user:foo or @foo
	let screenName = (parsed.query || "").trim();
	if (screenName.charAt(0) === "@") {
		screenName = screenName.substr(1);
	}
	// multi-word free text → full search query (hashtags / words)
	const isSingleUser = screenName.length > 0 && screenName.indexOf(" ") < 0 && !screenName.match(/[#:]/);

	return {
		screenName: isSingleUser ? screenName : undefined,
		userId: parsed.user_id,
		listId: parsed.list_id,
		query: isSingleUser ? undefined : (parsed.query || undefined),
		retweets: parsed.retweets !== false,
		replies: parsed.replies !== false,
		resultType,
	};
}

const meta: ISource["meta"] = {
	user_id: {
		type: "input",
	},
	list_id: {
		type: "input",
	},
	retweets: {
		type: "bool",
		default: true,
	},
	replies: {
		type: "bool",
		default: true,
	},
};

export const source: ISource = {
	name: "Twitter",
	modifiers: [
		"retweets:yes", "retweets:no", "replies:yes", "replies:no",
		"order:newest", "order:recent", "order:descending", "order:desc", "order:date", "order:id_desc",
		"order:popular", "order:score", "order:best",
		"result:recent", "result:popular", "result:mixed",
		"user_id:", "list_id:",
	],
	tokens: ["tweet_id", "original_tweet_id", "original_author", "original_author_id", "original_date"],
	meta,
	auth: {
		oauth2: {
			type: "oauth2",
			authType: "client_credentials",
			clientAuthentication: "header",
			tokenUrl: "/oauth2/token",
		},
	},
	apis: {
		json: {
			name: "JSON",
			auth: ["oauth2"],
			maxLimit: 200,
			search: {
				url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
					try {
						const search = parseTwitterSearch(query.search);
						const pageUrl = Grabber.pageUrl(query.page, previous, 1, "", "&since_id={max}", "&max_id={min-1}");
						const commonParams = [
							"count=" + opts.limit,
							"include_rts=" + (search.retweets ? "true" : "false"),
							"tweet_mode=extended",
						];

						// List lookup
						if (search.listId) {
							const params = [
								...commonParams,
								"list_id=" + search.listId,
							];
							return "/1.1/lists/statuses.json?" + params.join("&") + pageUrl;
						}

						// Free-text / hashtag search (media only)
						if (search.query) {
							const q = search.query + " filter:media -filter:retweets";
							const params = [
								"q=" + encodeURIComponent(q),
								"count=" + opts.limit,
								"result_type=" + search.resultType,
								"include_entities=true",
								"tweet_mode=extended",
							];
							return "/1.1/search/tweets.json?" + params.join("&") + pageUrl;
						}

						// User timeline
						if (!search.screenName && !search.userId) {
							return { error: "Twitter needs a @username or free-text query (booru meta-only tags were stripped)" };
						}
						const params = [
							...commonParams,
							"exclude_replies=" + (!search.replies ? "true" : "false"),
							search.userId
								? "user_id=" + search.userId
								: "screen_name=" + encodeURIComponent(search.screenName as string),
						];
						return "/1.1/statuses/user_timeline.json?" + params.join("&") + pageUrl;
					} catch (e: any) {
						return { error: e.message };
					}
				},
				parse: (src: string): IParsedSearch | IError => {
					const data = JSON.parse(src);
					if (data.errors) {
						return { error: data.errors.map((e: any) => e.message).join("; ") };
					}

					// search/tweets wraps statuses; user_timeline is a bare array
					const list = Array.isArray(data) ? data : (data.statuses || []);
					const images: IImage[] = [];
					for (const i in list) {
						const img = parseTweet(list[i], false);
						if (img) {
							images.push(img as IImage);
						}
					}

					return { images };
				},
			},
			gallery: {
				url: (query: IGalleryQuery, opts: IUrlOptions): string => {
					return "/1.1/statuses/show.json?id=" + query.id + "&tweet_mode=extended";
				},
				parse: (src: string): IParsedGallery => {
					const data = JSON.parse(src);
					const images = Grabber.makeArray(parseTweet(data, true)).filter((x: any) => x);
					return {
						images,
						imageCount: images.length,
						pageCount: 1,
					};
				},
			},
		},
	},
};
