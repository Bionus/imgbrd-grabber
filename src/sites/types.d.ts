/**
 * Describes a tag.
 */
interface ITag {
    /**
     * The tag's ID on this source.
     */
    id?: number;

    /**
     * The tag name.
     */
    name: string;

    /**
     * The number of images having this tag on this source.
     */
    count?: number;

    /**
     * The tag type as string.
     *
     * @example "copyright"
     */
    type?: string;

    /**
     * The tag type ID.
     *
     * It must either be mapped in "tag-types.txt", or loaded by {@link IApi#tagTypes}
     */
    typeId?: number;
}

/**
 * Describes a tag type.
 *
 * At tag type is a category for tags, such as "general", "copyright", "character", "meta", etc.
 */
interface ITagType {
    /**
     * The tag type ID.
     *
     * Useful for mapping a type ID to a useful type down the road.
     */
    id: number;

    /**
     * The tag type name.
     *
     * @example "copyright"
     */
    name: string;
}

/**
 * The identity of a media object. It should be unique for per media per source.
 *
 * On most sources, that would be an ID and/or an MD5, but sometimes a token or generic key is also needed.
 */
type IImageIdentity = Record<string, string | number | boolean>;

/**
 * Describes a media object. It can be an image, a video, or a gallery.
 */
interface IImage {
    // Known "meaningful" tokens
    type?: "image" | "gallery";
    md5?: string;
    author?: string;
    name?: string;
    status?: string;
    id?: number | string;
    score?: number;
    parent_id?: number | string;
    author_id?: number | string;
    has_children?: boolean;
    has_note?: boolean;
    has_comments?: boolean;
    sources?: string[];
    source?: string;
    position?: number;
    gallery_count?: number;
    page_url?: string;
    rating?: string;
    tags?: string[] | ITag[];
    ext?: string;
    created_at?: string;
    date?: string; // same as created_at, but only accepts ISO dates

    // Full size
    file_url?: string;
    width?: number;
    height?: number;
    file_size?: number;
    rect?: string;

    // Sample
    sample_url?: string;
    sample_width?: number;
    sample_height?: number;
    sample_file_size?: number;
    sample_rect?: string;

    // Thumbnail
    preview_url?: string;
    preview_width?: number;
    preview_height?: number;
    preview_file_size?: number;
    preview_rect?: string;

    /**
     * The identity of this media object. It should be unique for this media on that source.
     */
    identity?: IImageIdentity;

    /**
     * The endpoint to call to load more information about this media.
     */
    details_endpoint?: {
        /**
         * The key of the endpoint in the "endpoints" object of this API.
         */
        endpoint: string;

        /**
         * The inputs to pass to the endpoint to have it load details about this media.
         */
        input: Record<string, any>;
    }

    /**
     * The media files. At least one is required.
     * It's preferred to have one of each type, otherwise it will be inferred.
     */
    medias?: {
        /**
         * The URL to download the media file at.
         */
        url: string;

        /**
         * The "size" type of this media file. If not provided, it will be inferred from the dimensions.
         *
         * * full: the original sized media
         * * sample: a big version of the media, better sized for slower connections
         * * preview: a preview version of the media, used as a thumbnail
         */
        type?: "full" | "sample" | "preview";

        /**
         * The width (in pixels) of this media file.
         */
        width?: number;

        /**
         * The height (in pixels) of this media file.
         */
        height?: number;

        /**
         * For videos, the bitrate (in bits per second) of this file.
         * Useful to differentiate multiple videos when they have the same dimensions.
         */
        bitrate?: number;

        /**
         * The file size (in bytes) of this media file.
         */
        file_size?: number;

        /**
         * If this media is just a smaller part of the file, the rectangle to cut the media before showing it.
         * Necessary sometimes for example in situations where all thumbnails are concatenated into a single file to save bandwidth.
         */
        rect?: {
            x: number;
            y: number;
            width: number;
            height: number;
        };
    }[];

    // Additional raw tokens to pass to the filename
    tokens?: {
        [key: string]: any;
    };
}
interface IPool {
    id?: number;
    name: string;
    previous?: number;
    current?: number;
    next?: number;
}

/**
 * Object representing an HTTP request.
 *
 * Alternative to returning a simple string URL when we also want to return HTTP headers or use POST requests.
 */
interface IRequestBase {
    /**
     * The URL to load.
     */
    url: string;

    /**
     * Additional headers to pass to the query.
     */
    headers?: { [key: string]: string };
}

interface IRequestGet extends IRequestBase {
    /**
     * The HTTP verb to use for this request. GET or POST.
     */
    method?: "GET";
}

