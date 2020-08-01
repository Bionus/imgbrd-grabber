interface ITag {
    id?: number;
    name: string;
    count?: number;
    type?: string;
    typeId?: number;
}
interface ITagType {
    id: number;
    name: string;
}
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
    date?: string;

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

interface IUrl {
    url: string;
    headers?: { [key: string]: string };
}

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
}
interface IParsedGallery extends IParsedSearch {}

type IAuthField = IAuthNormalField | IAuthConstField | IAuthHashField;
interface IAuthFieldBase {
    key?: string;
}
interface IAuthNormalField extends IAuthFieldBase {
    id: string;
    type?: "text" | "password" | "salt";
    def?: string;
}
interface IAuthConstField extends IAuthFieldBase {
    type: "const";
    value: string;
}
interface IAuthHashField extends IAuthFieldBase {
    type: "hash";
    hash: "md5" | "sha1";
    salt: string;
}

type IAuthCheck = IAuthCheckCookie | IAuthCheckMaxPage;
interface IAuthCheckCookie {
    type: "cookie";
    key: string;
}
interface IAuthCheckMaxPage {
    type: "max_page";
    value: number;
}

type IAuth = IBasicAuth | IOauth2Auth | IHttpAuth | IHttpBasicAuth;
interface IOauth2Auth {
    type: "oauth2";
    authType: "password" | "client_credentials" | "header_basic";
    requestUrl?: string;
    tokenUrl?: string;
    refreshTokenUrl?: string;
    scope?: string[];
}
interface IBasicAuth {
    type: "url";
    fields: IAuthField[];
    check?: IAuthCheck;
}
interface IHttpAuth {
    type: "get" | "post";
    url: string;
    fields: IAuthField[];
    check?: IAuthCheck;
}
interface IHttpBasicAuth {
    type: "http_basic";
    passwordType?: "password" | "apiKey";
    check?: IAuthCheck;
}

interface ITagFormat {
    case: "lower" | "upper_first" | "upper" | "caps";
    wordSeparator: string;
}

type SearchFormat = ISearchFormatBasic | ISearchFormatFull;
interface ISearchFormatBasic {
    and: ISearchFormatType | string;
}
interface ISearchFormatFull extends ISearchFormatBasic {
    or: ISearchFormatType | string;
    parenthesis: boolean;
    precedence: "and" | "or";
}
interface ISearchFormatType {
    separator: string;
    prefix?: string;
}

interface ISearchQuery {
    search: string;
    page: number;
}
interface IGalleryQuery {
    id: string;
    md5: string;
    page: number;
}
interface ITagsQuery {
    page: number;
    order: "count" | "date" | "name";
}

interface IUrlOptions {
    limit: number;
    baseUrl: string;
    loggedIn: boolean;
}

interface IPreviousSearch {
    page: number;
    minIdM1: string;
    minId: string;
    minDate: string;
    maxId: string;
    maxIdP1: string;
    maxDate: string;
}

interface IApi {
    name: string;
    auth: string[];
    maxLimit?: number;
    forcedLimit?: number;
    search: {
        parseErrors?: boolean;
        url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined) => IUrl | IError | string;
        parse: (src: string, statusCode: number) => IParsedSearch | IError;
    };
    details?: {
        parseErrors?: boolean;
        url: (id: string, md5: string) => IUrl | IError | string;
        parse: (src: string, statusCode: number) => IParsedDetails | IError;
    };
    gallery?: {
        parseErrors?: boolean;
        url: (query: IGalleryQuery, opts: IUrlOptions) => IUrl | IError | string;
        parse: (src: string, statusCode: number) => IParsedGallery | IError;
    };
    tagTypes?: {
        parseErrors?: boolean;
        url: () => IUrl | IError | string;
        parse: (src: string, statusCode: number) => IParsedTagTypes | IError;
    } | false;
    tags?: {
        parseErrors?: boolean;
        url: (query: ITagsQuery, opts: IUrlOptions) => IUrl | IError | string;
        parse: (src: string, statusCode: number) => IParsedTags | IError;
    };
    check?: {
        parseErrors?: boolean;
        url: () => IUrl | IError | string;
        parse: (src: string, statusCode: number) => boolean | IError;
    };
}
interface ISource {
    name: string;
    modifiers?: string[];
    tokens?: string[];
    forcedTokens?: string[];
    tagFormat?: ITagFormat;
    searchFormat?: SearchFormat;
    auth?: { [id: string]: IAuth };
    apis: { [id: string]: IApi };
}

declare const Grabber: any;