interface ITag {
    id?: number;
    name: string;
    count?: number;
    type?: string;
    typeId?: number;
}
interface IImage {
    file_url: string;
    [key: string]: any;
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
    key: string;
}
interface IAuthNormalField extends IAuthFieldBase {
    type: "username" | "password";
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

type IAuth = IBasicAuth | IOauth2Auth | IHttpAuth;
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

interface IApi {
    name: string;
    auth: string[];
    maxLimit?: number;
    forcedLimit?: number;
    search: {
        parseErrors?: boolean;
        url: (query: any, opts: any, previous: any) => IUrl | IError | string;
        parse: (src: string, statusCode: number) => IParsedSearch | IError;
    };
    details?: {
        parseErrors?: boolean;
        url: (id: number, md5: string) => IUrl | IError | string;
        parse: (src: string, statusCode: number) => IParsedDetails | IError;
    };
    gallery?: {
        parseErrors?: boolean;
        url: (query: any, opts: any) => IUrl | IError | string;
        parse: (src: string, statusCode: number) => IParsedGallery | IError;
    };
    tags?: {
        parseErrors?: boolean;
        url: (query: any, opts: any) => IUrl | IError | string;
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
    forcedTokens?: string[];
    tagFormat?: ITagFormat;
    searchFormat?: SearchFormat;
    auth?: { [id: string]: IAuth };
    apis: { [id: string]: IApi };
}

declare const Grabber: any;