interface IRequestPost extends IRequestBase {
    /**
     * The HTTP verb to use for this request. GET or POST.
     */
    method: "POST";

    /**
     * The data to send with the POST request.
     * If passing an object, it will be converted to JSON and the Content-Type header automatically set to "application/json".
     */
    data?: any | string;
}

type IRequest = IRequestGet | IRequestPost;

/**
 * An object reprensenting an error.
 */
interface IError {
    error: string;
}
interface IParsedSearch {
    images: IImage[];
    tags?: ITag[] | string[];
    wiki?: string;
    pageCount?: number;
    imageCount?: number;
    urlNextPage?: string;
    urlPrevPage?: string;
}
interface IParsedTagTypes {
    types: ITagType[];
}
interface IParsedTags {
    tags: ITag[] | string[];
}
interface IParsedDetails {
    pools?: IPool[];
    tags?: ITag[] | string[];
    imageUrl?: string;
    createdAt?: string;
    sources?: string[];
    source?: string;
}
interface IParsedGallery extends IParsedSearch {}

type IAuthField = IAuthNormalField | IAuthConstField | IAuthHashField;

/**
 * Parent type for authentication fields.
 */
interface IAuthFieldBase {
    /**
     * The name of the field, as it will be passed in the request.
     */
    key?: string;
}

interface IAuthNormalField extends IAuthFieldBase {
    /**
     * The ID of the field as stored in the settings.
     *
     * Values are shared between authentications if they have the same ID, even if the key is different.
     * Standard IDs allow proper translation and naming in the UI, but they can be any other value.
     */
    id: "pseudo" | "userId" | "password" | "salt" | "apiKey" | "consumerKey" | "consumerSecret" | "accessToken" | "refreshToken" | string;

    /**
     * The type of field. For example, a "password" field will have its input hidden to the user in the UI.
     */
    type?: "text" | "password" | "salt";

    /**
     * The default value of the field if the user does not provide one.
     */
    def?: string;
}

/**
 * A field whose value never changes.
 *
 * For example for some forms that have a constant "hidden" field.
 */
interface IAuthConstField extends IAuthFieldBase {
    type: "const";

    /**
     * The value of the field. Never changes.
     */
    value: string;
}

/**
 * A field for which the value provided by the user will be hashed before sending.
 */
interface IAuthHashField extends IAuthFieldBase {
    type: "hash";

    /**
     * Which hash method to use.
     */
    hash: "md5" | "sha1";

    /**
     * The salt to use. "%value%" will be replaced by the field's value.
     *
     * @example "my-dope-hash--%value%--"
     */
    salt: string;
}

type IAuthCheck = IAuthCheckCookie | IAuthCheckRedirect | IAuthCheckMaxPage;

/**
 * Checks whether the authentication worked by looking for the presence of a cookie.
 */
interface IAuthCheckCookie {
    type: "cookie";

    /**
     * The name of the cookie to look for after login.
     */
    key: string;
}

/**
 * Checks whether the authentication worked by checking the redirect URL.
 */
interface IAuthCheckRedirect {
    type: "redirect";

    /**
     * Part of the destination URL after login to check.
     */
    url: string;
}

/**
 * Checks whether the authentication worked by trying to load a faraway page.
 */
interface IAuthCheckMaxPage {
    type: "max_page";

    /**
     * The page number to try to load. It should work for logged-in users but not anonymous users.
     */
    value: number;
}

/**
 * Authentication type that can be used on this source.
 */
type IAuth = IBasicAuth | IOauth2Auth | IOauth1Auth | IHttpAuth | IHttpBasicAuth;

/**
 * OAuth 2 authentication.
 *
 * @see {@link https://oauth.net/1/}
 */
interface IOauth2Auth {
    type: "oauth2";
    authType: "password" | "password_json" | "client_credentials" | "refresh_token"  | "pkce" | "authorization_code";

    /**
     * Where should the client authentication parameters should be passed (default: body).
     *
     * * header: they will be passed via HTTP Basic authentication
     * * body: they will be put in the request body
     */
    clientAuthentication?: "header" | "body";

    requestUrl?: string;
    tokenUrl?: string;
    refreshTokenUrl?: string;
    authorizationUrl?: string;
    redirectUrl?: string;
    urlProtocol?: string;
    scopes?: string[];
}

/**
 * OAuth 1 authentication.
 *
 * @see {@link https://oauth.net/2/}
 */
interface IOauth1Auth {
    type: "oauth1";
    temporaryCredentialsUrl: string,
    authorizationUrl: string,
    tokenCredentialsUrl: string,

