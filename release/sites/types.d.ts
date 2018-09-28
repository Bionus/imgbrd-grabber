type Iterable<T> = T[] | { [k: number]: T } | { [k: string]: T };

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
    images: Iterable<IImage>;
    tags?: Iterable<ITag>;
    wiki?: string;
    pageCount?: number;
    imageCount?: number;
    urlNextPage?: string;
    urlPrevPage?: string;
}
interface IParsedTags {
    tags: Iterable<ITag>;
}
interface IParsedDetails {
    pools?: Iterable<IPool>;
    tags?: Iterable<ITag>;
    imageUrl?: string;
    createdAt?: string;
}
interface IParsedGallery extends IParsedSearch {}

type IAuthField = IAuthNormalField | IAuthHashField;
interface IAuthFieldBase {
    key: string;
}
interface IAuthNormalField extends IAuthFieldBase {
    type: "username" | "password";
}
interface IAuthHashField extends IAuthFieldBase {
    type: "hash";
    hash: "md5" | "sha1";
    salt: string;
}

type IAuthCheck = IAuthCheckCookie;
interface IAuthCheckCookie {
    type: "cookie";
    key: string;
}

interface IAuth {
    type: "url" | "get" | "post" | "oauth2";
    fields: IAuthField[];
    check?: IAuthCheck;
    [name: string]: any;
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
        url: (query: any, opts: any, previous: any) => IUrl | IError | string;
        parse: (src: string) => IParsedSearch | IError;
    };
    details?: {
        url: (id: number, md5: string) => IUrl | IError | string;
        parse: (src: string) => IParsedDetails | IError;
    };
    gallery?: {
        url: (query: any, opts: any) => IUrl | IError | string;
        parse: (src: string) => IParsedGallery | IError;
    };
    tags?: {
        url: (query: any, opts: any) => IUrl | IError | string;
        parse: (src: string) => IParsedTags | IError;
    };
    check?: {
        url: () => IUrl | IError | string;
        parse: (src: string) => boolean | IError;
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