    /**
     * The signature method for the requests.
     *
     * RSA-SHA1 is not supported.
     *
     * @see {@link https://datatracker.ietf.org/doc/html/rfc5849#section-3.4}
     */
    signatureMethod?: "hmac-sha1" | "plaintext",
}

/**
 * In-URL authentication. Given fields will be passed as URL parameters to all requests.
 */
interface IBasicAuth {
    type: "url";

    /**
     * Definition of fields to pass as URL parameters to all requests.
     */
    fields: IAuthField[];

    /**
     * Method to check for successful authentication.
     */
    check?: IAuthCheck;
}

interface IParsedCsrf {
    /**
     * Additional fields to pass to the auth request.
     */
    fields: { [key: string]: string | number };

    /**
     * Override the URL to call in the main login flow.
     */
    url?: string;
}

/**
 * Authentication by an HTTP call to a given URL (for example for login forms).
 */
interface IHttpAuth {
    /**
     * The HTTP method to use for the call, either GET or POST (usually POST).
     */
    type: "get" | "post";

    /**
     * The URL to call to log in.
     */
    url: string;

    /**
     * Definition of fields to pass to the request.
     */
    fields: IAuthField[];

    /**
     * Additional HTTP headers to pass to the login request.
     */
    headers?: Record<string, string>;

    /**
     * Optional URL to load to get any CSRF token from the form.
     */
    csrf?: {
        /**
         * The URL to load.
         */
        url: string;

        /**
         * Field names to parse in the results.
         */
        fields: string[];
    },

    /**
     * Method to check for successful authentication.
     */
    check?: IAuthCheck;
}

/**
 * HTTP Basic authentication.
 *
 * @see {@link https://en.wikipedia.org/wiki/Basic_access_authentication}
 */
interface IHttpBasicAuth {
    type: "http_basic";

    /**
     * Whether the value to use for HTTP Basic authentication is the user's raw password or its API key.
     */
    passwordType?: "password" | "apiKey";

    /**
     * The token type to pass in the Authorization header. Defaults to "Basic".
     */
    tokenType?: string;

    /**
     * Method to check for successful authentication.
     */
    check?: IAuthCheck;
}

/**
 * Describes how tags are formatted.
 *
 * For example, "test_tag" could also be written as "Test Tag" or "Test-tag".
 */
interface ITagFormat {
    /**
     * The casing to use for each word.
     *
     * * lower: all lowercase (ex: "test tag")
     * * upper_first: the first letter of the first word uppercase (ex: "Test tag")
     * * upper: the first letter of each word uppercase (ex: "Test Tag")
     * * caps: all uppercase (ex: "TEST TAG")
     */
    case: "lower" | "upper_first" | "upper" | "caps";

    /**
     * Which character to use between each word.
     *
     * Usually either a space or underscore.
     */
    wordSeparator: string;
}

/**
 * Describes how searches are formatted.
 *
 * For example, "tag1 ~tag2 ~tag3" could also be written as "tag1 AND (tag2 OR tag3)".
 */
type SearchFormat = ISearchFormatBasic | ISearchFormatFull;

/**
 * Basic search format where only the "AND" operator is supported.
 */
interface ISearchFormatBasic {
    /**
     * The format to use for the "AND" operator.
     *
     * When searching two tags using this format, both tags must be in the image for the image to be returned.
     * Usually, either a space or sometimes directly " AND ".
     */
    and: ISearchFormatType | string;
}

/**
 * Full search format where all options are customizable.
 */
interface ISearchFormatFull extends ISearchFormatBasic {
    /**
     * The format to use for the "OR" operator.
     *
     * When searching two tags using this format, either one of the tags (or both) must be in the image for the image to be returned.
     * Usually, either a pipe ("|") or sometimes directly " OR ".
     */
    or: ISearchFormatType | string;

    /**
     * Whether this source supports parentheses in search.
     */
    parenthesis: boolean;

    /**
     * Which operator has precedence (priority) above the other.
     *
     * Depending on it, "A and B or C" can be mapped to either "(A and B) or C" ("AND" has precedence),  or "A and (B or C)" ("OR" has precedence).
     * Usually, the "and" operator has precedence.
     */
    precedence: "and" | "or";
}

/**
 * Describes how a search format operator works.
 */
interface ISearchFormatType {
    /**
     * The separator between the operands of this operation.
     *
     * For example, if separator is " &&& ", then ("tag1", "tag2") will be mapped to "tag1 &&& tag2".
     */
    separator: string;

    /**
     * Optional prefix before every operand of this operation.
     *
     * For example, if the separator is " " and the prefix is "~", then ("tag1", "tag2") will be mapped to "~tag1 ~tag2".
     */
    prefix?: string;
}

type MetaField = IMetaFieldOptions | IMetaTypeInput | IMetaTypeBoolean;
interface IMetaFieldBase {
    label?: string;
    default?: any;
}
interface IMetaFieldOptions extends IMetaFieldBase {
    type: "options";
    options: Array<{
        label?: string;
        value: string;
    }> | string[];
}
interface IMetaTypeInput extends IMetaFieldBase  {
    type: "input";
    parser?: (value: string) => any;
}
interface IMetaTypeBoolean extends IMetaFieldBase  {
    type: "bool";
    default?: boolean;
}

type IParsedSearchQuery = ITag | IParsedSearchOperator;
interface IParsedSearchOperator {
    operator: "or" | "and";
    left: IParsedSearchQuery;
    right: IParsedSearchQuery;
}

/**
 * Information about a user's query during a basic search.
 */
interface ISearchQuery {
    /**
     * What the user typed in the search field, as-is.
     */
    search: string;

    /**
     * If "parseInput" is enabled, the parsed search query, based on the tag database.
     *
     * @see {IApi#search#parseInput}
     */
    parsedSearch?: IParsedSearchQuery;

    /**
     * The page number of this query.
     *
     * @deprecated Use `opts.page` instead
     */
    page: number;
}

/**
 * Information about a user's query when loading a gallery.
 */
interface IGalleryQuery {
    /**
     * The gallery's ID.
     */
    id: string;

    /**
     * The gallery's MD5.
     */
    md5: string;

    /**
     * The gallery's identity.
     */
    identity?: IImageIdentity;

    /**
     * The page number of this query.
     *
     * @deprecated Use `opts.page` instead.
     */
    page: number;
}

/**
 * Information about a user's query when loading tags.
 */
interface ITagsQuery {
    /**
     * The page number of this query.
     *
     * @deprecated Use `opts.page` instead.
     */
    page: number;

    /**
     * How to sort the results.
     */
    order: "count" | "date" | "name";
}

/**
 * Base interface for search query additional information.
 */
interface IUrlOptionsBase {
    /**
     * The current site's base URL.
     */
    baseUrl: string;

    /**
     * Whether the user is currently successfully logged in to this source.
     */
    loggedIn: boolean;
}

/**
 * Additional information for a query.
 */
interface IUrlOptions extends IUrlOptionsBase {
    /**
     * The page number of this query.
     */
    page: number;

    /**
     * The number of results per page to return.
     */
    limit: number;
}

interface IParsedUgoiraDetails {
    /**
     * The list of all frames contained in this ugoira file, sorted in order.
     */
    frames: {
        /**
         * The file for that frame. If empty, will take the next frame from the ZIP file alphabetically.
         */
        file?: string;

        /**
         * The duration for which this frame should be shown.
         */
        delay: number;
    }[];
}

/**
 * Additional information for a details query.
 */
interface IUrlDetailsOptions extends IUrlOptionsBase {}

/**
 * Information about the previous search the user did.
 *
 * Useful for example when the user clicks "next" to know what was the previous page's highest ID.
 */
interface IPreviousSearch {
    /**
     * The page number.
     */
    page: number;

    /**
     * The smallest ID in the results, minus one.
     */
    minIdM1: string;

    /**
     * The smallest ID in the results.
     */
    minId: string;

    /**
     * The smallest date in the results.
     */
    minDate: string;

    /**
     * The biggest ID in the results.
     */
    maxId: string;

    /**
     * The biggest ID in the results, plus one.
     */
    maxIdP1: string;

    /**
     * The biggest date in the results.
     */
    maxDate: string;
}

/**
 * A specific search mode on a source API.
 */
interface IEndpoint<T extends string> {
    /**
     * The name to show to the end user in the interface.
     * If left empty, it will not be shown in the list.
     */
    name?: string;

    /**
     * The user input to provide to this endpoint.
     */
    input: Record<T, MetaField>;

    /**
     * The ID of the authentication methods required to access this endpoint, or "true" for any method.
     */
    auth?: string[] | boolean;

    /**
     * Whether to still pass HTTP errors to the parse function.
     */
    parseErrors?: boolean;

    /**
     * The function that will generate the URL to load based on the user input.
     */
    url: (query: Record<T, any>, opts: IUrlOptions, previous: IPreviousSearch | undefined) => IRequest | IError | string;

    /**
     * The function that will parse the response of the URL above.
     */
    parse: (src: string, statusCode: number) => IParsedSearch | IParsedUgoiraDetails | IError;
}

/**
 * A source's API, such as JSON or HTML.
 */
interface IApi {
    /**
     * User-friendly name of the API.
     */
    name: "JSON" | "XML" | "RSS" | "Regex";

    /**
     * The ID of the authentication methods required to access this API.
     */
    auth: string[];

    /**
     * The maximum value of "images per page" for this API.
     */
    maxLimit?: number;

    /**
     * @see {@link ISource#forcedTokens}
     */
    forcedTokens?: string[];

    /**
     * The value of "images per page" that is always used for this API.
     *
     * Useful to calculate the number of pages from the total number of images, for example.
     */
    forcedLimit?: number;

    /**
     * Describes the endpoint for basic search.
     */
    search: {
        /**
         * Whether to parse the user input using the tag database.
         *
         * @see {@link ISearchQuery#parsedSearch}
         */
        parseInput?: boolean;

        /**
         * Whether to still pass HTTP errors to the parse function.
         */
        parseErrors?: boolean;

        url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined) => IRequest | IError | string;
        parse: (src: string, statusCode: number) => IParsedSearch | IError;
    };

    /**
     * Additional endpoints can be added as needed.
     */
    endpoints?: Record<string, IEndpoint<any>>;

    /**
     * Describes the endpoint for loading a single image's details.
     */
    details?: {
        parseErrors?: boolean;
        fullResults?: boolean;
        url: (id: string, md5: string, opts: IUrlDetailsOptions, identity?: IImageIdentity) => IRequest | IError | string;
        parse: (src: string, statusCode: number) => IParsedDetails | IImage | IError;
    };

    /**
     * Describes the endpoint for loading a gallery's details.
     */
    gallery?: {
        parseErrors?: boolean;
        url: (query: IGalleryQuery, opts: IUrlOptions) => IRequest | IError | string;
        parse: (src: string, statusCode: number) => IParsedGallery | IError;
    };

    /**
     * Describes the endpoint for loading the source's tag types.
     */
    tagTypes?: {
        parseErrors?: boolean;
        url: () => IRequest | IError | string;
        parse: (src: string, statusCode: number) => IParsedTagTypes | IError;
    } | false;

    /**
     * Describes the endpoint for loading the source's tags.
     */
    tags?: {
        parseErrors?: boolean;
        url: (query: ITagsQuery, opts: IUrlOptions) => IRequest | IError | string;
        parse: (src: string, statusCode: number) => IParsedTags | IError;
    };

    /**
     * Describes the endpoint to check whether an arbitrary website matches this source.
     */
    check?: {
        parseErrors?: boolean;
        url: () => IRequest | IError | string;
        parse: (src: string, statusCode: number) => boolean | IError;
    };
}

/**
 * Main type to describe a Grabber source.
 */
interface ISource {
    /**
     * The name of the source.
     *
     * @example "My super source"
     */
    name: string;

    /**
     * Modifiers that can be used to refine search.
     *
     * Used for example in auto-complete or search syntax highlighting.
     *
     * @example ["date:", "order:score", "order:rank"]
     */
    modifiers?: string[];

    /**
     * Tokens returned by this source for the images.
     *
     * Used for example for filename validation.
     *
     * @example ["tweet_id", "original_date"]
     */
    tokens?: string[];

    /**
     * Tokens that must always trigger a "details" load.
     *
     * Use "*" to always do it.
     *
     * @example ["filename"]
     */
    forcedTokens?: string[];

    /**
     * The tag format used by the source. Used to map from a single "universal" type to each source's preferred type.
     */
    tagFormat?: ITagFormat;

    /**
     * The search format used by the source. Used to map from a single "universal" type to each source's preferred type.
     */
    searchFormat?: SearchFormat;

    /**
     * Meta fields that can be used to search.
     *
     * Used to provide user-friendly interface for the user to input search options via dropdowns, checkboxes, etc.
     */
    meta?: { [id: string]: MetaField };

    /**
     * Possible authentication methods for this source.
     *
     * The key in this mapping is later used for the "auth" field in the IApi object.
     *
     * @see {@link IApi#auth}
     */
    auth?: { [id: string]: IAuth };

    /**
     * Possible APIs for this source.
     *
     * For example, if the source can be browsed via both a JSON API and an HTML-parsed website, that's two "APIs".
     */
    apis: { [id: string]: IApi };
}

interface IFile {
    url: string;
    width?: number;
    height?: number;
    filesize?: number;
    ext?: string;
}

interface IDownloadable {
    tokens?: {
        [key: string]: any;
    };
    files: IFile[];
}

interface IDownloader {
    name: string;
    handlers: Array<{
        regexes: string[];
        url: (url: string) => IRequest | IError | string;
        parse: (src: string, statusCode: number) => IDownloadable | IError;
    }>;
}

declare const Grabber: any;
