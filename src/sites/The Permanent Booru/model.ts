// The Permanent Booru

/* Since this Booru is only accessible through anonymized overlay networks, it must be accompanied by Grabber's proxy settings.
 * If you are using the Tor Browser Bundle (https://www.torproject.org) on ​​Windows, go to Grabber->Preference->Proxy while Tor Browser is running and configure it as follows.
 * ☑ Use Proxy
 * ☐ Use system-wide proxy settings
 * Type: SOCKS v5
 * Host: 127.0.0.1
 * Port: 9150
 * User:
 * Password:
 * If you use i2p or lokinet, the contents of other proxy settings may vary.
*/

/*
 * site maps *
 * / -> main page
 * /posts/<pagenumber>/<and_keyword>?filter=<filter_keyword>&or=<or_keyword>&unless=<unless_keyword> -> search result
 * /user/pools/<pool_number> -> pool page
 * /tags -> tage list
 * /wall/ -> board
 * /comics/ -> comics, gallery
 * /upload/ -> upload page
 * /user/<user_number> -> user page
 * /user/message/new/?recipient=<user_number> -> send message
 * /.data//files/<last_3_character_of_sha256_hash>/<sha256_hash>.<file_ext> -> original file(Download)
 * /spine/?page=<page_number>&user=<user_number> -> log
 * /post/<postID> -> image select by post number
 * /post/md5/<image_md5> -> image select by md5
 * /post/sha256/<image_sha256> -> image select by sha256
 * /api/v1/posts -> Image List API(JSON), more info:https://github.com/Kycklingar/pbooru-downloader
 * /tombstone/ -> List of images removed from their original source sites (with reasons)
 * /dns/ -> Link to the artist's SNS or art posting site
 * /candidates -> A page where users vote for images that have different hashes but are actually duplicates (requires JavaScript, need login)
*/

/* This booru sets the number of images per page as OPTION method instead of passing it as GET method.
 * So if you want to change the number of images per page, you need to change the cookie value in the source settings.
*/

// Sometimes comments on a line are taken from another model.js, but it is not clear what they are used for.

// -------- Typescript Type Narrowing Start --------

// Check if an object is an ITag interface
function isITagArr(obj: any): obj is ITag[]{
    return obj.every((i: any) => i.name);
}

// Check if an object is an String Array
function isStringArr(obj: any): obj is string[] {
    return obj.every((i: any) => typeof i === "string");
}

// Check if an object is an String
function isString(obj: any): obj is string {
    return typeof obj === "string";
}

// -------- Typescript Type Narrowing End --------


// -------- Static Map Start --------

// tag type name -> tag type ID
const TAG_NAME_TO_TTYPE_ID_MAP: Record<string, number> = {
    "none": 0,
    "rating": 1,
    "meta": 2,
    "medium": 3,
    "series": 4,
    "gender": 5,
    "species": 6,
    "creator": 7,
    "character": 8
};

// tag namespace -> tag type name
const TAG_NAME_TO_GRB_TTYPE_MAP: Record<string, string> = {
    "none": "general",
    "rating": "meta",
    "meta": "meta",
    "medium": "medium",
    "series": "copyright",
    "gender": "general",
    "species": "species",
    "creator": "artist",
    "character": "character"
};

// HTML entity(code) -> Character
// about this map: https://html.spec.whatwg.org/multipage/named-characters.html
const html_entity_code_normal_character_maps: Record<string, string> = {
    "&#198;": "\u00C6", // &AElig &AElig;
    "&#38;": "\u0026", // &AMP &AMP; &amp &amp;
    "&#193;": "\u00C1", // &Aacute &Aacute;
    "&#258;": "\u0102", // &Abreve;
    "&#194;": "\u00C2", // &Acirc &Acirc;
    "&#1040;": "\u0410", // &Acy;
    "&#120068;": "\uD835\uDD04", // &Afr;
    "&#192;": "\u00C0", // &Agrave &Agrave;
    "&#913;": "\u0391", // &Alpha;
    "&#256;": "\u0100", // &Amacr;
    "&#10835;": "\u2A53", // &And;
    "&#260;": "\u0104", // &Aogon;
    "&#120120;": "\uD835\uDD38", // &Aopf;
    "&#8289;": "\u2061", // &ApplyFunction; &af;
    "&#197;": "\u00C5", // &Aring &Aring; &angst;
    "&#119964;": "\uD835\uDC9C", // &Ascr;
    "&#8788;": "\u2254", // &Assign; &colone; &coloneq;
    "&#195;": "\u00C3", // &Atilde &Atilde;
    "&#196;": "\u00C4", // &Auml &Auml;
    "&#8726;": "\u2216", // &Backslash; &setminus; &setmn; &smallsetminus; &ssetmn;
    "&#10983;": "\u2AE7", // &Barv;
    "&#8966;": "\u2306", // &Barwed; &doublebarwedge;
    "&#1041;": "\u0411", // &Bcy;
    "&#8757;": "\u2235", // &Because; &becaus; &because;
    "&#8492;": "\u212C", // &Bernoullis; &Bscr; &bernou;
    "&#914;": "\u0392", // &Beta;
    "&#120069;": "\uD835\uDD05", // &Bfr;
    "&#120121;": "\uD835\uDD39", // &Bopf;
    "&#728;": "\u02D8", // &Breve; &breve;
    "&#8782;": "\u224E", // &Bumpeq; &HumpDownHump; &NotHumpDownHump; &bump; &nbump;
    "&#1063;": "\u0427", // &CHcy;
    "&#169;": "\u00A9", // &COPY &COPY; &copy &copy;
    "&#262;": "\u0106", // &Cacute;
    "&#8914;": "\u22D2", // &Cap;
    "&#8517;": "\u2145", // &CapitalDifferentialD; &DD;
    "&#8493;": "\u212D", // &Cayleys; &Cfr;
    "&#268;": "\u010C", // &Ccaron;
    "&#199;": "\u00C7", // &Ccedil &Ccedil;
    "&#264;": "\u0108", // &Ccirc;
    "&#8752;": "\u2230", // &Cconint;
    "&#266;": "\u010A", // &Cdot;
    "&#184;": "\u00B8", // &Cedilla; &cedil &cedil;
    "&#183;": "\u00B7", // &CenterDot; &centerdot; &middot &middot;
    "&#935;": "\u03A7", // &Chi;
    "&#8857;": "\u2299", // &CircleDot; &odot;
    "&#8854;": "\u2296", // &CircleMinus; &ominus;
    "&#8853;": "\u2295", // &CirclePlus; &oplus;
    "&#8855;": "\u2297", // &CircleTimes; &otimes;
    "&#8754;": "\u2232", // &ClockwiseContourIntegral; &cwconint;
    "&#8221;": "\u201D", // &CloseCurlyDoubleQuote; &rdquo; &rdquor;
    "&#8217;": "\u2019", // &CloseCurlyQuote; &rsquo; &rsquor;
    "&#8759;": "\u2237", // &Colon; &Proportion;
    "&#10868;": "\u2A74", // &Colone;
    "&#8801;": "\u2261", // &Congruent; &bnequiv; &equiv;
    "&#8751;": "\u222F", // &Conint; &DoubleContourIntegral;
    "&#8750;": "\u222E", // &ContourIntegral; &conint; &oint;
    "&#8450;": "\u2102", // &Copf; &complexes;
    "&#8720;": "\u2210", // &Coproduct; &coprod;
    "&#8755;": "\u2233", // &CounterClockwiseContourIntegral; &awconint;
    "&#10799;": "\u2A2F", // &Cross;
    "&#119966;": "\uD835\uDC9E", // &Cscr;
    "&#8915;": "\u22D3", // &Cup;
    "&#8781;": "\u224D", // &CupCap; &asympeq; &nvap;
    "&#10513;": "\u2911", // &DDotrahd;
    "&#1026;": "\u0402", // &DJcy;
    "&#1029;": "\u0405", // &DScy;
    "&#1039;": "\u040F", // &DZcy;
    "&#8225;": "\u2021", // &Dagger; &ddagger;
    "&#8609;": "\u21A1", // &Darr;
    "&#10980;": "\u2AE4", // &Dashv; &DoubleLeftTee;
    "&#270;": "\u010E", // &Dcaron;
    "&#1044;": "\u0414", // &Dcy;
    "&#8711;": "\u2207", // &Del; &nabla;
    "&#916;": "\u0394", // &Delta;
    "&#120071;": "\uD835\uDD07", // &Dfr;
    "&#180;": "\u00B4", // &DiacriticalAcute; &acute &acute;
    "&#729;": "\u02D9", // &DiacriticalDot; &dot;
    "&#733;": "\u02DD", // &DiacriticalDoubleAcute; &dblac;
    "&#96;": "\u0060", // &DiacriticalGrave; &grave;
    "&#732;": "\u02DC", // &DiacriticalTilde; &tilde;
    "&#8900;": "\u22C4", // &Diamond; &diam; &diamond;
    "&#8518;": "\u2146", // &DifferentialD; &dd;
    "&#120123;": "\uD835\uDD3B", // &Dopf;
    "&#168;": "\u00A8", // &Dot; &DoubleDot; &die; &uml &uml;
    "&#8412;": "\u20DC", // &DotDot;
    "&#8784;": "\u2250", // &DotEqual; &doteq; &esdot; &nedot;
    "&#8659;": "\u21D3", // &DoubleDownArrow; &Downarrow; &dArr;
    "&#8656;": "\u21D0", // &DoubleLeftArrow; &Leftarrow; &lArr;
    "&#8660;": "\u21D4", // &DoubleLeftRightArrow; &Leftrightarrow; &hArr; &iff;
    "&#10232;": "\u27F8", // &DoubleLongLeftArrow; &Longleftarrow; &xlArr;
    "&#10234;": "\u27FA", // &DoubleLongLeftRightArrow; &Longleftrightarrow; &xhArr;
    "&#10233;": "\u27F9", // &DoubleLongRightArrow; &Longrightarrow; &xrArr;
    "&#8658;": "\u21D2", // &DoubleRightArrow; &Implies; &Rightarrow; &rArr;
    "&#8872;": "\u22A8", // &DoubleRightTee; &vDash;
    "&#8657;": "\u21D1", // &DoubleUpArrow; &Uparrow; &uArr;
    "&#8661;": "\u21D5", // &DoubleUpDownArrow; &Updownarrow; &vArr;
    "&#8741;": "\u2225", // &DoubleVerticalBar; &par; &parallel; &shortparallel; &spar;
    "&#8595;": "\u2193", // &DownArrow; &ShortDownArrow; &darr; &downarrow;
    "&#10515;": "\u2913", // &DownArrowBar;
    "&#8693;": "\u21F5", // &DownArrowUpArrow; &duarr;
    "&#785;": "\u0311", // &DownBreve;
    "&#10576;": "\u2950", // &DownLeftRightVector;
    "&#10590;": "\u295E", // &DownLeftTeeVector;
    "&#8637;": "\u21BD", // &DownLeftVector; &leftharpoondown; &lhard;
    "&#10582;": "\u2956", // &DownLeftVectorBar;
    "&#10591;": "\u295F", // &DownRightTeeVector;
    "&#8641;": "\u21C1", // &DownRightVector; &rhard; &rightharpoondown;
    "&#10583;": "\u2957", // &DownRightVectorBar;
    "&#8868;": "\u22A4", // &DownTee; &top;
    "&#8615;": "\u21A7", // &DownTeeArrow; &mapstodown;
    "&#119967;": "\uD835\uDC9F", // &Dscr;
    "&#272;": "\u0110", // &Dstrok;
    "&#330;": "\u014A", // &ENG;
    "&#208;": "\u00D0", // &ETH &ETH;
    "&#201;": "\u00C9", // &Eacute &Eacute;
    "&#282;": "\u011A", // &Ecaron;
    "&#202;": "\u00CA", // &Ecirc &Ecirc;
    "&#1069;": "\u042D", // &Ecy;
    "&#278;": "\u0116", // &Edot;
    "&#120072;": "\uD835\uDD08", // &Efr;
    "&#200;": "\u00C8", // &Egrave &Egrave;
    "&#8712;": "\u2208", // &Element; &in; &isin; &isinv;
    "&#274;": "\u0112", // &Emacr;
    "&#9723;": "\u25FB", // &EmptySmallSquare;
    "&#9643;": "\u25AB", // &EmptyVerySmallSquare;
    "&#280;": "\u0118", // &Eogon;
    "&#120124;": "\uD835\uDD3C", // &Eopf;
    "&#917;": "\u0395", // &Epsilon;
    "&#10869;": "\u2A75", // &Equal;
    "&#8770;": "\u2242", // &EqualTilde; &NotEqualTilde; &eqsim; &esim; &nesim;
    "&#8652;": "\u21CC", // &Equilibrium; &rightleftharpoons; &rlhar;
    "&#8496;": "\u2130", // &Escr; &expectation;
    "&#10867;": "\u2A73", // &Esim;
    "&#919;": "\u0397", // &Eta;
    "&#203;": "\u00CB", // &Euml &Euml;
    "&#8707;": "\u2203", // &Exists; &exist;
    "&#8519;": "\u2147", // &ExponentialE; &ee; &exponentiale;
    "&#1060;": "\u0424", // &Fcy;
    "&#120073;": "\uD835\uDD09", // &Ffr;
    "&#9724;": "\u25FC", // &FilledSmallSquare;
    "&#9642;": "\u25AA", // &FilledVerySmallSquare; &blacksquare; &squarf; &squf;
    "&#120125;": "\uD835\uDD3D", // &Fopf;
    "&#8704;": "\u2200", // &ForAll; &forall;
    "&#8497;": "\u2131", // &Fouriertrf; &Fscr;
    "&#1027;": "\u0403", // &GJcy;
    "&#62;": "\u003E", // &GT &GT; &gt &gt; &nvgt;
    "&#915;": "\u0393", // &Gamma;
    "&#988;": "\u03DC", // &Gammad;
    "&#286;": "\u011E", // &Gbreve;
    "&#290;": "\u0122", // &Gcedil;
    "&#284;": "\u011C", // &Gcirc;
    "&#1043;": "\u0413", // &Gcy;
    "&#288;": "\u0120", // &Gdot;
    "&#120074;": "\uD835\uDD0A", // &Gfr;
    "&#8921;": "\u22D9", // &Gg; &ggg; &nGg;
    "&#120126;": "\uD835\uDD3E", // &Gopf;
    "&#8805;": "\u2265", // &GreaterEqual; &ge; &geq; &nvge;
    "&#8923;": "\u22DB", // &GreaterEqualLess; &gel; &gesl; &gtreqless;
    "&#8807;": "\u2267", // &GreaterFullEqual; &NotGreaterFullEqual; &gE; &geqq; &ngE; &ngeqq;
    "&#10914;": "\u2AA2", // &GreaterGreater; &NotNestedGreaterGreater;
    "&#8823;": "\u2277", // &GreaterLess; &gl; &gtrless;
    "&#10878;": "\u2A7E", // &GreaterSlantEqual; &NotGreaterSlantEqual; &geqslant; &ges; &ngeqslant; &nges;
    "&#8819;": "\u2273", // &GreaterTilde; &gsim; &gtrsim;
    "&#119970;": "\uD835\uDCA2", // &Gscr;
    "&#8811;": "\u226B", // &Gt; &NestedGreaterGreater; &NotGreaterGreater; &gg; &nGt; &nGtv;
    "&#1066;": "\u042A", // &HARDcy;
    "&#711;": "\u02C7", // &Hacek; &caron;
    "&#94;": "\u005E", // &Hat;
    "&#292;": "\u0124", // &Hcirc;
    "&#8460;": "\u210C", // &Hfr; &Poincareplane;
    "&#8459;": "\u210B", // &HilbertSpace; &Hscr; &hamilt;
    "&#8461;": "\u210D", // &Hopf; &quaternions;
    "&#9472;": "\u2500", // &HorizontalLine; &boxh;
    "&#294;": "\u0126", // &Hstrok;
    "&#8783;": "\u224F", // &HumpEqual; &NotHumpEqual; &bumpe; &bumpeq; &nbumpe;
    "&#1045;": "\u0415", // &IEcy;
    "&#306;": "\u0132", // &IJlig;
    "&#1025;": "\u0401", // &IOcy;
    "&#205;": "\u00CD", // &Iacute &Iacute;
    "&#206;": "\u00CE", // &Icirc &Icirc;
    "&#1048;": "\u0418", // &Icy;
    "&#304;": "\u0130", // &Idot;
    "&#8465;": "\u2111", // &Ifr; &Im; &image; &imagpart;
    "&#204;": "\u00CC", // &Igrave &Igrave;
    "&#298;": "\u012A", // &Imacr;
    "&#8520;": "\u2148", // &ImaginaryI; &ii;
    "&#8748;": "\u222C", // &Int;
    "&#8747;": "\u222B", // &Integral; &int;
    "&#8898;": "\u22C2", // &Intersection; &bigcap; &xcap;
    "&#8291;": "\u2063", // &InvisibleComma; &ic;
    "&#8290;": "\u2062", // &InvisibleTimes; &it;
    "&#302;": "\u012E", // &Iogon;
    "&#120128;": "\uD835\uDD40", // &Iopf;
    "&#921;": "\u0399", // &Iota;
    "&#8464;": "\u2110", // &Iscr; &imagline;
    "&#296;": "\u0128", // &Itilde;
    "&#1030;": "\u0406", // &Iukcy;
    "&#207;": "\u00CF", // &Iuml &Iuml;
    "&#308;": "\u0134", // &Jcirc;
    "&#1049;": "\u0419", // &Jcy;
    "&#120077;": "\uD835\uDD0D", // &Jfr;
    "&#120129;": "\uD835\uDD41", // &Jopf;
    "&#119973;": "\uD835\uDCA5", // &Jscr;
    "&#1032;": "\u0408", // &Jsercy;
    "&#1028;": "\u0404", // &Jukcy;
    "&#1061;": "\u0425", // &KHcy;
    "&#1036;": "\u040C", // &KJcy;
    "&#922;": "\u039A", // &Kappa;
    "&#310;": "\u0136", // &Kcedil;
    "&#1050;": "\u041A", // &Kcy;
    "&#120078;": "\uD835\uDD0E", // &Kfr;
    "&#120130;": "\uD835\uDD42", // &Kopf;
    "&#119974;": "\uD835\uDCA6", // &Kscr;
    "&#1033;": "\u0409", // &LJcy;
    "&#60;": "\u003C", // &LT &LT; &lt &lt; &nvlt;
    "&#313;": "\u0139", // &Lacute;
    "&#923;": "\u039B", // &Lambda;
    "&#10218;": "\u27EA", // &Lang;
    "&#8466;": "\u2112", // &Laplacetrf; &Lscr; &lagran;
    "&#8606;": "\u219E", // &Larr; &twoheadleftarrow;
    "&#317;": "\u013D", // &Lcaron;
    "&#315;": "\u013B", // &Lcedil;
    "&#1051;": "\u041B", // &Lcy;
    "&#10216;": "\u27E8", // &LeftAngleBracket; &lang; &langle;
    "&#8592;": "\u2190", // &LeftArrow; &ShortLeftArrow; &larr; &leftarrow; &slarr;
    "&#8676;": "\u21E4", // &LeftArrowBar; &larrb;
    "&#8646;": "\u21C6", // &LeftArrowRightArrow; &leftrightarrows; &lrarr;
    "&#8968;": "\u2308", // &LeftCeiling; &lceil;
    "&#10214;": "\u27E6", // &LeftDoubleBracket; &lobrk;
    "&#10593;": "\u2961", // &LeftDownTeeVector;
    "&#8643;": "\u21C3", // &LeftDownVector; &dharl; &downharpoonleft;
    "&#10585;": "\u2959", // &LeftDownVectorBar;
    "&#8970;": "\u230A", // &LeftFloor; &lfloor;
    "&#8596;": "\u2194", // &LeftRightArrow; &harr; &leftrightarrow;
    "&#10574;": "\u294E", // &LeftRightVector;
    "&#8867;": "\u22A3", // &LeftTee; &dashv;
    "&#8612;": "\u21A4", // &LeftTeeArrow; &mapstoleft;
    "&#10586;": "\u295A", // &LeftTeeVector;
    "&#8882;": "\u22B2", // &LeftTriangle; &vartriangleleft; &vltri;
    "&#10703;": "\u29CF", // &LeftTriangleBar; &NotLeftTriangleBar;
    "&#8884;": "\u22B4", // &LeftTriangleEqual; &ltrie; &nvltrie; &trianglelefteq;
    "&#10577;": "\u2951", // &LeftUpDownVector;
    "&#10592;": "\u2960", // &LeftUpTeeVector;
    "&#8639;": "\u21BF", // &LeftUpVector; &uharl; &upharpoonleft;
    "&#10584;": "\u2958", // &LeftUpVectorBar;
    "&#8636;": "\u21BC", // &LeftVector; &leftharpoonup; &lharu;
    "&#10578;": "\u2952", // &LeftVectorBar;
    "&#8922;": "\u22DA", // &LessEqualGreater; &leg; &lesg; &lesseqgtr;
    "&#8806;": "\u2266", // &LessFullEqual; &lE; &leqq; &nlE; &nleqq;
    "&#8822;": "\u2276", // &LessGreater; &lessgtr; &lg;
    "&#10913;": "\u2AA1", // &LessLess; &NotNestedLessLess;
    "&#10877;": "\u2A7D", // &LessSlantEqual; &NotLessSlantEqual; &leqslant; &les; &nleqslant; &nles;
    "&#8818;": "\u2272", // &LessTilde; &lesssim; &lsim;
    "&#120079;": "\uD835\uDD0F", // &Lfr;
    "&#8920;": "\u22D8", // &Ll; &nLl;
    "&#8666;": "\u21DA", // &Lleftarrow; &lAarr;
    "&#319;": "\u013F", // &Lmidot;
    "&#10229;": "\u27F5", // &LongLeftArrow; &longleftarrow; &xlarr;
    "&#10231;": "\u27F7", // &LongLeftRightArrow; &longleftrightarrow; &xharr;
    "&#10230;": "\u27F6", // &LongRightArrow; &longrightarrow; &xrarr;
    "&#120131;": "\uD835\uDD43", // &Lopf;
    "&#8601;": "\u2199", // &LowerLeftArrow; &swarr; &swarrow;
    "&#8600;": "\u2198", // &LowerRightArrow; &searr; &searrow;
    "&#8624;": "\u21B0", // &Lsh; &lsh;
    "&#321;": "\u0141", // &Lstrok;
    "&#8810;": "\u226A", // &Lt; &NestedLessLess; &NotLessLess; &ll; &nLt; &nLtv;
    "&#10501;": "\u2905", // &Map;
    "&#1052;": "\u041C", // &Mcy;
    "&#8287;": "\u205F", // &MediumSpace; &ThickSpace;
    "&#8499;": "\u2133", // &Mellintrf; &Mscr; &phmmat;
    "&#120080;": "\uD835\uDD10", // &Mfr;
    "&#8723;": "\u2213", // &MinusPlus; &mnplus; &mp;
    "&#120132;": "\uD835\uDD44", // &Mopf;
    "&#924;": "\u039C", // &Mu;
    "&#1034;": "\u040A", // &NJcy;
    "&#323;": "\u0143", // &Nacute;
    "&#327;": "\u0147", // &Ncaron;
    "&#325;": "\u0145", // &Ncedil;
    "&#1053;": "\u041D", // &Ncy;
    "&#8203;": "\u200B", // &NegativeMediumSpace; &NegativeThickSpace; &NegativeThinSpace; &NegativeVeryThinSpace; &ZeroWidthSpace;
    "&#10;": "\u000A", // &NewLine;
    "&#120081;": "\uD835\uDD11", // &Nfr;
    "&#8288;": "\u2060", // &NoBreak;
    "&#160;": "\u00A0", // &NonBreakingSpace; &nbsp &nbsp;
    "&#8469;": "\u2115", // &Nopf; &naturals;
    "&#10988;": "\u2AEC", // &Not;
    "&#8802;": "\u2262", // &NotCongruent; &nequiv;
    "&#8813;": "\u226D", // &NotCupCap;
    "&#8742;": "\u2226", // &NotDoubleVerticalBar; &npar; &nparallel; &nshortparallel; &nspar;
    "&#8713;": "\u2209", // &NotElement; &notin; &notinva;
    "&#8800;": "\u2260", // &NotEqual; &ne;
    "&#8708;": "\u2204", // &NotExists; &nexist; &nexists;
    "&#8815;": "\u226F", // &NotGreater; &ngt; &ngtr;
    "&#8817;": "\u2271", // &NotGreaterEqual; &nge; &ngeq;
    "&#8825;": "\u2279", // &NotGreaterLess; &ntgl;
    "&#8821;": "\u2275", // &NotGreaterTilde; &ngsim;
    "&#8938;": "\u22EA", // &NotLeftTriangle; &nltri; &ntriangleleft;
    "&#8940;": "\u22EC", // &NotLeftTriangleEqual; &nltrie; &ntrianglelefteq;
    "&#8814;": "\u226E", // &NotLess; &nless; &nlt;
    "&#8816;": "\u2270", // &NotLessEqual; &nle; &nleq;
    "&#8824;": "\u2278", // &NotLessGreater; &ntlg;
    "&#8820;": "\u2274", // &NotLessTilde; &nlsim;
    "&#8832;": "\u2280", // &NotPrecedes; &npr; &nprec;
    "&#10927;": "\u2AAF\u0338", // &NotPrecedesEqual; &PrecedesEqual; &npre; &npreceq; &pre; &preceq;
    "&#8928;": "\u22E0", // &NotPrecedesSlantEqual; &nprcue;
    "&#8716;": "\u220C", // &NotReverseElement; &notni; &notniva;
    "&#8939;": "\u22EB", // &NotRightTriangle; &nrtri; &ntriangleright;
    "&#10704;": "\u29D0\u0338", // &NotRightTriangleBar; &RightTriangleBar;
    "&#8941;": "\u22ED", // &NotRightTriangleEqual; &nrtrie; &ntrianglerighteq;
    "&#8847;": "\u228F\u0338", // &NotSquareSubset; &SquareSubset; &sqsub; &sqsubset;
    "&#8930;": "\u22E2", // &NotSquareSubsetEqual; &nsqsube;
    "&#8848;": "\u2290\u0338", // &NotSquareSuperset; &SquareSuperset; &sqsup; &sqsupset;
    "&#8931;": "\u22E3", // &NotSquareSupersetEqual; &nsqsupe;
    "&#8834;": "\u2282\u20D2", // &NotSubset; &nsubset; &sub; &subset; &vnsub;
    "&#8840;": "\u2288", // &NotSubsetEqual; &nsube; &nsubseteq;
    "&#8833;": "\u2281", // &NotSucceeds; &nsc; &nsucc;
    "&#10928;": "\u2AB0\u0338", // &NotSucceedsEqual; &SucceedsEqual; &nsce; &nsucceq; &sce; &succeq;
    "&#8929;": "\u22E1", // &NotSucceedsSlantEqual; &nsccue;
    "&#8831;": "\u227F\u0338", // &NotSucceedsTilde; &SucceedsTilde; &scsim; &succsim;
    "&#8835;": "\u2283\u20D2", // &NotSuperset; &Superset; &nsupset; &sup; &supset; &vnsup;
    "&#8841;": "\u2289", // &NotSupersetEqual; &nsupe; &nsupseteq;
    "&#8769;": "\u2241", // &NotTilde; &nsim;
    "&#8772;": "\u2244", // &NotTildeEqual; &nsime; &nsimeq;
    "&#8775;": "\u2247", // &NotTildeFullEqual; &ncong;
    "&#8777;": "\u2249", // &NotTildeTilde; &nap; &napprox;
    "&#8740;": "\u2224", // &NotVerticalBar; &nmid; &nshortmid; &nsmid;
    "&#119977;": "\uD835\uDCA9", // &Nscr;
    "&#209;": "\u00D1", // &Ntilde &Ntilde;
    "&#925;": "\u039D", // &Nu;
    "&#338;": "\u0152", // &OElig;
    "&#211;": "\u00D3", // &Oacute &Oacute;
    "&#212;": "\u00D4", // &Ocirc &Ocirc;
    "&#1054;": "\u041E", // &Ocy;
    "&#336;": "\u0150", // &Odblac;
    "&#120082;": "\uD835\uDD12", // &Ofr;
    "&#210;": "\u00D2", // &Ograve &Ograve;
    "&#332;": "\u014C", // &Omacr;
    "&#937;": "\u03A9", // &Omega; &ohm;
    "&#927;": "\u039F", // &Omicron;
    "&#120134;": "\uD835\uDD46", // &Oopf;
    "&#8220;": "\u201C", // &OpenCurlyDoubleQuote; &ldquo;
    "&#8216;": "\u2018", // &OpenCurlyQuote; &lsquo;
    "&#10836;": "\u2A54", // &Or;
    "&#119978;": "\uD835\uDCAA", // &Oscr;
    "&#216;": "\u00D8", // &Oslash &Oslash;
    "&#213;": "\u00D5", // &Otilde &Otilde;
    "&#10807;": "\u2A37", // &Otimes;
    "&#214;": "\u00D6", // &Ouml &Ouml;
    "&#8254;": "\u203E", // &OverBar; &oline;
    "&#9182;": "\u23DE", // &OverBrace;
    "&#9140;": "\u23B4", // &OverBracket; &tbrk;
    "&#9180;": "\u23DC", // &OverParenthesis;
    "&#8706;": "\u2202", // &PartialD; &npart; &part;
    "&#1055;": "\u041F", // &Pcy;
    "&#120083;": "\uD835\uDD13", // &Pfr;
    "&#934;": "\u03A6", // &Phi;
    "&#928;": "\u03A0", // &Pi;
    "&#177;": "\u00B1", // &PlusMinus; &plusmn &plusmn; &pm;
    "&#8473;": "\u2119", // &Popf; &primes;
    "&#10939;": "\u2ABB", // &Pr;
    "&#8826;": "\u227A", // &Precedes; &pr; &prec;
    "&#8828;": "\u227C", // &PrecedesSlantEqual; &prcue; &preccurlyeq;
    "&#8830;": "\u227E", // &PrecedesTilde; &precsim; &prsim;
    "&#8243;": "\u2033", // &Prime;
    "&#8719;": "\u220F", // &Product; &prod;
    "&#8733;": "\u221D", // &Proportional; &prop; &propto; &varpropto; &vprop;
    "&#119979;": "\uD835\uDCAB", // &Pscr;
    "&#936;": "\u03A8", // &Psi;
    "&#34;": "\u0022", // &QUOT &QUOT; &quot &quot;
    "&#120084;": "\uD835\uDD14", // &Qfr;
    "&#8474;": "\u211A", // &Qopf; &rationals;
    "&#119980;": "\uD835\uDCAC", // &Qscr;
    "&#10512;": "\u2910", // &RBarr; &drbkarow;
    "&#174;": "\u00AE", // &REG &REG; &circledR; &reg &reg;
    "&#340;": "\u0154", // &Racute;
    "&#10219;": "\u27EB", // &Rang;
    "&#8608;": "\u21A0", // &Rarr; &twoheadrightarrow;
    "&#10518;": "\u2916", // &Rarrtl;
    "&#344;": "\u0158", // &Rcaron;
    "&#342;": "\u0156", // &Rcedil;
    "&#1056;": "\u0420", // &Rcy;
    "&#8476;": "\u211C", // &Re; &Rfr; &real; &realpart;
    "&#8715;": "\u220B", // &ReverseElement; &SuchThat; &ni; &niv;
    "&#8651;": "\u21CB", // &ReverseEquilibrium; &leftrightharpoons; &lrhar;
    "&#10607;": "\u296F", // &ReverseUpEquilibrium; &duhar;
    "&#929;": "\u03A1", // &Rho;
    "&#10217;": "\u27E9", // &RightAngleBracket; &rang; &rangle;
    "&#8594;": "\u2192", // &RightArrow; &ShortRightArrow; &rarr; &rightarrow; &srarr;
    "&#8677;": "\u21E5", // &RightArrowBar; &rarrb;
    "&#8644;": "\u21C4", // &RightArrowLeftArrow; &rightleftarrows; &rlarr;
    "&#8969;": "\u2309", // &RightCeiling; &rceil;
    "&#10215;": "\u27E7", // &RightDoubleBracket; &robrk;
    "&#10589;": "\u295D", // &RightDownTeeVector;
    "&#8642;": "\u21C2", // &RightDownVector; &dharr; &downharpoonright;
    "&#10581;": "\u2955", // &RightDownVectorBar;
    "&#8971;": "\u230B", // &RightFloor; &rfloor;
    "&#8866;": "\u22A2", // &RightTee; &vdash;
    "&#8614;": "\u21A6", // &RightTeeArrow; &map; &mapsto;
    "&#10587;": "\u295B", // &RightTeeVector;
    "&#8883;": "\u22B3", // &RightTriangle; &vartriangleright; &vrtri;
    "&#8885;": "\u22B5", // &RightTriangleEqual; &nvrtrie; &rtrie; &trianglerighteq;
    "&#10575;": "\u294F", // &RightUpDownVector;
    "&#10588;": "\u295C", // &RightUpTeeVector;
    "&#8638;": "\u21BE", // &RightUpVector; &uharr; &upharpoonright;
    "&#10580;": "\u2954", // &RightUpVectorBar;
    "&#8640;": "\u21C0", // &RightVector; &rharu; &rightharpoonup;
    "&#10579;": "\u2953", // &RightVectorBar;
    "&#8477;": "\u211D", // &Ropf; &reals;
    "&#10608;": "\u2970", // &RoundImplies;
    "&#8667;": "\u21DB", // &Rrightarrow; &rAarr;
    "&#8475;": "\u211B", // &Rscr; &realine;
    "&#8625;": "\u21B1", // &Rsh; &rsh;
    "&#10740;": "\u29F4", // &RuleDelayed;
    "&#1065;": "\u0429", // &SHCHcy;
    "&#1064;": "\u0428", // &SHcy;
    "&#1068;": "\u042C", // &SOFTcy;
    "&#346;": "\u015A", // &Sacute;
    "&#10940;": "\u2ABC", // &Sc;
    "&#352;": "\u0160", // &Scaron;
    "&#350;": "\u015E", // &Scedil;
    "&#348;": "\u015C", // &Scirc;
    "&#1057;": "\u0421", // &Scy;
    "&#120086;": "\uD835\uDD16", // &Sfr;
    "&#8593;": "\u2191", // &ShortUpArrow; &UpArrow; &uarr; &uparrow;
    "&#931;": "\u03A3", // &Sigma;
    "&#8728;": "\u2218", // &SmallCircle; &compfn;
    "&#120138;": "\uD835\uDD4A", // &Sopf;
    "&#8730;": "\u221A", // &Sqrt; &radic;
    "&#9633;": "\u25A1", // &Square; &squ; &square;
    "&#8851;": "\u2293", // &SquareIntersection; &sqcap; &sqcaps;
    "&#8849;": "\u2291", // &SquareSubsetEqual; &sqsube; &sqsubseteq;
    "&#8850;": "\u2292", // &SquareSupersetEqual; &sqsupe; &sqsupseteq;
    "&#8852;": "\u2294", // &SquareUnion; &sqcup; &sqcups;
    "&#119982;": "\uD835\uDCAE", // &Sscr;
    "&#8902;": "\u22C6", // &Star; &sstarf;
    "&#8912;": "\u22D0", // &Sub; &Subset;
    "&#8838;": "\u2286", // &SubsetEqual; &sube; &subseteq;
    "&#8827;": "\u227B", // &Succeeds; &sc; &succ;
    "&#8829;": "\u227D", // &SucceedsSlantEqual; &sccue; &succcurlyeq;
    "&#8721;": "\u2211", // &Sum; &sum;
    "&#8913;": "\u22D1", // &Sup; &Supset;
    "&#8839;": "\u2287", // &SupersetEqual; &supe; &supseteq;
    "&#222;": "\u00DE", // &THORN &THORN;
    "&#8482;": "\u2122", // &TRADE; &trade;
    "&#1035;": "\u040B", // &TSHcy;
    "&#1062;": "\u0426", // &TScy;
    "&#9;": "\u0009", // &Tab;
    "&#932;": "\u03A4", // &Tau;
    "&#356;": "\u0164", // &Tcaron;
    "&#354;": "\u0162", // &Tcedil;
    "&#1058;": "\u0422", // &Tcy;
    "&#120087;": "\uD835\uDD17", // &Tfr;
    "&#8756;": "\u2234", // &Therefore; &there4; &therefore;
    "&#920;": "\u0398", // &Theta;
    "&#8201;": "\u2009", // &ThinSpace; &thinsp;
    "&#8764;": "\u223C", // &Tilde; &nvsim; &sim; &thicksim; &thksim;
    "&#8771;": "\u2243", // &TildeEqual; &sime; &simeq;
    "&#8773;": "\u2245", // &TildeFullEqual; &cong;
    "&#8776;": "\u2248", // &TildeTilde; &ap; &approx; &asymp; &thickapprox; &thkap;
    "&#120139;": "\uD835\uDD4B", // &Topf;
    "&#8411;": "\u20DB", // &TripleDot; &tdot;
    "&#119983;": "\uD835\uDCAF", // &Tscr;
    "&#358;": "\u0166", // &Tstrok;
    "&#218;": "\u00DA", // &Uacute &Uacute;
    "&#8607;": "\u219F", // &Uarr;
    "&#10569;": "\u2949", // &Uarrocir;
    "&#1038;": "\u040E", // &Ubrcy;
    "&#364;": "\u016C", // &Ubreve;
    "&#219;": "\u00DB", // &Ucirc &Ucirc;
    "&#1059;": "\u0423", // &Ucy;
    "&#368;": "\u0170", // &Udblac;
    "&#120088;": "\uD835\uDD18", // &Ufr;
    "&#217;": "\u00D9", // &Ugrave &Ugrave;
    "&#362;": "\u016A", // &Umacr;
    "&#95;": "\u005F", // &UnderBar; &lowbar;
    "&#9183;": "\u23DF", // &UnderBrace;
    "&#9141;": "\u23B5", // &UnderBracket; &bbrk;
    "&#9181;": "\u23DD", // &UnderParenthesis;
    "&#8899;": "\u22C3", // &Union; &bigcup; &xcup;
    "&#8846;": "\u228E", // &UnionPlus; &uplus;
    "&#370;": "\u0172", // &Uogon;
    "&#120140;": "\uD835\uDD4C", // &Uopf;
    "&#10514;": "\u2912", // &UpArrowBar;
    "&#8645;": "\u21C5", // &UpArrowDownArrow; &udarr;
    "&#8597;": "\u2195", // &UpDownArrow; &updownarrow; &varr;
    "&#10606;": "\u296E", // &UpEquilibrium; &udhar;
    "&#8869;": "\u22A5", // &UpTee; &bot; &bottom; &perp;
    "&#8613;": "\u21A5", // &UpTeeArrow; &mapstoup;
    "&#8598;": "\u2196", // &UpperLeftArrow; &nwarr; &nwarrow;
    "&#8599;": "\u2197", // &UpperRightArrow; &nearr; &nearrow;
    "&#978;": "\u03D2", // &Upsi; &upsih;
    "&#933;": "\u03A5", // &Upsilon;
    "&#366;": "\u016E", // &Uring;
    "&#119984;": "\uD835\uDCB0", // &Uscr;
    "&#360;": "\u0168", // &Utilde;
    "&#220;": "\u00DC", // &Uuml &Uuml;
    "&#8875;": "\u22AB", // &VDash;
    "&#10987;": "\u2AEB", // &Vbar;
    "&#1042;": "\u0412", // &Vcy;
    "&#8873;": "\u22A9", // &Vdash;
    "&#10982;": "\u2AE6", // &Vdashl;
    "&#8897;": "\u22C1", // &Vee; &bigvee; &xvee;
    "&#8214;": "\u2016", // &Verbar; &Vert;
    "&#8739;": "\u2223", // &VerticalBar; &mid; &shortmid; &smid;
    "&#124;": "\u007C", // &VerticalLine; &verbar; &vert;
    "&#10072;": "\u2758", // &VerticalSeparator;
    "&#8768;": "\u2240", // &VerticalTilde; &wr; &wreath;
    "&#8202;": "\u200A", // &VeryThinSpace; &hairsp;
    "&#120089;": "\uD835\uDD19", // &Vfr;
    "&#120141;": "\uD835\uDD4D", // &Vopf;
    "&#119985;": "\uD835\uDCB1", // &Vscr;
    "&#8874;": "\u22AA", // &Vvdash;
    "&#372;": "\u0174", // &Wcirc;
    "&#8896;": "\u22C0", // &Wedge; &bigwedge; &xwedge;
    "&#120090;": "\uD835\uDD1A", // &Wfr;
    "&#120142;": "\uD835\uDD4E", // &Wopf;
    "&#119986;": "\uD835\uDCB2", // &Wscr;
    "&#120091;": "\uD835\uDD1B", // &Xfr;
    "&#926;": "\u039E", // &Xi;
    "&#120143;": "\uD835\uDD4F", // &Xopf;
    "&#119987;": "\uD835\uDCB3", // &Xscr;
    "&#1071;": "\u042F", // &YAcy;
    "&#1031;": "\u0407", // &YIcy;
    "&#1070;": "\u042E", // &YUcy;
    "&#221;": "\u00DD", // &Yacute &Yacute;
    "&#374;": "\u0176", // &Ycirc;
    "&#1067;": "\u042B", // &Ycy;
    "&#120092;": "\uD835\uDD1C", // &Yfr;
    "&#120144;": "\uD835\uDD50", // &Yopf;
    "&#119988;": "\uD835\uDCB4", // &Yscr;
    "&#376;": "\u0178", // &Yuml;
    "&#1046;": "\u0416", // &ZHcy;
    "&#377;": "\u0179", // &Zacute;
    "&#381;": "\u017D", // &Zcaron;
    "&#1047;": "\u0417", // &Zcy;
    "&#379;": "\u017B", // &Zdot;
    "&#918;": "\u0396", // &Zeta;
    "&#8488;": "\u2128", // &Zfr; &zeetrf;
    "&#8484;": "\u2124", // &Zopf; &integers;
    "&#119989;": "\uD835\uDCB5", // &Zscr;
    "&#225;": "\u00E1", // &aacute &aacute;
    "&#259;": "\u0103", // &abreve;
    "&#8766;": "\u223E", // &ac; &acE; &mstpos;
    "&#8767;": "\u223F", // &acd;
    "&#226;": "\u00E2", // &acirc &acirc;
    "&#1072;": "\u0430", // &acy;
    "&#230;": "\u00E6", // &aelig &aelig;
    "&#120094;": "\uD835\uDD1E", // &afr;
    "&#224;": "\u00E0", // &agrave &agrave;
    "&#8501;": "\u2135", // &alefsym; &aleph;
    "&#945;": "\u03B1", // &alpha;
    "&#257;": "\u0101", // &amacr;
    "&#10815;": "\u2A3F", // &amalg;
    "&#8743;": "\u2227", // &and; &wedge;
    "&#10837;": "\u2A55", // &andand;
    "&#10844;": "\u2A5C", // &andd;
    "&#10840;": "\u2A58", // &andslope;
    "&#10842;": "\u2A5A", // &andv;
    "&#8736;": "\u2220", // &ang; &angle; &nang;
    "&#10660;": "\u29A4", // &ange;
    "&#8737;": "\u2221", // &angmsd; &measuredangle;
    "&#10664;": "\u29A8", // &angmsdaa;
    "&#10665;": "\u29A9", // &angmsdab;
    "&#10666;": "\u29AA", // &angmsdac;
    "&#10667;": "\u29AB", // &angmsdad;
    "&#10668;": "\u29AC", // &angmsdae;
    "&#10669;": "\u29AD", // &angmsdaf;
    "&#10670;": "\u29AE", // &angmsdag;
    "&#10671;": "\u29AF", // &angmsdah;
    "&#8735;": "\u221F", // &angrt;
    "&#8894;": "\u22BE", // &angrtvb;
    "&#10653;": "\u299D", // &angrtvbd;
    "&#8738;": "\u2222", // &angsph;
    "&#9084;": "\u237C", // &angzarr;
    "&#261;": "\u0105", // &aogon;
    "&#120146;": "\uD835\uDD52", // &aopf;
    "&#10864;": "\u2A70", // &apE; &napE;
    "&#10863;": "\u2A6F", // &apacir;
    "&#8778;": "\u224A", // &ape; &approxeq;
    "&#8779;": "\u224B", // &apid; &napid;
    "&#39;": "\u0027", // &apos;
    "&#229;": "\u00E5", // &aring &aring;
    "&#119990;": "\uD835\uDCB6", // &ascr;
    "&#42;": "\u002A", // &ast; &midast;
    "&#227;": "\u00E3", // &atilde &atilde;
    "&#228;": "\u00E4", // &auml &auml;
    "&#10769;": "\u2A11", // &awint;
    "&#10989;": "\u2AED", // &bNot;
    "&#8780;": "\u224C", // &backcong; &bcong;
    "&#1014;": "\u03F6", // &backepsilon; &bepsi;
    "&#8245;": "\u2035", // &backprime; &bprime;
    "&#8765;": "\u223D", // &backsim; &bsim; &race;
    "&#8909;": "\u22CD", // &backsimeq; &bsime;
    "&#8893;": "\u22BD", // &barvee;
    "&#8965;": "\u2305", // &barwed; &barwedge;
    "&#9142;": "\u23B6", // &bbrktbrk;
    "&#1073;": "\u0431", // &bcy;
    "&#8222;": "\u201E", // &bdquo; &ldquor;
    "&#10672;": "\u29B0", // &bemptyv;
    "&#946;": "\u03B2", // &beta;
    "&#8502;": "\u2136", // &beth;
    "&#8812;": "\u226C", // &between; &twixt;
    "&#120095;": "\uD835\uDD1F", // &bfr;
    "&#9711;": "\u25EF", // &bigcirc; &xcirc;
    "&#10752;": "\u2A00", // &bigodot; &xodot;
    "&#10753;": "\u2A01", // &bigoplus; &xoplus;
    "&#10754;": "\u2A02", // &bigotimes; &xotime;
    "&#10758;": "\u2A06", // &bigsqcup; &xsqcup;
    "&#9733;": "\u2605", // &bigstar; &starf;
    "&#9661;": "\u25BD", // &bigtriangledown; &xdtri;
    "&#9651;": "\u25B3", // &bigtriangleup; &xutri;
    "&#10756;": "\u2A04", // &biguplus; &xuplus;
    "&#10509;": "\u290D", // &bkarow; &rbarr;
    "&#10731;": "\u29EB", // &blacklozenge; &lozf;
    "&#9652;": "\u25B4", // &blacktriangle; &utrif;
    "&#9662;": "\u25BE", // &blacktriangledown; &dtrif;
    "&#9666;": "\u25C2", // &blacktriangleleft; &ltrif;
    "&#9656;": "\u25B8", // &blacktriangleright; &rtrif;
    "&#9251;": "\u2423", // &blank;
    "&#9618;": "\u2592", // &blk12;
    "&#9617;": "\u2591", // &blk14;
    "&#9619;": "\u2593", // &blk34;
    "&#9608;": "\u2588", // &block;
    "&#61;": "\u003D\u20E5", // &bne; &equals;
    "&#8976;": "\u2310", // &bnot;
    "&#120147;": "\uD835\uDD53", // &bopf;
    "&#8904;": "\u22C8", // &bowtie;
    "&#9559;": "\u2557", // &boxDL;
    "&#9556;": "\u2554", // &boxDR;
    "&#9558;": "\u2556", // &boxDl;
    "&#9555;": "\u2553", // &boxDr;
    "&#9552;": "\u2550", // &boxH;
    "&#9574;": "\u2566", // &boxHD;
    "&#9577;": "\u2569", // &boxHU;
    "&#9572;": "\u2564", // &boxHd;
    "&#9575;": "\u2567", // &boxHu;
    "&#9565;": "\u255D", // &boxUL;
    "&#9562;": "\u255A", // &boxUR;
    "&#9564;": "\u255C", // &boxUl;
    "&#9561;": "\u2559", // &boxUr;
    "&#9553;": "\u2551", // &boxV;
    "&#9580;": "\u256C", // &boxVH;
    "&#9571;": "\u2563", // &boxVL;
    "&#9568;": "\u2560", // &boxVR;
    "&#9579;": "\u256B", // &boxVh;
    "&#9570;": "\u2562", // &boxVl;
    "&#9567;": "\u255F", // &boxVr;
    "&#10697;": "\u29C9", // &boxbox;
    "&#9557;": "\u2555", // &boxdL;
    "&#9554;": "\u2552", // &boxdR;
    "&#9488;": "\u2510", // &boxdl;
    "&#9484;": "\u250C", // &boxdr;
    "&#9573;": "\u2565", // &boxhD;
    "&#9576;": "\u2568", // &boxhU;
    "&#9516;": "\u252C", // &boxhd;
    "&#9524;": "\u2534", // &boxhu;
    "&#8863;": "\u229F", // &boxminus; &minusb;
    "&#8862;": "\u229E", // &boxplus; &plusb;
    "&#8864;": "\u22A0", // &boxtimes; &timesb;
    "&#9563;": "\u255B", // &boxuL;
    "&#9560;": "\u2558", // &boxuR;
    "&#9496;": "\u2518", // &boxul;
    "&#9492;": "\u2514", // &boxur;
    "&#9474;": "\u2502", // &boxv;
    "&#9578;": "\u256A", // &boxvH;
    "&#9569;": "\u2561", // &boxvL;
    "&#9566;": "\u255E", // &boxvR;
    "&#9532;": "\u253C", // &boxvh;
    "&#9508;": "\u2524", // &boxvl;
    "&#9500;": "\u251C", // &boxvr;
    "&#166;": "\u00A6", // &brvbar &brvbar;
    "&#119991;": "\uD835\uDCB7", // &bscr;
    "&#8271;": "\u204F", // &bsemi;
    "&#92;": "\u005C", // &bsol;
    "&#10693;": "\u29C5", // &bsolb;
    "&#10184;": "\u27C8", // &bsolhsub;
    "&#8226;": "\u2022", // &bull; &bullet;
    "&#10926;": "\u2AAE", // &bumpE;
    "&#263;": "\u0107", // &cacute;
    "&#8745;": "\u2229", // &cap; &caps;
    "&#10820;": "\u2A44", // &capand;
    "&#10825;": "\u2A49", // &capbrcup;
    "&#10827;": "\u2A4B", // &capcap;
    "&#10823;": "\u2A47", // &capcup;
    "&#10816;": "\u2A40", // &capdot;
    "&#8257;": "\u2041", // &caret;
    "&#10829;": "\u2A4D", // &ccaps;
    "&#269;": "\u010D", // &ccaron;
    "&#231;": "\u00E7", // &ccedil &ccedil;
    "&#265;": "\u0109", // &ccirc;
    "&#10828;": "\u2A4C", // &ccups;
    "&#10832;": "\u2A50", // &ccupssm;
    "&#267;": "\u010B", // &cdot;
    "&#10674;": "\u29B2", // &cemptyv;
    "&#162;": "\u00A2", // &cent &cent;
    "&#120096;": "\uD835\uDD20", // &cfr;
    "&#1095;": "\u0447", // &chcy;
    "&#10003;": "\u2713", // &check; &checkmark;
    "&#967;": "\u03C7", // &chi;
    "&#9675;": "\u25CB", // &cir;
    "&#10691;": "\u29C3", // &cirE;
    "&#710;": "\u02C6", // &circ;
    "&#8791;": "\u2257", // &circeq; &cire;
    "&#8634;": "\u21BA", // &circlearrowleft; &olarr;
    "&#8635;": "\u21BB", // &circlearrowright; &orarr;
    "&#9416;": "\u24C8", // &circledS; &oS;
    "&#8859;": "\u229B", // &circledast; &oast;
    "&#8858;": "\u229A", // &circledcirc; &ocir;
    "&#8861;": "\u229D", // &circleddash; &odash;
    "&#10768;": "\u2A10", // &cirfnint;
    "&#10991;": "\u2AEF", // &cirmid;
    "&#10690;": "\u29C2", // &cirscir;
    "&#9827;": "\u2663", // &clubs; &clubsuit;
    "&#58;": "\u003A", // &colon;
    "&#44;": "\u002C", // &comma;
    "&#64;": "\u0040", // &commat;
    "&#8705;": "\u2201", // &comp; &complement;
    "&#10861;": "\u2A6D", // &congdot; &ncongdot;
    "&#120148;": "\uD835\uDD54", // &copf;
    "&#8471;": "\u2117", // &copysr;
    "&#8629;": "\u21B5", // &crarr;
    "&#10007;": "\u2717", // &cross;
    "&#119992;": "\uD835\uDCB8", // &cscr;
    "&#10959;": "\u2ACF", // &csub;
    "&#10961;": "\u2AD1", // &csube;
    "&#10960;": "\u2AD0", // &csup;
    "&#10962;": "\u2AD2", // &csupe;
    "&#8943;": "\u22EF", // &ctdot;
    "&#10552;": "\u2938", // &cudarrl;
    "&#10549;": "\u2935", // &cudarrr;
    "&#8926;": "\u22DE", // &cuepr; &curlyeqprec;
    "&#8927;": "\u22DF", // &cuesc; &curlyeqsucc;
    "&#8630;": "\u21B6", // &cularr; &curvearrowleft;
    "&#10557;": "\u293D", // &cularrp;
    "&#8746;": "\u222A", // &cup; &cups;
    "&#10824;": "\u2A48", // &cupbrcap;
    "&#10822;": "\u2A46", // &cupcap;
    "&#10826;": "\u2A4A", // &cupcup;
    "&#8845;": "\u228D", // &cupdot;
    "&#10821;": "\u2A45", // &cupor;
    "&#8631;": "\u21B7", // &curarr; &curvearrowright;
    "&#10556;": "\u293C", // &curarrm;
    "&#8910;": "\u22CE", // &curlyvee; &cuvee;
    "&#8911;": "\u22CF", // &curlywedge; &cuwed;
    "&#164;": "\u00A4", // &curren &curren;
    "&#8753;": "\u2231", // &cwint;
    "&#9005;": "\u232D", // &cylcty;
    "&#10597;": "\u2965", // &dHar;
    "&#8224;": "\u2020", // &dagger;
    "&#8504;": "\u2138", // &daleth;
    "&#8208;": "\u2010", // &dash; &hyphen;
    "&#10511;": "\u290F", // &dbkarow; &rBarr;
    "&#271;": "\u010F", // &dcaron;
    "&#1076;": "\u0434", // &dcy;
    "&#8650;": "\u21CA", // &ddarr; &downdownarrows;
    "&#10871;": "\u2A77", // &ddotseq; &eDDot;
    "&#176;": "\u00B0", // &deg &deg;
    "&#948;": "\u03B4", // &delta;
    "&#10673;": "\u29B1", // &demptyv;
    "&#10623;": "\u297F", // &dfisht;
    "&#120097;": "\uD835\uDD21", // &dfr;
    "&#9830;": "\u2666", // &diamondsuit; &diams;
    "&#989;": "\u03DD", // &digamma; &gammad;
    "&#8946;": "\u22F2", // &disin;
    "&#247;": "\u00F7", // &div; &divide &divide;
    "&#8903;": "\u22C7", // &divideontimes; &divonx;
    "&#1106;": "\u0452", // &djcy;
    "&#8990;": "\u231E", // &dlcorn; &llcorner;
    "&#8973;": "\u230D", // &dlcrop;
    "&#36;": "\u0024", // &dollar;
    "&#120149;": "\uD835\uDD55", // &dopf;
    "&#8785;": "\u2251", // &doteqdot; &eDot;
    "&#8760;": "\u2238", // &dotminus; &minusd;
    "&#8724;": "\u2214", // &dotplus; &plusdo;
    "&#8865;": "\u22A1", // &dotsquare; &sdotb;
    "&#8991;": "\u231F", // &drcorn; &lrcorner;
    "&#8972;": "\u230C", // &drcrop;
    "&#119993;": "\uD835\uDCB9", // &dscr;
    "&#1109;": "\u0455", // &dscy;
    "&#10742;": "\u29F6", // &dsol;
    "&#273;": "\u0111", // &dstrok;
    "&#8945;": "\u22F1", // &dtdot;
    "&#9663;": "\u25BF", // &dtri; &triangledown;
    "&#10662;": "\u29A6", // &dwangle;
    "&#1119;": "\u045F", // &dzcy;
    "&#10239;": "\u27FF", // &dzigrarr;
    "&#233;": "\u00E9", // &eacute &eacute;
    "&#10862;": "\u2A6E", // &easter;
    "&#283;": "\u011B", // &ecaron;
    "&#8790;": "\u2256", // &ecir; &eqcirc;
    "&#234;": "\u00EA", // &ecirc &ecirc;
    "&#8789;": "\u2255", // &ecolon; &eqcolon;
    "&#1101;": "\u044D", // &ecy;
    "&#279;": "\u0117", // &edot;
    "&#8786;": "\u2252", // &efDot; &fallingdotseq;
    "&#120098;": "\uD835\uDD22", // &efr;
    "&#10906;": "\u2A9A", // &eg;
    "&#232;": "\u00E8", // &egrave &egrave;
    "&#10902;": "\u2A96", // &egs; &eqslantgtr;
    "&#10904;": "\u2A98", // &egsdot;
    "&#10905;": "\u2A99", // &el;
    "&#9191;": "\u23E7", // &elinters;
    "&#8467;": "\u2113", // &ell;
    "&#10901;": "\u2A95", // &els; &eqslantless;
    "&#10903;": "\u2A97", // &elsdot;
    "&#275;": "\u0113", // &emacr;
    "&#8709;": "\u2205", // &empty; &emptyset; &emptyv; &varnothing;
    "&#8196;": "\u2004", // &emsp13;
    "&#8197;": "\u2005", // &emsp14;
    "&#8195;": "\u2003", // &emsp;
    "&#331;": "\u014B", // &eng;
    "&#8194;": "\u2002", // &ensp;
    "&#281;": "\u0119", // &eogon;
    "&#120150;": "\uD835\uDD56", // &eopf;
    "&#8917;": "\u22D5", // &epar;
    "&#10723;": "\u29E3", // &eparsl;
    "&#10865;": "\u2A71", // &eplus;
    "&#949;": "\u03B5", // &epsi; &epsilon;
    "&#1013;": "\u03F5", // &epsiv; &straightepsilon; &varepsilon;
    "&#8799;": "\u225F", // &equest; &questeq;
    "&#10872;": "\u2A78", // &equivDD;
    "&#10725;": "\u29E5", // &eqvparsl;
    "&#8787;": "\u2253", // &erDot; &risingdotseq;
    "&#10609;": "\u2971", // &erarr;
    "&#8495;": "\u212F", // &escr;
    "&#951;": "\u03B7", // &eta;
    "&#240;": "\u00F0", // &eth &eth;
    "&#235;": "\u00EB", // &euml &euml;
    "&#8364;": "\u20AC", // &euro;
    "&#33;": "\u0021", // &excl;
    "&#1092;": "\u0444", // &fcy;
    "&#9792;": "\u2640", // &female;
    "&#64259;": "\uFB03", // &ffilig;
    "&#64256;": "\uFB00", // &fflig;
    "&#64260;": "\uFB04", // &ffllig;
    "&#120099;": "\uD835\uDD23", // &ffr;
    "&#64257;": "\uFB01", // &filig;
    "&#102;": "\u0066\u006A", // &fjlig;
    "&#9837;": "\u266D", // &flat;
    "&#64258;": "\uFB02", // &fllig;
    "&#9649;": "\u25B1", // &fltns;
    "&#402;": "\u0192", // &fnof;
    "&#120151;": "\uD835\uDD57", // &fopf;
    "&#8916;": "\u22D4", // &fork; &pitchfork;
    "&#10969;": "\u2AD9", // &forkv;
    "&#10765;": "\u2A0D", // &fpartint;
    "&#189;": "\u00BD", // &frac12 &frac12; &half;
    "&#8531;": "\u2153", // &frac13;
    "&#188;": "\u00BC", // &frac14 &frac14;
    "&#8533;": "\u2155", // &frac15;
    "&#8537;": "\u2159", // &frac16;
    "&#8539;": "\u215B", // &frac18;
    "&#8532;": "\u2154", // &frac23;
    "&#8534;": "\u2156", // &frac25;
    "&#190;": "\u00BE", // &frac34 &frac34;
    "&#8535;": "\u2157", // &frac35;
    "&#8540;": "\u215C", // &frac38;
    "&#8536;": "\u2158", // &frac45;
    "&#8538;": "\u215A", // &frac56;
    "&#8541;": "\u215D", // &frac58;
    "&#8542;": "\u215E", // &frac78;
    "&#8260;": "\u2044", // &frasl;
    "&#8994;": "\u2322", // &frown; &sfrown;
    "&#119995;": "\uD835\uDCBB", // &fscr;
    "&#10892;": "\u2A8C", // &gEl; &gtreqqless;
    "&#501;": "\u01F5", // &gacute;
    "&#947;": "\u03B3", // &gamma;
    "&#10886;": "\u2A86", // &gap; &gtrapprox;
    "&#287;": "\u011F", // &gbreve;
    "&#285;": "\u011D", // &gcirc;
    "&#1075;": "\u0433", // &gcy;
    "&#289;": "\u0121", // &gdot;
    "&#10921;": "\u2AA9", // &gescc;
    "&#10880;": "\u2A80", // &gesdot;
    "&#10882;": "\u2A82", // &gesdoto;
    "&#10884;": "\u2A84", // &gesdotol;
    "&#10900;": "\u2A94", // &gesles;
    "&#120100;": "\uD835\uDD24", // &gfr;
    "&#8503;": "\u2137", // &gimel;
    "&#1107;": "\u0453", // &gjcy;
    "&#10898;": "\u2A92", // &glE;
    "&#10917;": "\u2AA5", // &gla;
    "&#10916;": "\u2AA4", // &glj;
    "&#8809;": "\u2269", // &gnE; &gneqq; &gvertneqq; &gvnE;
    "&#10890;": "\u2A8A", // &gnap; &gnapprox;
    "&#10888;": "\u2A88", // &gne; &gneq;
    "&#8935;": "\u22E7", // &gnsim;
    "&#120152;": "\uD835\uDD58", // &gopf;
    "&#8458;": "\u210A", // &gscr;
    "&#10894;": "\u2A8E", // &gsime;
    "&#10896;": "\u2A90", // &gsiml;
    "&#10919;": "\u2AA7", // &gtcc;
    "&#10874;": "\u2A7A", // &gtcir;
    "&#8919;": "\u22D7", // &gtdot; &gtrdot;
    "&#10645;": "\u2995", // &gtlPar;
    "&#10876;": "\u2A7C", // &gtquest;
    "&#10616;": "\u2978", // &gtrarr;
    "&#1098;": "\u044A", // &hardcy;
    "&#10568;": "\u2948", // &harrcir;
    "&#8621;": "\u21AD", // &harrw; &leftrightsquigarrow;
    "&#8463;": "\u210F", // &hbar; &hslash; &planck; &plankv;
    "&#293;": "\u0125", // &hcirc;
    "&#9829;": "\u2665", // &hearts; &heartsuit;
    "&#8230;": "\u2026", // &hellip; &mldr;
    "&#8889;": "\u22B9", // &hercon;
    "&#120101;": "\uD835\uDD25", // &hfr;
    "&#10533;": "\u2925", // &hksearow; &searhk;
    "&#10534;": "\u2926", // &hkswarow; &swarhk;
    "&#8703;": "\u21FF", // &hoarr;
    "&#8763;": "\u223B", // &homtht;
    "&#8617;": "\u21A9", // &hookleftarrow; &larrhk;
    "&#8618;": "\u21AA", // &hookrightarrow; &rarrhk;
    "&#120153;": "\uD835\uDD59", // &hopf;
    "&#8213;": "\u2015", // &horbar;
    "&#119997;": "\uD835\uDCBD", // &hscr;
    "&#295;": "\u0127", // &hstrok;
    "&#8259;": "\u2043", // &hybull;
    "&#237;": "\u00ED", // &iacute &iacute;
    "&#238;": "\u00EE", // &icirc &icirc;
    "&#1080;": "\u0438", // &icy;
    "&#1077;": "\u0435", // &iecy;
    "&#161;": "\u00A1", // &iexcl &iexcl;
    "&#120102;": "\uD835\uDD26", // &ifr;
    "&#236;": "\u00EC", // &igrave &igrave;
    "&#10764;": "\u2A0C", // &iiiint; &qint;
    "&#8749;": "\u222D", // &iiint; &tint;
    "&#10716;": "\u29DC", // &iinfin;
    "&#8489;": "\u2129", // &iiota;
    "&#307;": "\u0133", // &ijlig;
    "&#299;": "\u012B", // &imacr;
    "&#305;": "\u0131", // &imath; &inodot;
    "&#8887;": "\u22B7", // &imof;
    "&#437;": "\u01B5", // &imped;
    "&#8453;": "\u2105", // &incare;
    "&#8734;": "\u221E", // &infin;
    "&#10717;": "\u29DD", // &infintie;
    "&#8890;": "\u22BA", // &intcal; &intercal;
    "&#10775;": "\u2A17", // &intlarhk;
    "&#10812;": "\u2A3C", // &intprod; &iprod;
    "&#1105;": "\u0451", // &iocy;
    "&#303;": "\u012F", // &iogon;
    "&#120154;": "\uD835\uDD5A", // &iopf;
    "&#953;": "\u03B9", // &iota;
    "&#191;": "\u00BF", // &iquest &iquest;
    "&#119998;": "\uD835\uDCBE", // &iscr;
    "&#8953;": "\u22F9", // &isinE; &notinE;
    "&#8949;": "\u22F5", // &isindot; &notindot;
    "&#8948;": "\u22F4", // &isins;
    "&#8947;": "\u22F3", // &isinsv;
    "&#297;": "\u0129", // &itilde;
    "&#1110;": "\u0456", // &iukcy;
    "&#239;": "\u00EF", // &iuml &iuml;
    "&#309;": "\u0135", // &jcirc;
    "&#1081;": "\u0439", // &jcy;
    "&#120103;": "\uD835\uDD27", // &jfr;
    "&#567;": "\u0237", // &jmath;
    "&#120155;": "\uD835\uDD5B", // &jopf;
    "&#119999;": "\uD835\uDCBF", // &jscr;
    "&#1112;": "\u0458", // &jsercy;
    "&#1108;": "\u0454", // &jukcy;
    "&#954;": "\u03BA", // &kappa;
    "&#1008;": "\u03F0", // &kappav; &varkappa;
    "&#311;": "\u0137", // &kcedil;
    "&#1082;": "\u043A", // &kcy;
    "&#120104;": "\uD835\uDD28", // &kfr;
    "&#312;": "\u0138", // &kgreen;
    "&#1093;": "\u0445", // &khcy;
    "&#1116;": "\u045C", // &kjcy;
    "&#120156;": "\uD835\uDD5C", // &kopf;
    "&#120000;": "\uD835\uDCC0", // &kscr;
    "&#10523;": "\u291B", // &lAtail;
    "&#10510;": "\u290E", // &lBarr;
    "&#10891;": "\u2A8B", // &lEg; &lesseqqgtr;
    "&#10594;": "\u2962", // &lHar;
    "&#314;": "\u013A", // &lacute;
    "&#10676;": "\u29B4", // &laemptyv;
    "&#955;": "\u03BB", // &lambda;
    "&#10641;": "\u2991", // &langd;
    "&#10885;": "\u2A85", // &lap; &lessapprox;
    "&#171;": "\u00AB", // &laquo &laquo;
    "&#10527;": "\u291F", // &larrbfs;
    "&#10525;": "\u291D", // &larrfs;
    "&#8619;": "\u21AB", // &larrlp; &looparrowleft;
    "&#10553;": "\u2939", // &larrpl;
    "&#10611;": "\u2973", // &larrsim;
    "&#8610;": "\u21A2", // &larrtl; &leftarrowtail;
    "&#10923;": "\u2AAB", // &lat;
    "&#10521;": "\u2919", // &latail;
    "&#10925;": "\u2AAD", // &late; &lates;
    "&#10508;": "\u290C", // &lbarr;
    "&#10098;": "\u2772", // &lbbrk;
    "&#123;": "\u007B", // &lbrace; &lcub;
    "&#91;": "\u005B", // &lbrack; &lsqb;
    "&#10635;": "\u298B", // &lbrke;
    "&#10639;": "\u298F", // &lbrksld;
    "&#10637;": "\u298D", // &lbrkslu;
    "&#318;": "\u013E", // &lcaron;
    "&#316;": "\u013C", // &lcedil;
    "&#1083;": "\u043B", // &lcy;
    "&#10550;": "\u2936", // &ldca;
    "&#10599;": "\u2967", // &ldrdhar;
    "&#10571;": "\u294B", // &ldrushar;
    "&#8626;": "\u21B2", // &ldsh;
    "&#8804;": "\u2264", // &le; &leq; &nvle;
    "&#8647;": "\u21C7", // &leftleftarrows; &llarr;
    "&#8907;": "\u22CB", // &leftthreetimes; &lthree;
    "&#10920;": "\u2AA8", // &lescc;
    "&#10879;": "\u2A7F", // &lesdot;
    "&#10881;": "\u2A81", // &lesdoto;
    "&#10883;": "\u2A83", // &lesdotor;
    "&#10899;": "\u2A93", // &lesges;
    "&#8918;": "\u22D6", // &lessdot; &ltdot;
    "&#10620;": "\u297C", // &lfisht;
    "&#120105;": "\uD835\uDD29", // &lfr;
    "&#10897;": "\u2A91", // &lgE;
    "&#10602;": "\u296A", // &lharul;
    "&#9604;": "\u2584", // &lhblk;
    "&#1113;": "\u0459", // &ljcy;
    "&#10603;": "\u296B", // &llhard;
    "&#9722;": "\u25FA", // &lltri;
    "&#320;": "\u0140", // &lmidot;
    "&#9136;": "\u23B0", // &lmoust; &lmoustache;
    "&#8808;": "\u2268", // &lnE; &lneqq; &lvertneqq; &lvnE;
    "&#10889;": "\u2A89", // &lnap; &lnapprox;
    "&#10887;": "\u2A87", // &lne; &lneq;
    "&#8934;": "\u22E6", // &lnsim;
    "&#10220;": "\u27EC", // &loang;
    "&#8701;": "\u21FD", // &loarr;
    "&#10236;": "\u27FC", // &longmapsto; &xmap;
    "&#8620;": "\u21AC", // &looparrowright; &rarrlp;
    "&#10629;": "\u2985", // &lopar;
    "&#120157;": "\uD835\uDD5D", // &lopf;
    "&#10797;": "\u2A2D", // &loplus;
    "&#10804;": "\u2A34", // &lotimes;
    "&#8727;": "\u2217", // &lowast;
    "&#9674;": "\u25CA", // &loz; &lozenge;
    "&#40;": "\u0028", // &lpar;
    "&#10643;": "\u2993", // &lparlt;
    "&#10605;": "\u296D", // &lrhard;
    "&#8206;": "\u200E", // &lrm;
    "&#8895;": "\u22BF", // &lrtri;
    "&#8249;": "\u2039", // &lsaquo;
    "&#120001;": "\uD835\uDCC1", // &lscr;
    "&#10893;": "\u2A8D", // &lsime;
    "&#10895;": "\u2A8F", // &lsimg;
    "&#8218;": "\u201A", // &lsquor; &sbquo;
    "&#322;": "\u0142", // &lstrok;
    "&#10918;": "\u2AA6", // &ltcc;
    "&#10873;": "\u2A79", // &ltcir;
    "&#8905;": "\u22C9", // &ltimes;
    "&#10614;": "\u2976", // &ltlarr;
    "&#10875;": "\u2A7B", // &ltquest;
    "&#10646;": "\u2996", // &ltrPar;
    "&#9667;": "\u25C3", // &ltri; &triangleleft;
    "&#10570;": "\u294A", // &lurdshar;
    "&#10598;": "\u2966", // &luruhar;
    "&#8762;": "\u223A", // &mDDot;
    "&#175;": "\u00AF", // &macr &macr; &strns;
    "&#9794;": "\u2642", // &male;
    "&#10016;": "\u2720", // &malt; &maltese;
    "&#9646;": "\u25AE", // &marker;
    "&#10793;": "\u2A29", // &mcomma;
    "&#1084;": "\u043C", // &mcy;
    "&#8212;": "\u2014", // &mdash;
    "&#120106;": "\uD835\uDD2A", // &mfr;
    "&#8487;": "\u2127", // &mho;
    "&#181;": "\u00B5", // &micro &micro;
    "&#10992;": "\u2AF0", // &midcir;
    "&#8722;": "\u2212", // &minus;
    "&#10794;": "\u2A2A", // &minusdu;
    "&#10971;": "\u2ADB", // &mlcp;
    "&#8871;": "\u22A7", // &models;
    "&#120158;": "\uD835\uDD5E", // &mopf;
    "&#120002;": "\uD835\uDCC2", // &mscr;
    "&#956;": "\u03BC", // &mu;
    "&#8888;": "\u22B8", // &multimap; &mumap;
    "&#8653;": "\u21CD", // &nLeftarrow; &nlArr;
    "&#8654;": "\u21CE", // &nLeftrightarrow; &nhArr;
    "&#8655;": "\u21CF", // &nRightarrow; &nrArr;
    "&#8879;": "\u22AF", // &nVDash;
    "&#8878;": "\u22AE", // &nVdash;
    "&#324;": "\u0144", // &nacute;
    "&#329;": "\u0149", // &napos;
    "&#9838;": "\u266E", // &natur; &natural;
    "&#10819;": "\u2A43", // &ncap;
    "&#328;": "\u0148", // &ncaron;
    "&#326;": "\u0146", // &ncedil;
    "&#10818;": "\u2A42", // &ncup;
    "&#1085;": "\u043D", // &ncy;
    "&#8211;": "\u2013", // &ndash;
    "&#8663;": "\u21D7", // &neArr;
    "&#10532;": "\u2924", // &nearhk;
    "&#10536;": "\u2928", // &nesear; &toea;
    "&#120107;": "\uD835\uDD2B", // &nfr;
    "&#8622;": "\u21AE", // &nharr; &nleftrightarrow;
    "&#10994;": "\u2AF2", // &nhpar;
    "&#8956;": "\u22FC", // &nis;
    "&#8954;": "\u22FA", // &nisd;
    "&#1114;": "\u045A", // &njcy;
    "&#8602;": "\u219A", // &nlarr; &nleftarrow;
    "&#8229;": "\u2025", // &nldr;
    "&#120159;": "\uD835\uDD5F", // &nopf;
    "&#172;": "\u00AC", // &not &not;
    "&#8951;": "\u22F7", // &notinvb;
    "&#8950;": "\u22F6", // &notinvc;
    "&#8958;": "\u22FE", // &notnivb;
    "&#8957;": "\u22FD", // &notnivc;
    "&#11005;": "\u2AFD\u20E5", // &nparsl; &parsl;
    "&#10772;": "\u2A14", // &npolint;
    "&#8603;": "\u219B", // &nrarr; &nrightarrow;
    "&#10547;": "\u2933\u0338", // &nrarrc; &rarrc;
    "&#8605;": "\u219D\u0338", // &nrarrw; &rarrw; &rightsquigarrow;
    "&#120003;": "\uD835\uDCC3", // &nscr;
    "&#8836;": "\u2284", // &nsub;
    "&#10949;": "\u2AC5\u0338", // &nsubE; &nsubseteqq; &subE; &subseteqq;
    "&#8837;": "\u2285", // &nsup;
    "&#10950;": "\u2AC6\u0338", // &nsupE; &nsupseteqq; &supE; &supseteqq;
    "&#241;": "\u00F1", // &ntilde &ntilde;
    "&#957;": "\u03BD", // &nu;
    "&#35;": "\u0023", // &num;
    "&#8470;": "\u2116", // &numero;
    "&#8199;": "\u2007", // &numsp;
    "&#8877;": "\u22AD", // &nvDash;
    "&#10500;": "\u2904", // &nvHarr;
    "&#8876;": "\u22AC", // &nvdash;
    "&#10718;": "\u29DE", // &nvinfin;
    "&#10498;": "\u2902", // &nvlArr;
    "&#10499;": "\u2903", // &nvrArr;
    "&#8662;": "\u21D6", // &nwArr;
    "&#10531;": "\u2923", // &nwarhk;
    "&#10535;": "\u2927", // &nwnear;
    "&#243;": "\u00F3", // &oacute &oacute;
    "&#244;": "\u00F4", // &ocirc &ocirc;
    "&#1086;": "\u043E", // &ocy;
    "&#337;": "\u0151", // &odblac;
    "&#10808;": "\u2A38", // &odiv;
    "&#10684;": "\u29BC", // &odsold;
    "&#339;": "\u0153", // &oelig;
    "&#10687;": "\u29BF", // &ofcir;
    "&#120108;": "\uD835\uDD2C", // &ofr;
    "&#731;": "\u02DB", // &ogon;
    "&#242;": "\u00F2", // &ograve &ograve;
    "&#10689;": "\u29C1", // &ogt;
    "&#10677;": "\u29B5", // &ohbar;
    "&#10686;": "\u29BE", // &olcir;
    "&#10683;": "\u29BB", // &olcross;
    "&#10688;": "\u29C0", // &olt;
    "&#333;": "\u014D", // &omacr;
    "&#969;": "\u03C9", // &omega;
    "&#959;": "\u03BF", // &omicron;
    "&#10678;": "\u29B6", // &omid;
    "&#120160;": "\uD835\uDD60", // &oopf;
    "&#10679;": "\u29B7", // &opar;
    "&#10681;": "\u29B9", // &operp;
    "&#8744;": "\u2228", // &or; &vee;
    "&#10845;": "\u2A5D", // &ord;
    "&#8500;": "\u2134", // &order; &orderof; &oscr;
    "&#170;": "\u00AA", // &ordf &ordf;
    "&#186;": "\u00BA", // &ordm &ordm;
    "&#8886;": "\u22B6", // &origof;
    "&#10838;": "\u2A56", // &oror;
    "&#10839;": "\u2A57", // &orslope;
    "&#10843;": "\u2A5B", // &orv;
    "&#248;": "\u00F8", // &oslash &oslash;
    "&#8856;": "\u2298", // &osol;
    "&#245;": "\u00F5", // &otilde &otilde;
    "&#10806;": "\u2A36", // &otimesas;
    "&#246;": "\u00F6", // &ouml &ouml;
    "&#9021;": "\u233D", // &ovbar;
    "&#182;": "\u00B6", // &para &para;
    "&#10995;": "\u2AF3", // &parsim;
    "&#1087;": "\u043F", // &pcy;
    "&#37;": "\u0025", // &percnt;
    "&#46;": "\u002E", // &period;
    "&#8240;": "\u2030", // &permil;
    "&#8241;": "\u2031", // &pertenk;
    "&#120109;": "\uD835\uDD2D", // &pfr;
    "&#966;": "\u03C6", // &phi;
    "&#981;": "\u03D5", // &phiv; &straightphi; &varphi;
    "&#9742;": "\u260E", // &phone;
    "&#960;": "\u03C0", // &pi;
    "&#982;": "\u03D6", // &piv; &varpi;
    "&#8462;": "\u210E", // &planckh;
    "&#43;": "\u002B", // &plus;
    "&#10787;": "\u2A23", // &plusacir;
    "&#10786;": "\u2A22", // &pluscir;
    "&#10789;": "\u2A25", // &plusdu;
    "&#10866;": "\u2A72", // &pluse;
    "&#10790;": "\u2A26", // &plussim;
    "&#10791;": "\u2A27", // &plustwo;
    "&#10773;": "\u2A15", // &pointint;
    "&#120161;": "\uD835\uDD61", // &popf;
    "&#163;": "\u00A3", // &pound &pound;
    "&#10931;": "\u2AB3", // &prE;
    "&#10935;": "\u2AB7", // &prap; &precapprox;
    "&#10937;": "\u2AB9", // &precnapprox; &prnap;
    "&#10933;": "\u2AB5", // &precneqq; &prnE;
    "&#8936;": "\u22E8", // &precnsim; &prnsim;
    "&#8242;": "\u2032", // &prime;
    "&#9006;": "\u232E", // &profalar;
    "&#8978;": "\u2312", // &profline;
    "&#8979;": "\u2313", // &profsurf;
    "&#8880;": "\u22B0", // &prurel;
    "&#120005;": "\uD835\uDCC5", // &pscr;
    "&#968;": "\u03C8", // &psi;
    "&#8200;": "\u2008", // &puncsp;
    "&#120110;": "\uD835\uDD2E", // &qfr;
    "&#120162;": "\uD835\uDD62", // &qopf;
    "&#8279;": "\u2057", // &qprime;
    "&#120006;": "\uD835\uDCC6", // &qscr;
    "&#10774;": "\u2A16", // &quatint;
    "&#63;": "\u003F", // &quest;
    "&#10524;": "\u291C", // &rAtail;
    "&#10596;": "\u2964", // &rHar;
    "&#341;": "\u0155", // &racute;
    "&#10675;": "\u29B3", // &raemptyv;
    "&#10642;": "\u2992", // &rangd;
    "&#10661;": "\u29A5", // &range;
    "&#187;": "\u00BB", // &raquo &raquo;
    "&#10613;": "\u2975", // &rarrap;
    "&#10528;": "\u2920", // &rarrbfs;
    "&#10526;": "\u291E", // &rarrfs;
    "&#10565;": "\u2945", // &rarrpl;
    "&#10612;": "\u2974", // &rarrsim;
    "&#8611;": "\u21A3", // &rarrtl; &rightarrowtail;
    "&#10522;": "\u291A", // &ratail;
    "&#8758;": "\u2236", // &ratio;
    "&#10099;": "\u2773", // &rbbrk;
    "&#125;": "\u007D", // &rbrace; &rcub;
    "&#93;": "\u005D", // &rbrack; &rsqb;
    "&#10636;": "\u298C", // &rbrke;
    "&#10638;": "\u298E", // &rbrksld;
    "&#10640;": "\u2990", // &rbrkslu;
    "&#345;": "\u0159", // &rcaron;
    "&#343;": "\u0157", // &rcedil;
    "&#1088;": "\u0440", // &rcy;
    "&#10551;": "\u2937", // &rdca;
    "&#10601;": "\u2969", // &rdldhar;
    "&#8627;": "\u21B3", // &rdsh;
    "&#9645;": "\u25AD", // &rect;
    "&#10621;": "\u297D", // &rfisht;
    "&#120111;": "\uD835\uDD2F", // &rfr;
    "&#10604;": "\u296C", // &rharul;
    "&#961;": "\u03C1", // &rho;
    "&#1009;": "\u03F1", // &rhov; &varrho;
    "&#8649;": "\u21C9", // &rightrightarrows; &rrarr;
    "&#8908;": "\u22CC", // &rightthreetimes; &rthree;
    "&#730;": "\u02DA", // &ring;
    "&#8207;": "\u200F", // &rlm;
    "&#9137;": "\u23B1", // &rmoust; &rmoustache;
    "&#10990;": "\u2AEE", // &rnmid;
    "&#10221;": "\u27ED", // &roang;
    "&#8702;": "\u21FE", // &roarr;
    "&#10630;": "\u2986", // &ropar;
    "&#120163;": "\uD835\uDD63", // &ropf;
    "&#10798;": "\u2A2E", // &roplus;
    "&#10805;": "\u2A35", // &rotimes;
    "&#41;": "\u0029", // &rpar;
    "&#10644;": "\u2994", // &rpargt;
    "&#10770;": "\u2A12", // &rppolint;
    "&#8250;": "\u203A", // &rsaquo;
    "&#120007;": "\uD835\uDCC7", // &rscr;
    "&#8906;": "\u22CA", // &rtimes;
    "&#9657;": "\u25B9", // &rtri; &triangleright;
    "&#10702;": "\u29CE", // &rtriltri;
    "&#10600;": "\u2968", // &ruluhar;
    "&#8478;": "\u211E", // &rx;
    "&#347;": "\u015B", // &sacute;
    "&#10932;": "\u2AB4", // &scE;
    "&#10936;": "\u2AB8", // &scap; &succapprox;
    "&#353;": "\u0161", // &scaron;
    "&#351;": "\u015F", // &scedil;
    "&#349;": "\u015D", // &scirc;
    "&#10934;": "\u2AB6", // &scnE; &succneqq;
    "&#10938;": "\u2ABA", // &scnap; &succnapprox;
    "&#8937;": "\u22E9", // &scnsim; &succnsim;
    "&#10771;": "\u2A13", // &scpolint;
    "&#1089;": "\u0441", // &scy;
    "&#8901;": "\u22C5", // &sdot;
    "&#10854;": "\u2A66", // &sdote;
    "&#8664;": "\u21D8", // &seArr;
    "&#167;": "\u00A7", // &sect &sect;
    "&#59;": "\u003B", // &semi;
    "&#10537;": "\u2929", // &seswar; &tosa;
    "&#10038;": "\u2736", // &sext;
    "&#120112;": "\uD835\uDD30", // &sfr;
    "&#9839;": "\u266F", // &sharp;
    "&#1097;": "\u0449", // &shchcy;
    "&#1096;": "\u0448", // &shcy;
    "&#173;": "\u00AD", // &shy &shy;
    "&#963;": "\u03C3", // &sigma;
    "&#962;": "\u03C2", // &sigmaf; &sigmav; &varsigma;
    "&#10858;": "\u2A6A", // &simdot;
    "&#10910;": "\u2A9E", // &simg;
    "&#10912;": "\u2AA0", // &simgE;
    "&#10909;": "\u2A9D", // &siml;
    "&#10911;": "\u2A9F", // &simlE;
    "&#8774;": "\u2246", // &simne;
    "&#10788;": "\u2A24", // &simplus;
    "&#10610;": "\u2972", // &simrarr;
    "&#10803;": "\u2A33", // &smashp;
    "&#10724;": "\u29E4", // &smeparsl;
    "&#8995;": "\u2323", // &smile; &ssmile;
    "&#10922;": "\u2AAA", // &smt;
    "&#10924;": "\u2AAC", // &smte; &smtes;
    "&#1100;": "\u044C", // &softcy;
    "&#47;": "\u002F", // &sol;
    "&#10692;": "\u29C4", // &solb;
    "&#9023;": "\u233F", // &solbar;
    "&#120164;": "\uD835\uDD64", // &sopf;
    "&#9824;": "\u2660", // &spades; &spadesuit;
    "&#120008;": "\uD835\uDCC8", // &sscr;
    "&#9734;": "\u2606", // &star;
    "&#10941;": "\u2ABD", // &subdot;
    "&#10947;": "\u2AC3", // &subedot;
    "&#10945;": "\u2AC1", // &submult;
    "&#10955;": "\u2ACB", // &subnE; &subsetneqq; &varsubsetneqq; &vsubnE;
    "&#8842;": "\u228A", // &subne; &subsetneq; &varsubsetneq; &vsubne;
    "&#10943;": "\u2ABF", // &subplus;
    "&#10617;": "\u2979", // &subrarr;
    "&#10951;": "\u2AC7", // &subsim;
    "&#10965;": "\u2AD5", // &subsub;
    "&#10963;": "\u2AD3", // &subsup;
    "&#9834;": "\u266A", // &sung;
    "&#185;": "\u00B9", // &sup1 &sup1;
    "&#178;": "\u00B2", // &sup2 &sup2;
    "&#179;": "\u00B3", // &sup3 &sup3;
    "&#10942;": "\u2ABE", // &supdot;
    "&#10968;": "\u2AD8", // &supdsub;
    "&#10948;": "\u2AC4", // &supedot;
    "&#10185;": "\u27C9", // &suphsol;
    "&#10967;": "\u2AD7", // &suphsub;
    "&#10619;": "\u297B", // &suplarr;
    "&#10946;": "\u2AC2", // &supmult;
    "&#10956;": "\u2ACC", // &supnE; &supsetneqq; &varsupsetneqq; &vsupnE;
    "&#8843;": "\u228B", // &supne; &supsetneq; &varsupsetneq; &vsupne;
    "&#10944;": "\u2AC0", // &supplus;
    "&#10952;": "\u2AC8", // &supsim;
    "&#10964;": "\u2AD4", // &supsub;
    "&#10966;": "\u2AD6", // &supsup;
    "&#8665;": "\u21D9", // &swArr;
    "&#10538;": "\u292A", // &swnwar;
    "&#223;": "\u00DF", // &szlig &szlig;
    "&#8982;": "\u2316", // &target;
    "&#964;": "\u03C4", // &tau;
    "&#357;": "\u0165", // &tcaron;
    "&#355;": "\u0163", // &tcedil;
    "&#1090;": "\u0442", // &tcy;
    "&#8981;": "\u2315", // &telrec;
    "&#120113;": "\uD835\uDD31", // &tfr;
    "&#952;": "\u03B8", // &theta;
    "&#977;": "\u03D1", // &thetasym; &thetav; &vartheta;
    "&#254;": "\u00FE", // &thorn &thorn;
    "&#215;": "\u00D7", // &times &times;
    "&#10801;": "\u2A31", // &timesbar;
    "&#10800;": "\u2A30", // &timesd;
    "&#9014;": "\u2336", // &topbot;
    "&#10993;": "\u2AF1", // &topcir;
    "&#120165;": "\uD835\uDD65", // &topf;
    "&#10970;": "\u2ADA", // &topfork;
    "&#8244;": "\u2034", // &tprime;
    "&#9653;": "\u25B5", // &triangle; &utri;
    "&#8796;": "\u225C", // &triangleq; &trie;
    "&#9708;": "\u25EC", // &tridot;
    "&#10810;": "\u2A3A", // &triminus;
    "&#10809;": "\u2A39", // &triplus;
    "&#10701;": "\u29CD", // &trisb;
    "&#10811;": "\u2A3B", // &tritime;
    "&#9186;": "\u23E2", // &trpezium;
    "&#120009;": "\uD835\uDCC9", // &tscr;
    "&#1094;": "\u0446", // &tscy;
    "&#1115;": "\u045B", // &tshcy;
    "&#359;": "\u0167", // &tstrok;
    "&#10595;": "\u2963", // &uHar;
    "&#250;": "\u00FA", // &uacute &uacute;
    "&#1118;": "\u045E", // &ubrcy;
    "&#365;": "\u016D", // &ubreve;
    "&#251;": "\u00FB", // &ucirc &ucirc;
    "&#1091;": "\u0443", // &ucy;
    "&#369;": "\u0171", // &udblac;
    "&#10622;": "\u297E", // &ufisht;
    "&#120114;": "\uD835\uDD32", // &ufr;
    "&#249;": "\u00F9", // &ugrave &ugrave;
    "&#9600;": "\u2580", // &uhblk;
    "&#8988;": "\u231C", // &ulcorn; &ulcorner;
    "&#8975;": "\u230F", // &ulcrop;
    "&#9720;": "\u25F8", // &ultri;
    "&#363;": "\u016B", // &umacr;
    "&#371;": "\u0173", // &uogon;
    "&#120166;": "\uD835\uDD66", // &uopf;
    "&#965;": "\u03C5", // &upsi; &upsilon;
    "&#8648;": "\u21C8", // &upuparrows; &uuarr;
    "&#8989;": "\u231D", // &urcorn; &urcorner;
    "&#8974;": "\u230E", // &urcrop;
    "&#367;": "\u016F", // &uring;
    "&#9721;": "\u25F9", // &urtri;
    "&#120010;": "\uD835\uDCCA", // &uscr;
    "&#8944;": "\u22F0", // &utdot;
    "&#361;": "\u0169", // &utilde;
    "&#252;": "\u00FC", // &uuml &uuml;
    "&#10663;": "\u29A7", // &uwangle;
    "&#10984;": "\u2AE8", // &vBar;
    "&#10985;": "\u2AE9", // &vBarv;
    "&#10652;": "\u299C", // &vangrt;
    "&#1074;": "\u0432", // &vcy;
    "&#8891;": "\u22BB", // &veebar;
    "&#8794;": "\u225A", // &veeeq;
    "&#8942;": "\u22EE", // &vellip;
    "&#120115;": "\uD835\uDD33", // &vfr;
    "&#120167;": "\uD835\uDD67", // &vopf;
    "&#120011;": "\uD835\uDCCB", // &vscr;
    "&#10650;": "\u299A", // &vzigzag;
    "&#373;": "\u0175", // &wcirc;
    "&#10847;": "\u2A5F", // &wedbar;
    "&#8793;": "\u2259", // &wedgeq;
    "&#8472;": "\u2118", // &weierp; &wp;
    "&#120116;": "\uD835\uDD34", // &wfr;
    "&#120168;": "\uD835\uDD68", // &wopf;
    "&#120012;": "\uD835\uDCCC", // &wscr;
    "&#120117;": "\uD835\uDD35", // &xfr;
    "&#958;": "\u03BE", // &xi;
    "&#8955;": "\u22FB", // &xnis;
    "&#120169;": "\uD835\uDD69", // &xopf;
    "&#120013;": "\uD835\uDCCD", // &xscr;
    "&#253;": "\u00FD", // &yacute &yacute;
    "&#1103;": "\u044F", // &yacy;
    "&#375;": "\u0177", // &ycirc;
    "&#1099;": "\u044B", // &ycy;
    "&#165;": "\u00A5", // &yen &yen;
    "&#120118;": "\uD835\uDD36", // &yfr;
    "&#1111;": "\u0457", // &yicy;
    "&#120170;": "\uD835\uDD6A", // &yopf;
    "&#120014;": "\uD835\uDCCE", // &yscr;
    "&#1102;": "\u044E", // &yucy;
    "&#255;": "\u00FF", // &yuml &yuml;
    "&#378;": "\u017A", // &zacute;
    "&#382;": "\u017E", // &zcaron;
    "&#1079;": "\u0437", // &zcy;
    "&#380;": "\u017C", // &zdot;
    "&#950;": "\u03B6", // &zeta;
    "&#120119;": "\uD835\uDD37", // &zfr;
    "&#1078;": "\u0436", // &zhcy;
    "&#8669;": "\u21DD", // &zigrarr;
    "&#120171;": "\uD835\uDD6B", // &zopf;
    "&#120015;": "\uD835\uDCCF", // &zscr;
    "&#8205;": "\u200D", // &zwj;
    "&#8204;": "\u200C", // &zwnj;
};

// HTML entity(name) -> Character
// about this map: https://html.spec.whatwg.org/multipage/named-characters.html
const html_entity_name_normal_character_maps: Record<string, string> = {
    "&AElig": "\u00C6",
    "&AElig;": "\u00C6",
    "&AMP": "\u0026",
    "&AMP;": "\u0026",
    "&Aacute": "\u00C1",
    "&Aacute;": "\u00C1",
    "&Abreve;": "\u0102",
    "&Acirc": "\u00C2",
    "&Acirc;": "\u00C2",
    "&Acy;": "\u0410",
    "&Afr;": "\uD835\uDD04",
    "&Agrave": "\u00C0",
    "&Agrave;": "\u00C0",
    "&Alpha;": "\u0391",
    "&Amacr;": "\u0100",
    "&And;": "\u2A53",
    "&Aogon;": "\u0104",
    "&Aopf;": "\uD835\uDD38",
    "&ApplyFunction;": "\u2061",
    "&Aring": "\u00C5",
    "&Aring;": "\u00C5",
    "&Ascr;": "\uD835\uDC9C",
    "&Assign;": "\u2254",
    "&Atilde": "\u00C3",
    "&Atilde;": "\u00C3",
    "&Auml": "\u00C4",
    "&Auml;": "\u00C4",
    "&Backslash;": "\u2216",
    "&Barv;": "\u2AE7",
    "&Barwed;": "\u2306",
    "&Bcy;": "\u0411",
    "&Because;": "\u2235",
    "&Bernoullis;": "\u212C",
    "&Beta;": "\u0392",
    "&Bfr;": "\uD835\uDD05",
    "&Bopf;": "\uD835\uDD39",
    "&Breve;": "\u02D8",
    "&Bscr;": "\u212C",
    "&Bumpeq;": "\u224E",
    "&CHcy;": "\u0427",
    "&COPY": "\u00A9",
    "&COPY;": "\u00A9",
    "&Cacute;": "\u0106",
    "&Cap;": "\u22D2",
    "&CapitalDifferentialD;": "\u2145",
    "&Cayleys;": "\u212D",
    "&Ccaron;": "\u010C",
    "&Ccedil": "\u00C7",
    "&Ccedil;": "\u00C7",
    "&Ccirc;": "\u0108",
    "&Cconint;": "\u2230",
    "&Cdot;": "\u010A",
    "&Cedilla;": "\u00B8",
    "&CenterDot;": "\u00B7",
    "&Cfr;": "\u212D",
    "&Chi;": "\u03A7",
    "&CircleDot;": "\u2299",
    "&CircleMinus;": "\u2296",
    "&CirclePlus;": "\u2295",
    "&CircleTimes;": "\u2297",
    "&ClockwiseContourIntegral;": "\u2232",
    "&CloseCurlyDoubleQuote;": "\u201D",
    "&CloseCurlyQuote;": "\u2019",
    "&Colon;": "\u2237",
    "&Colone;": "\u2A74",
    "&Congruent;": "\u2261",
    "&Conint;": "\u222F",
    "&ContourIntegral;": "\u222E",
    "&Copf;": "\u2102",
    "&Coproduct;": "\u2210",
    "&CounterClockwiseContourIntegral;": "\u2233",
    "&Cross;": "\u2A2F",
    "&Cscr;": "\uD835\uDC9E",
    "&Cup;": "\u22D3",
    "&CupCap;": "\u224D",
    "&DD;": "\u2145",
    "&DDotrahd;": "\u2911",
    "&DJcy;": "\u0402",
    "&DScy;": "\u0405",
    "&DZcy;": "\u040F",
    "&Dagger;": "\u2021",
    "&Darr;": "\u21A1",
    "&Dashv;": "\u2AE4",
    "&Dcaron;": "\u010E",
    "&Dcy;": "\u0414",
    "&Del;": "\u2207",
    "&Delta;": "\u0394",
    "&Dfr;": "\uD835\uDD07",
    "&DiacriticalAcute;": "\u00B4",
    "&DiacriticalDot;": "\u02D9",
    "&DiacriticalDoubleAcute;": "\u02DD",
    "&DiacriticalGrave;": "\u0060",
    "&DiacriticalTilde;": "\u02DC",
    "&Diamond;": "\u22C4",
    "&DifferentialD;": "\u2146",
    "&Dopf;": "\uD835\uDD3B",
    "&Dot;": "\u00A8",
    "&DotDot;": "\u20DC",
    "&DotEqual;": "\u2250",
    "&DoubleContourIntegral;": "\u222F",
    "&DoubleDot;": "\u00A8",
    "&DoubleDownArrow;": "\u21D3",
    "&DoubleLeftArrow;": "\u21D0",
    "&DoubleLeftRightArrow;": "\u21D4",
    "&DoubleLeftTee;": "\u2AE4",
    "&DoubleLongLeftArrow;": "\u27F8",
    "&DoubleLongLeftRightArrow;": "\u27FA",
    "&DoubleLongRightArrow;": "\u27F9",
    "&DoubleRightArrow;": "\u21D2",
    "&DoubleRightTee;": "\u22A8",
    "&DoubleUpArrow;": "\u21D1",
    "&DoubleUpDownArrow;": "\u21D5",
    "&DoubleVerticalBar;": "\u2225",
    "&DownArrow;": "\u2193",
    "&DownArrowBar;": "\u2913",
    "&DownArrowUpArrow;": "\u21F5",
    "&DownBreve;": "\u0311",
    "&DownLeftRightVector;": "\u2950",
    "&DownLeftTeeVector;": "\u295E",
    "&DownLeftVector;": "\u21BD",
    "&DownLeftVectorBar;": "\u2956",
    "&DownRightTeeVector;": "\u295F",
    "&DownRightVector;": "\u21C1",
    "&DownRightVectorBar;": "\u2957",
    "&DownTee;": "\u22A4",
    "&DownTeeArrow;": "\u21A7",
    "&Downarrow;": "\u21D3",
    "&Dscr;": "\uD835\uDC9F",
    "&Dstrok;": "\u0110",
    "&ENG;": "\u014A",
    "&ETH": "\u00D0",
    "&ETH;": "\u00D0",
    "&Eacute": "\u00C9",
    "&Eacute;": "\u00C9",
    "&Ecaron;": "\u011A",
    "&Ecirc": "\u00CA",
    "&Ecirc;": "\u00CA",
    "&Ecy;": "\u042D",
    "&Edot;": "\u0116",
    "&Efr;": "\uD835\uDD08",
    "&Egrave": "\u00C8",
    "&Egrave;": "\u00C8",
    "&Element;": "\u2208",
    "&Emacr;": "\u0112",
    "&EmptySmallSquare;": "\u25FB",
    "&EmptyVerySmallSquare;": "\u25AB",
    "&Eogon;": "\u0118",
    "&Eopf;": "\uD835\uDD3C",
    "&Epsilon;": "\u0395",
    "&Equal;": "\u2A75",
    "&EqualTilde;": "\u2242",
    "&Equilibrium;": "\u21CC",
    "&Escr;": "\u2130",
    "&Esim;": "\u2A73",
    "&Eta;": "\u0397",
    "&Euml": "\u00CB",
    "&Euml;": "\u00CB",
    "&Exists;": "\u2203",
    "&ExponentialE;": "\u2147",
    "&Fcy;": "\u0424",
    "&Ffr;": "\uD835\uDD09",
    "&FilledSmallSquare;": "\u25FC",
    "&FilledVerySmallSquare;": "\u25AA",
    "&Fopf;": "\uD835\uDD3D",
    "&ForAll;": "\u2200",
    "&Fouriertrf;": "\u2131",
    "&Fscr;": "\u2131",
    "&GJcy;": "\u0403",
    "&GT": "\u003E",
    "&GT;": "\u003E",
    "&Gamma;": "\u0393",
    "&Gammad;": "\u03DC",
    "&Gbreve;": "\u011E",
    "&Gcedil;": "\u0122",
    "&Gcirc;": "\u011C",
    "&Gcy;": "\u0413",
    "&Gdot;": "\u0120",
    "&Gfr;": "\uD835\uDD0A",
    "&Gg;": "\u22D9",
    "&Gopf;": "\uD835\uDD3E",
    "&GreaterEqual;": "\u2265",
    "&GreaterEqualLess;": "\u22DB",
    "&GreaterFullEqual;": "\u2267",
    "&GreaterGreater;": "\u2AA2",
    "&GreaterLess;": "\u2277",
    "&GreaterSlantEqual;": "\u2A7E",
    "&GreaterTilde;": "\u2273",
    "&Gscr;": "\uD835\uDCA2",
    "&Gt;": "\u226B",
    "&HARDcy;": "\u042A",
    "&Hacek;": "\u02C7",
    "&Hat;": "\u005E",
    "&Hcirc;": "\u0124",
    "&Hfr;": "\u210C",
    "&HilbertSpace;": "\u210B",
    "&Hopf;": "\u210D",
    "&HorizontalLine;": "\u2500",
    "&Hscr;": "\u210B",
    "&Hstrok;": "\u0126",
    "&HumpDownHump;": "\u224E",
    "&HumpEqual;": "\u224F",
    "&IEcy;": "\u0415",
    "&IJlig;": "\u0132",
    "&IOcy;": "\u0401",
    "&Iacute": "\u00CD",
    "&Iacute;": "\u00CD",
    "&Icirc": "\u00CE",
    "&Icirc;": "\u00CE",
    "&Icy;": "\u0418",
    "&Idot;": "\u0130",
    "&Ifr;": "\u2111",
    "&Igrave": "\u00CC",
    "&Igrave;": "\u00CC",
    "&Im;": "\u2111",
    "&Imacr;": "\u012A",
    "&ImaginaryI;": "\u2148",
    "&Implies;": "\u21D2",
    "&Int;": "\u222C",
    "&Integral;": "\u222B",
    "&Intersection;": "\u22C2",
    "&InvisibleComma;": "\u2063",
    "&InvisibleTimes;": "\u2062",
    "&Iogon;": "\u012E",
    "&Iopf;": "\uD835\uDD40",
    "&Iota;": "\u0399",
    "&Iscr;": "\u2110",
    "&Itilde;": "\u0128",
    "&Iukcy;": "\u0406",
    "&Iuml": "\u00CF",
    "&Iuml;": "\u00CF",
    "&Jcirc;": "\u0134",
    "&Jcy;": "\u0419",
    "&Jfr;": "\uD835\uDD0D",
    "&Jopf;": "\uD835\uDD41",
    "&Jscr;": "\uD835\uDCA5",
    "&Jsercy;": "\u0408",
    "&Jukcy;": "\u0404",
    "&KHcy;": "\u0425",
    "&KJcy;": "\u040C",
    "&Kappa;": "\u039A",
    "&Kcedil;": "\u0136",
    "&Kcy;": "\u041A",
    "&Kfr;": "\uD835\uDD0E",
    "&Kopf;": "\uD835\uDD42",
    "&Kscr;": "\uD835\uDCA6",
    "&LJcy;": "\u0409",
    "&LT": "\u003C",
    "&LT;": "\u003C",
    "&Lacute;": "\u0139",
    "&Lambda;": "\u039B",
    "&Lang;": "\u27EA",
    "&Laplacetrf;": "\u2112",
    "&Larr;": "\u219E",
    "&Lcaron;": "\u013D",
    "&Lcedil;": "\u013B",
    "&Lcy;": "\u041B",
    "&LeftAngleBracket;": "\u27E8",
    "&LeftArrow;": "\u2190",
    "&LeftArrowBar;": "\u21E4",
    "&LeftArrowRightArrow;": "\u21C6",
    "&LeftCeiling;": "\u2308",
    "&LeftDoubleBracket;": "\u27E6",
    "&LeftDownTeeVector;": "\u2961",
    "&LeftDownVector;": "\u21C3",
    "&LeftDownVectorBar;": "\u2959",
    "&LeftFloor;": "\u230A",
    "&LeftRightArrow;": "\u2194",
    "&LeftRightVector;": "\u294E",
    "&LeftTee;": "\u22A3",
    "&LeftTeeArrow;": "\u21A4",
    "&LeftTeeVector;": "\u295A",
    "&LeftTriangle;": "\u22B2",
    "&LeftTriangleBar;": "\u29CF",
    "&LeftTriangleEqual;": "\u22B4",
    "&LeftUpDownVector;": "\u2951",
    "&LeftUpTeeVector;": "\u2960",
    "&LeftUpVector;": "\u21BF",
    "&LeftUpVectorBar;": "\u2958",
    "&LeftVector;": "\u21BC",
    "&LeftVectorBar;": "\u2952",
    "&Leftarrow;": "\u21D0",
    "&Leftrightarrow;": "\u21D4",
    "&LessEqualGreater;": "\u22DA",
    "&LessFullEqual;": "\u2266",
    "&LessGreater;": "\u2276",
    "&LessLess;": "\u2AA1",
    "&LessSlantEqual;": "\u2A7D",
    "&LessTilde;": "\u2272",
    "&Lfr;": "\uD835\uDD0F",
    "&Ll;": "\u22D8",
    "&Lleftarrow;": "\u21DA",
    "&Lmidot;": "\u013F",
    "&LongLeftArrow;": "\u27F5",
    "&LongLeftRightArrow;": "\u27F7",
    "&LongRightArrow;": "\u27F6",
    "&Longleftarrow;": "\u27F8",
    "&Longleftrightarrow;": "\u27FA",
    "&Longrightarrow;": "\u27F9",
    "&Lopf;": "\uD835\uDD43",
    "&LowerLeftArrow;": "\u2199",
    "&LowerRightArrow;": "\u2198",
    "&Lscr;": "\u2112",
    "&Lsh;": "\u21B0",
    "&Lstrok;": "\u0141",
    "&Lt;": "\u226A",
    "&Map;": "\u2905",
    "&Mcy;": "\u041C",
    "&MediumSpace;": "\u205F",
    "&Mellintrf;": "\u2133",
    "&Mfr;": "\uD835\uDD10",
    "&MinusPlus;": "\u2213",
    "&Mopf;": "\uD835\uDD44",
    "&Mscr;": "\u2133",
    "&Mu;": "\u039C",
    "&NJcy;": "\u040A",
    "&Nacute;": "\u0143",
    "&Ncaron;": "\u0147",
    "&Ncedil;": "\u0145",
    "&Ncy;": "\u041D",
    "&NegativeMediumSpace;": "\u200B",
    "&NegativeThickSpace;": "\u200B",
    "&NegativeThinSpace;": "\u200B",
    "&NegativeVeryThinSpace;": "\u200B",
    "&NestedGreaterGreater;": "\u226B",
    "&NestedLessLess;": "\u226A",
    "&NewLine;": "\u000A",
    "&Nfr;": "\uD835\uDD11",
    "&NoBreak;": "\u2060",
    "&NonBreakingSpace;": "\u00A0",
    "&Nopf;": "\u2115",
    "&Not;": "\u2AEC",
    "&NotCongruent;": "\u2262",
    "&NotCupCap;": "\u226D",
    "&NotDoubleVerticalBar;": "\u2226",
    "&NotElement;": "\u2209",
    "&NotEqual;": "\u2260",
    "&NotEqualTilde;": "\u2242\u0338",
    "&NotExists;": "\u2204",
    "&NotGreater;": "\u226F",
    "&NotGreaterEqual;": "\u2271",
    "&NotGreaterFullEqual;": "\u2267\u0338",
    "&NotGreaterGreater;": "\u226B\u0338",
    "&NotGreaterLess;": "\u2279",
    "&NotGreaterSlantEqual;": "\u2A7E\u0338",
    "&NotGreaterTilde;": "\u2275",
    "&NotHumpDownHump;": "\u224E\u0338",
    "&NotHumpEqual;": "\u224F\u0338",
    "&NotLeftTriangle;": "\u22EA",
    "&NotLeftTriangleBar;": "\u29CF\u0338",
    "&NotLeftTriangleEqual;": "\u22EC",
    "&NotLess;": "\u226E",
    "&NotLessEqual;": "\u2270",
    "&NotLessGreater;": "\u2278",
    "&NotLessLess;": "\u226A\u0338",
    "&NotLessSlantEqual;": "\u2A7D\u0338",
    "&NotLessTilde;": "\u2274",
    "&NotNestedGreaterGreater;": "\u2AA2\u0338",
    "&NotNestedLessLess;": "\u2AA1\u0338",
    "&NotPrecedes;": "\u2280",
    "&NotPrecedesEqual;": "\u2AAF\u0338",
    "&NotPrecedesSlantEqual;": "\u22E0",
    "&NotReverseElement;": "\u220C",
    "&NotRightTriangle;": "\u22EB",
    "&NotRightTriangleBar;": "\u29D0\u0338",
    "&NotRightTriangleEqual;": "\u22ED",
    "&NotSquareSubset;": "\u228F\u0338",
    "&NotSquareSubsetEqual;": "\u22E2",
    "&NotSquareSuperset;": "\u2290\u0338",
    "&NotSquareSupersetEqual;": "\u22E3",
    "&NotSubset;": "\u2282\u20D2",
    "&NotSubsetEqual;": "\u2288",
    "&NotSucceeds;": "\u2281",
    "&NotSucceedsEqual;": "\u2AB0\u0338",
    "&NotSucceedsSlantEqual;": "\u22E1",
    "&NotSucceedsTilde;": "\u227F\u0338",
    "&NotSuperset;": "\u2283\u20D2",
    "&NotSupersetEqual;": "\u2289",
    "&NotTilde;": "\u2241",
    "&NotTildeEqual;": "\u2244",
    "&NotTildeFullEqual;": "\u2247",
    "&NotTildeTilde;": "\u2249",
    "&NotVerticalBar;": "\u2224",
    "&Nscr;": "\uD835\uDCA9",
    "&Ntilde": "\u00D1",
    "&Ntilde;": "\u00D1",
    "&Nu;": "\u039D",
    "&OElig;": "\u0152",
    "&Oacute": "\u00D3",
    "&Oacute;": "\u00D3",
    "&Ocirc": "\u00D4",
    "&Ocirc;": "\u00D4",
    "&Ocy;": "\u041E",
    "&Odblac;": "\u0150",
    "&Ofr;": "\uD835\uDD12",
    "&Ograve": "\u00D2",
    "&Ograve;": "\u00D2",
    "&Omacr;": "\u014C",
    "&Omega;": "\u03A9",
    "&Omicron;": "\u039F",
    "&Oopf;": "\uD835\uDD46",
    "&OpenCurlyDoubleQuote;": "\u201C",
    "&OpenCurlyQuote;": "\u2018",
    "&Or;": "\u2A54",
    "&Oscr;": "\uD835\uDCAA",
    "&Oslash": "\u00D8",
    "&Oslash;": "\u00D8",
    "&Otilde": "\u00D5",
    "&Otilde;": "\u00D5",
    "&Otimes;": "\u2A37",
    "&Ouml": "\u00D6",
    "&Ouml;": "\u00D6",
    "&OverBar;": "\u203E",
    "&OverBrace;": "\u23DE",
    "&OverBracket;": "\u23B4",
    "&OverParenthesis;": "\u23DC",
    "&PartialD;": "\u2202",
    "&Pcy;": "\u041F",
    "&Pfr;": "\uD835\uDD13",
    "&Phi;": "\u03A6",
    "&Pi;": "\u03A0",
    "&PlusMinus;": "\u00B1",
    "&Poincareplane;": "\u210C",
    "&Popf;": "\u2119",
    "&Pr;": "\u2ABB",
    "&Precedes;": "\u227A",
    "&PrecedesEqual;": "\u2AAF",
    "&PrecedesSlantEqual;": "\u227C",
    "&PrecedesTilde;": "\u227E",
    "&Prime;": "\u2033",
    "&Product;": "\u220F",
    "&Proportion;": "\u2237",
    "&Proportional;": "\u221D",
    "&Pscr;": "\uD835\uDCAB",
    "&Psi;": "\u03A8",
    "&QUOT": "\u0022",
    "&QUOT;": "\u0022",
    "&Qfr;": "\uD835\uDD14",
    "&Qopf;": "\u211A",
    "&Qscr;": "\uD835\uDCAC",
    "&RBarr;": "\u2910",
    "&REG": "\u00AE",
    "&REG;": "\u00AE",
    "&Racute;": "\u0154",
    "&Rang;": "\u27EB",
    "&Rarr;": "\u21A0",
    "&Rarrtl;": "\u2916",
    "&Rcaron;": "\u0158",
    "&Rcedil;": "\u0156",
    "&Rcy;": "\u0420",
    "&Re;": "\u211C",
    "&ReverseElement;": "\u220B",
    "&ReverseEquilibrium;": "\u21CB",
    "&ReverseUpEquilibrium;": "\u296F",
    "&Rfr;": "\u211C",
    "&Rho;": "\u03A1",
    "&RightAngleBracket;": "\u27E9",
    "&RightArrow;": "\u2192",
    "&RightArrowBar;": "\u21E5",
    "&RightArrowLeftArrow;": "\u21C4",
    "&RightCeiling;": "\u2309",
    "&RightDoubleBracket;": "\u27E7",
    "&RightDownTeeVector;": "\u295D",
    "&RightDownVector;": "\u21C2",
    "&RightDownVectorBar;": "\u2955",
    "&RightFloor;": "\u230B",
    "&RightTee;": "\u22A2",
    "&RightTeeArrow;": "\u21A6",
    "&RightTeeVector;": "\u295B",
    "&RightTriangle;": "\u22B3",
    "&RightTriangleBar;": "\u29D0",
    "&RightTriangleEqual;": "\u22B5",
    "&RightUpDownVector;": "\u294F",
    "&RightUpTeeVector;": "\u295C",
    "&RightUpVector;": "\u21BE",
    "&RightUpVectorBar;": "\u2954",
    "&RightVector;": "\u21C0",
    "&RightVectorBar;": "\u2953",
    "&Rightarrow;": "\u21D2",
    "&Ropf;": "\u211D",
    "&RoundImplies;": "\u2970",
    "&Rrightarrow;": "\u21DB",
    "&Rscr;": "\u211B",
    "&Rsh;": "\u21B1",
    "&RuleDelayed;": "\u29F4",
    "&SHCHcy;": "\u0429",
    "&SHcy;": "\u0428",
    "&SOFTcy;": "\u042C",
    "&Sacute;": "\u015A",
    "&Sc;": "\u2ABC",
    "&Scaron;": "\u0160",
    "&Scedil;": "\u015E",
    "&Scirc;": "\u015C",
    "&Scy;": "\u0421",
    "&Sfr;": "\uD835\uDD16",
    "&ShortDownArrow;": "\u2193",
    "&ShortLeftArrow;": "\u2190",
    "&ShortRightArrow;": "\u2192",
    "&ShortUpArrow;": "\u2191",
    "&Sigma;": "\u03A3",
    "&SmallCircle;": "\u2218",
    "&Sopf;": "\uD835\uDD4A",
    "&Sqrt;": "\u221A",
    "&Square;": "\u25A1",
    "&SquareIntersection;": "\u2293",
    "&SquareSubset;": "\u228F",
    "&SquareSubsetEqual;": "\u2291",
    "&SquareSuperset;": "\u2290",
    "&SquareSupersetEqual;": "\u2292",
    "&SquareUnion;": "\u2294",
    "&Sscr;": "\uD835\uDCAE",
    "&Star;": "\u22C6",
    "&Sub;": "\u22D0",
    "&Subset;": "\u22D0",
    "&SubsetEqual;": "\u2286",
    "&Succeeds;": "\u227B",
    "&SucceedsEqual;": "\u2AB0",
    "&SucceedsSlantEqual;": "\u227D",
    "&SucceedsTilde;": "\u227F",
    "&SuchThat;": "\u220B",
    "&Sum;": "\u2211",
    "&Sup;": "\u22D1",
    "&Superset;": "\u2283",
    "&SupersetEqual;": "\u2287",
    "&Supset;": "\u22D1",
    "&THORN": "\u00DE",
    "&THORN;": "\u00DE",
    "&TRADE;": "\u2122",
    "&TSHcy;": "\u040B",
    "&TScy;": "\u0426",
    "&Tab;": "\u0009",
    "&Tau;": "\u03A4",
    "&Tcaron;": "\u0164",
    "&Tcedil;": "\u0162",
    "&Tcy;": "\u0422",
    "&Tfr;": "\uD835\uDD17",
    "&Therefore;": "\u2234",
    "&Theta;": "\u0398",
    "&ThickSpace;": "\u205F\u200A",
    "&ThinSpace;": "\u2009",
    "&Tilde;": "\u223C",
    "&TildeEqual;": "\u2243",
    "&TildeFullEqual;": "\u2245",
    "&TildeTilde;": "\u2248",
    "&Topf;": "\uD835\uDD4B",
    "&TripleDot;": "\u20DB",
    "&Tscr;": "\uD835\uDCAF",
    "&Tstrok;": "\u0166",
    "&Uacute": "\u00DA",
    "&Uacute;": "\u00DA",
    "&Uarr;": "\u219F",
    "&Uarrocir;": "\u2949",
    "&Ubrcy;": "\u040E",
    "&Ubreve;": "\u016C",
    "&Ucirc": "\u00DB",
    "&Ucirc;": "\u00DB",
    "&Ucy;": "\u0423",
    "&Udblac;": "\u0170",
    "&Ufr;": "\uD835\uDD18",
    "&Ugrave": "\u00D9",
    "&Ugrave;": "\u00D9",
    "&Umacr;": "\u016A",
    "&UnderBar;": "\u005F",
    "&UnderBrace;": "\u23DF",
    "&UnderBracket;": "\u23B5",
    "&UnderParenthesis;": "\u23DD",
    "&Union;": "\u22C3",
    "&UnionPlus;": "\u228E",
    "&Uogon;": "\u0172",
    "&Uopf;": "\uD835\uDD4C",
    "&UpArrow;": "\u2191",
    "&UpArrowBar;": "\u2912",
    "&UpArrowDownArrow;": "\u21C5",
    "&UpDownArrow;": "\u2195",
    "&UpEquilibrium;": "\u296E",
    "&UpTee;": "\u22A5",
    "&UpTeeArrow;": "\u21A5",
    "&Uparrow;": "\u21D1",
    "&Updownarrow;": "\u21D5",
    "&UpperLeftArrow;": "\u2196",
    "&UpperRightArrow;": "\u2197",
    "&Upsi;": "\u03D2",
    "&Upsilon;": "\u03A5",
    "&Uring;": "\u016E",
    "&Uscr;": "\uD835\uDCB0",
    "&Utilde;": "\u0168",
    "&Uuml": "\u00DC",
    "&Uuml;": "\u00DC",
    "&VDash;": "\u22AB",
    "&Vbar;": "\u2AEB",
    "&Vcy;": "\u0412",
    "&Vdash;": "\u22A9",
    "&Vdashl;": "\u2AE6",
    "&Vee;": "\u22C1",
    "&Verbar;": "\u2016",
    "&Vert;": "\u2016",
    "&VerticalBar;": "\u2223",
    "&VerticalLine;": "\u007C",
    "&VerticalSeparator;": "\u2758",
    "&VerticalTilde;": "\u2240",
    "&VeryThinSpace;": "\u200A",
    "&Vfr;": "\uD835\uDD19",
    "&Vopf;": "\uD835\uDD4D",
    "&Vscr;": "\uD835\uDCB1",
    "&Vvdash;": "\u22AA",
    "&Wcirc;": "\u0174",
    "&Wedge;": "\u22C0",
    "&Wfr;": "\uD835\uDD1A",
    "&Wopf;": "\uD835\uDD4E",
    "&Wscr;": "\uD835\uDCB2",
    "&Xfr;": "\uD835\uDD1B",
    "&Xi;": "\u039E",
    "&Xopf;": "\uD835\uDD4F",
    "&Xscr;": "\uD835\uDCB3",
    "&YAcy;": "\u042F",
    "&YIcy;": "\u0407",
    "&YUcy;": "\u042E",
    "&Yacute": "\u00DD",
    "&Yacute;": "\u00DD",
    "&Ycirc;": "\u0176",
    "&Ycy;": "\u042B",
    "&Yfr;": "\uD835\uDD1C",
    "&Yopf;": "\uD835\uDD50",
    "&Yscr;": "\uD835\uDCB4",
    "&Yuml;": "\u0178",
    "&ZHcy;": "\u0416",
    "&Zacute;": "\u0179",
    "&Zcaron;": "\u017D",
    "&Zcy;": "\u0417",
    "&Zdot;": "\u017B",
    "&ZeroWidthSpace;": "\u200B",
    "&Zeta;": "\u0396",
    "&Zfr;": "\u2128",
    "&Zopf;": "\u2124",
    "&Zscr;": "\uD835\uDCB5",
    "&aacute": "\u00E1",
    "&aacute;": "\u00E1",
    "&abreve;": "\u0103",
    "&ac;": "\u223E",
    "&acE;": "\u223E\u0333",
    "&acd;": "\u223F",
    "&acirc": "\u00E2",
    "&acirc;": "\u00E2",
    "&acute": "\u00B4",
    "&acute;": "\u00B4",
    "&acy;": "\u0430",
    "&aelig": "\u00E6",
    "&aelig;": "\u00E6",
    "&af;": "\u2061",
    "&afr;": "\uD835\uDD1E",
    "&agrave": "\u00E0",
    "&agrave;": "\u00E0",
    "&alefsym;": "\u2135",
    "&aleph;": "\u2135",
    "&alpha;": "\u03B1",
    "&amacr;": "\u0101",
    "&amalg;": "\u2A3F",
    "&amp": "\u0026",
    "&amp;": "\u0026",
    "&and;": "\u2227",
    "&andand;": "\u2A55",
    "&andd;": "\u2A5C",
    "&andslope;": "\u2A58",
    "&andv;": "\u2A5A",
    "&ang;": "\u2220",
    "&ange;": "\u29A4",
    "&angle;": "\u2220",
    "&angmsd;": "\u2221",
    "&angmsdaa;": "\u29A8",
    "&angmsdab;": "\u29A9",
    "&angmsdac;": "\u29AA",
    "&angmsdad;": "\u29AB",
    "&angmsdae;": "\u29AC",
    "&angmsdaf;": "\u29AD",
    "&angmsdag;": "\u29AE",
    "&angmsdah;": "\u29AF",
    "&angrt;": "\u221F",
    "&angrtvb;": "\u22BE",
    "&angrtvbd;": "\u299D",
    "&angsph;": "\u2222",
    "&angst;": "\u00C5",
    "&angzarr;": "\u237C",
    "&aogon;": "\u0105",
    "&aopf;": "\uD835\uDD52",
    "&ap;": "\u2248",
    "&apE;": "\u2A70",
    "&apacir;": "\u2A6F",
    "&ape;": "\u224A",
    "&apid;": "\u224B",
    "&apos;": "\u0027",
    "&approx;": "\u2248",
    "&approxeq;": "\u224A",
    "&aring": "\u00E5",
    "&aring;": "\u00E5",
    "&ascr;": "\uD835\uDCB6",
    "&ast;": "\u002A",
    "&asymp;": "\u2248",
    "&asympeq;": "\u224D",
    "&atilde": "\u00E3",
    "&atilde;": "\u00E3",
    "&auml": "\u00E4",
    "&auml;": "\u00E4",
    "&awconint;": "\u2233",
    "&awint;": "\u2A11",
    "&bNot;": "\u2AED",
    "&backcong;": "\u224C",
    "&backepsilon;": "\u03F6",
    "&backprime;": "\u2035",
    "&backsim;": "\u223D",
    "&backsimeq;": "\u22CD",
    "&barvee;": "\u22BD",
    "&barwed;": "\u2305",
    "&barwedge;": "\u2305",
    "&bbrk;": "\u23B5",
    "&bbrktbrk;": "\u23B6",
    "&bcong;": "\u224C",
    "&bcy;": "\u0431",
    "&bdquo;": "\u201E",
    "&becaus;": "\u2235",
    "&because;": "\u2235",
    "&bemptyv;": "\u29B0",
    "&bepsi;": "\u03F6",
    "&bernou;": "\u212C",
    "&beta;": "\u03B2",
    "&beth;": "\u2136",
    "&between;": "\u226C",
    "&bfr;": "\uD835\uDD1F",
    "&bigcap;": "\u22C2",
    "&bigcirc;": "\u25EF",
    "&bigcup;": "\u22C3",
    "&bigodot;": "\u2A00",
    "&bigoplus;": "\u2A01",
    "&bigotimes;": "\u2A02",
    "&bigsqcup;": "\u2A06",
    "&bigstar;": "\u2605",
    "&bigtriangledown;": "\u25BD",
    "&bigtriangleup;": "\u25B3",
    "&biguplus;": "\u2A04",
    "&bigvee;": "\u22C1",
    "&bigwedge;": "\u22C0",
    "&bkarow;": "\u290D",
    "&blacklozenge;": "\u29EB",
    "&blacksquare;": "\u25AA",
    "&blacktriangle;": "\u25B4",
    "&blacktriangledown;": "\u25BE",
    "&blacktriangleleft;": "\u25C2",
    "&blacktriangleright;": "\u25B8",
    "&blank;": "\u2423",
    "&blk12;": "\u2592",
    "&blk14;": "\u2591",
    "&blk34;": "\u2593",
    "&block;": "\u2588",
    "&bne;": "\u003D\u20E5",
    "&bnequiv;": "\u2261\u20E5",
    "&bnot;": "\u2310",
    "&bopf;": "\uD835\uDD53",
    "&bot;": "\u22A5",
    "&bottom;": "\u22A5",
    "&bowtie;": "\u22C8",
    "&boxDL;": "\u2557",
    "&boxDR;": "\u2554",
    "&boxDl;": "\u2556",
    "&boxDr;": "\u2553",
    "&boxH;": "\u2550",
    "&boxHD;": "\u2566",
    "&boxHU;": "\u2569",
    "&boxHd;": "\u2564",
    "&boxHu;": "\u2567",
    "&boxUL;": "\u255D",
    "&boxUR;": "\u255A",
    "&boxUl;": "\u255C",
    "&boxUr;": "\u2559",
    "&boxV;": "\u2551",
    "&boxVH;": "\u256C",
    "&boxVL;": "\u2563",
    "&boxVR;": "\u2560",
    "&boxVh;": "\u256B",
    "&boxVl;": "\u2562",
    "&boxVr;": "\u255F",
    "&boxbox;": "\u29C9",
    "&boxdL;": "\u2555",
    "&boxdR;": "\u2552",
    "&boxdl;": "\u2510",
    "&boxdr;": "\u250C",
    "&boxh;": "\u2500",
    "&boxhD;": "\u2565",
    "&boxhU;": "\u2568",
    "&boxhd;": "\u252C",
    "&boxhu;": "\u2534",
    "&boxminus;": "\u229F",
    "&boxplus;": "\u229E",
    "&boxtimes;": "\u22A0",
    "&boxuL;": "\u255B",
    "&boxuR;": "\u2558",
    "&boxul;": "\u2518",
    "&boxur;": "\u2514",
    "&boxv;": "\u2502",
    "&boxvH;": "\u256A",
    "&boxvL;": "\u2561",
    "&boxvR;": "\u255E",
    "&boxvh;": "\u253C",
    "&boxvl;": "\u2524",
    "&boxvr;": "\u251C",
    "&bprime;": "\u2035",
    "&breve;": "\u02D8",
    "&brvbar": "\u00A6",
    "&brvbar;": "\u00A6",
    "&bscr;": "\uD835\uDCB7",
    "&bsemi;": "\u204F",
    "&bsim;": "\u223D",
    "&bsime;": "\u22CD",
    "&bsol;": "\u005C",
    "&bsolb;": "\u29C5",
    "&bsolhsub;": "\u27C8",
    "&bull;": "\u2022",
    "&bullet;": "\u2022",
    "&bump;": "\u224E",
    "&bumpE;": "\u2AAE",
    "&bumpe;": "\u224F",
    "&bumpeq;": "\u224F",
    "&cacute;": "\u0107",
    "&cap;": "\u2229",
    "&capand;": "\u2A44",
    "&capbrcup;": "\u2A49",
    "&capcap;": "\u2A4B",
    "&capcup;": "\u2A47",
    "&capdot;": "\u2A40",
    "&caps;": "\u2229\uFE00",
    "&caret;": "\u2041",
    "&caron;": "\u02C7",
    "&ccaps;": "\u2A4D",
    "&ccaron;": "\u010D",
    "&ccedil": "\u00E7",
    "&ccedil;": "\u00E7",
    "&ccirc;": "\u0109",
    "&ccups;": "\u2A4C",
    "&ccupssm;": "\u2A50",
    "&cdot;": "\u010B",
    "&cedil": "\u00B8",
    "&cedil;": "\u00B8",
    "&cemptyv;": "\u29B2",
    "&cent": "\u00A2",
    "&cent;": "\u00A2",
    "&centerdot;": "\u00B7",
    "&cfr;": "\uD835\uDD20",
    "&chcy;": "\u0447",
    "&check;": "\u2713",
    "&checkmark;": "\u2713",
    "&chi;": "\u03C7",
    "&cir;": "\u25CB",
    "&cirE;": "\u29C3",
    "&circ;": "\u02C6",
    "&circeq;": "\u2257",
    "&circlearrowleft;": "\u21BA",
    "&circlearrowright;": "\u21BB",
    "&circledR;": "\u00AE",
    "&circledS;": "\u24C8",
    "&circledast;": "\u229B",
    "&circledcirc;": "\u229A",
    "&circleddash;": "\u229D",
    "&cire;": "\u2257",
    "&cirfnint;": "\u2A10",
    "&cirmid;": "\u2AEF",
    "&cirscir;": "\u29C2",
    "&clubs;": "\u2663",
    "&clubsuit;": "\u2663",
    "&colon;": "\u003A",
    "&colone;": "\u2254",
    "&coloneq;": "\u2254",
    "&comma;": "\u002C",
    "&commat;": "\u0040",
    "&comp;": "\u2201",
    "&compfn;": "\u2218",
    "&complement;": "\u2201",
    "&complexes;": "\u2102",
    "&cong;": "\u2245",
    "&congdot;": "\u2A6D",
    "&conint;": "\u222E",
    "&copf;": "\uD835\uDD54",
    "&coprod;": "\u2210",
    "&copy": "\u00A9",
    "&copy;": "\u00A9",
    "&copysr;": "\u2117",
    "&crarr;": "\u21B5",
    "&cross;": "\u2717",
    "&cscr;": "\uD835\uDCB8",
    "&csub;": "\u2ACF",
    "&csube;": "\u2AD1",
    "&csup;": "\u2AD0",
    "&csupe;": "\u2AD2",
    "&ctdot;": "\u22EF",
    "&cudarrl;": "\u2938",
    "&cudarrr;": "\u2935",
    "&cuepr;": "\u22DE",
    "&cuesc;": "\u22DF",
    "&cularr;": "\u21B6",
    "&cularrp;": "\u293D",
    "&cup;": "\u222A",
    "&cupbrcap;": "\u2A48",
    "&cupcap;": "\u2A46",
    "&cupcup;": "\u2A4A",
    "&cupdot;": "\u228D",
    "&cupor;": "\u2A45",
    "&cups;": "\u222A\uFE00",
    "&curarr;": "\u21B7",
    "&curarrm;": "\u293C",
    "&curlyeqprec;": "\u22DE",
    "&curlyeqsucc;": "\u22DF",
    "&curlyvee;": "\u22CE",
    "&curlywedge;": "\u22CF",
    "&curren": "\u00A4",
    "&curren;": "\u00A4",
    "&curvearrowleft;": "\u21B6",
    "&curvearrowright;": "\u21B7",
    "&cuvee;": "\u22CE",
    "&cuwed;": "\u22CF",
    "&cwconint;": "\u2232",
    "&cwint;": "\u2231",
    "&cylcty;": "\u232D",
    "&dArr;": "\u21D3",
    "&dHar;": "\u2965",
    "&dagger;": "\u2020",
    "&daleth;": "\u2138",
    "&darr;": "\u2193",
    "&dash;": "\u2010",
    "&dashv;": "\u22A3",
    "&dbkarow;": "\u290F",
    "&dblac;": "\u02DD",
    "&dcaron;": "\u010F",
    "&dcy;": "\u0434",
    "&dd;": "\u2146",
    "&ddagger;": "\u2021",
    "&ddarr;": "\u21CA",
    "&ddotseq;": "\u2A77",
    "&deg": "\u00B0",
    "&deg;": "\u00B0",
    "&delta;": "\u03B4",
    "&demptyv;": "\u29B1",
    "&dfisht;": "\u297F",
    "&dfr;": "\uD835\uDD21",
    "&dharl;": "\u21C3",
    "&dharr;": "\u21C2",
    "&diam;": "\u22C4",
    "&diamond;": "\u22C4",
    "&diamondsuit;": "\u2666",
    "&diams;": "\u2666",
    "&die;": "\u00A8",
    "&digamma;": "\u03DD",
    "&disin;": "\u22F2",
    "&div;": "\u00F7",
    "&divide": "\u00F7",
    "&divide;": "\u00F7",
    "&divideontimes;": "\u22C7",
    "&divonx;": "\u22C7",
    "&djcy;": "\u0452",
    "&dlcorn;": "\u231E",
    "&dlcrop;": "\u230D",
    "&dollar;": "\u0024",
    "&dopf;": "\uD835\uDD55",
    "&dot;": "\u02D9",
    "&doteq;": "\u2250",
    "&doteqdot;": "\u2251",
    "&dotminus;": "\u2238",
    "&dotplus;": "\u2214",
    "&dotsquare;": "\u22A1",
    "&doublebarwedge;": "\u2306",
    "&downarrow;": "\u2193",
    "&downdownarrows;": "\u21CA",
    "&downharpoonleft;": "\u21C3",
    "&downharpoonright;": "\u21C2",
    "&drbkarow;": "\u2910",
    "&drcorn;": "\u231F",
    "&drcrop;": "\u230C",
    "&dscr;": "\uD835\uDCB9",
    "&dscy;": "\u0455",
    "&dsol;": "\u29F6",
    "&dstrok;": "\u0111",
    "&dtdot;": "\u22F1",
    "&dtri;": "\u25BF",
    "&dtrif;": "\u25BE",
    "&duarr;": "\u21F5",
    "&duhar;": "\u296F",
    "&dwangle;": "\u29A6",
    "&dzcy;": "\u045F",
    "&dzigrarr;": "\u27FF",
    "&eDDot;": "\u2A77",
    "&eDot;": "\u2251",
    "&eacute": "\u00E9",
    "&eacute;": "\u00E9",
    "&easter;": "\u2A6E",
    "&ecaron;": "\u011B",
    "&ecir;": "\u2256",
    "&ecirc": "\u00EA",
    "&ecirc;": "\u00EA",
    "&ecolon;": "\u2255",
    "&ecy;": "\u044D",
    "&edot;": "\u0117",
    "&ee;": "\u2147",
    "&efDot;": "\u2252",
    "&efr;": "\uD835\uDD22",
    "&eg;": "\u2A9A",
    "&egrave": "\u00E8",
    "&egrave;": "\u00E8",
    "&egs;": "\u2A96",
    "&egsdot;": "\u2A98",
    "&el;": "\u2A99",
    "&elinters;": "\u23E7",
    "&ell;": "\u2113",
    "&els;": "\u2A95",
    "&elsdot;": "\u2A97",
    "&emacr;": "\u0113",
    "&empty;": "\u2205",
    "&emptyset;": "\u2205",
    "&emptyv;": "\u2205",
    "&emsp13;": "\u2004",
    "&emsp14;": "\u2005",
    "&emsp;": "\u2003",
    "&eng;": "\u014B",
    "&ensp;": "\u2002",
    "&eogon;": "\u0119",
    "&eopf;": "\uD835\uDD56",
    "&epar;": "\u22D5",
    "&eparsl;": "\u29E3",
    "&eplus;": "\u2A71",
    "&epsi;": "\u03B5",
    "&epsilon;": "\u03B5",
    "&epsiv;": "\u03F5",
    "&eqcirc;": "\u2256",
    "&eqcolon;": "\u2255",
    "&eqsim;": "\u2242",
    "&eqslantgtr;": "\u2A96",
    "&eqslantless;": "\u2A95",
    "&equals;": "\u003D",
    "&equest;": "\u225F",
    "&equiv;": "\u2261",
    "&equivDD;": "\u2A78",
    "&eqvparsl;": "\u29E5",
    "&erDot;": "\u2253",
    "&erarr;": "\u2971",
    "&escr;": "\u212F",
    "&esdot;": "\u2250",
    "&esim;": "\u2242",
    "&eta;": "\u03B7",
    "&eth": "\u00F0",
    "&eth;": "\u00F0",
    "&euml": "\u00EB",
    "&euml;": "\u00EB",
    "&euro;": "\u20AC",
    "&excl;": "\u0021",
    "&exist;": "\u2203",
    "&expectation;": "\u2130",
    "&exponentiale;": "\u2147",
    "&fallingdotseq;": "\u2252",
    "&fcy;": "\u0444",
    "&female;": "\u2640",
    "&ffilig;": "\uFB03",
    "&fflig;": "\uFB00",
    "&ffllig;": "\uFB04",
    "&ffr;": "\uD835\uDD23",
    "&filig;": "\uFB01",
    "&fjlig;": "\u0066\u006A",
    "&flat;": "\u266D",
    "&fllig;": "\uFB02",
    "&fltns;": "\u25B1",
    "&fnof;": "\u0192",
    "&fopf;": "\uD835\uDD57",
    "&forall;": "\u2200",
    "&fork;": "\u22D4",
    "&forkv;": "\u2AD9",
    "&fpartint;": "\u2A0D",
    "&frac12": "\u00BD",
    "&frac12;": "\u00BD",
    "&frac13;": "\u2153",
    "&frac14": "\u00BC",
    "&frac14;": "\u00BC",
    "&frac15;": "\u2155",
    "&frac16;": "\u2159",
    "&frac18;": "\u215B",
    "&frac23;": "\u2154",
    "&frac25;": "\u2156",
    "&frac34": "\u00BE",
    "&frac34;": "\u00BE",
    "&frac35;": "\u2157",
    "&frac38;": "\u215C",
    "&frac45;": "\u2158",
    "&frac56;": "\u215A",
    "&frac58;": "\u215D",
    "&frac78;": "\u215E",
    "&frasl;": "\u2044",
    "&frown;": "\u2322",
    "&fscr;": "\uD835\uDCBB",
    "&gE;": "\u2267",
    "&gEl;": "\u2A8C",
    "&gacute;": "\u01F5",
    "&gamma;": "\u03B3",
    "&gammad;": "\u03DD",
    "&gap;": "\u2A86",
    "&gbreve;": "\u011F",
    "&gcirc;": "\u011D",
    "&gcy;": "\u0433",
    "&gdot;": "\u0121",
    "&ge;": "\u2265",
    "&gel;": "\u22DB",
    "&geq;": "\u2265",
    "&geqq;": "\u2267",
    "&geqslant;": "\u2A7E",
    "&ges;": "\u2A7E",
    "&gescc;": "\u2AA9",
    "&gesdot;": "\u2A80",
    "&gesdoto;": "\u2A82",
    "&gesdotol;": "\u2A84",
    "&gesl;": "\u22DB\uFE00",
    "&gesles;": "\u2A94",
    "&gfr;": "\uD835\uDD24",
    "&gg;": "\u226B",
    "&ggg;": "\u22D9",
    "&gimel;": "\u2137",
    "&gjcy;": "\u0453",
    "&gl;": "\u2277",
    "&glE;": "\u2A92",
    "&gla;": "\u2AA5",
    "&glj;": "\u2AA4",
    "&gnE;": "\u2269",
    "&gnap;": "\u2A8A",
    "&gnapprox;": "\u2A8A",
    "&gne;": "\u2A88",
    "&gneq;": "\u2A88",
    "&gneqq;": "\u2269",
    "&gnsim;": "\u22E7",
    "&gopf;": "\uD835\uDD58",
    "&grave;": "\u0060",
    "&gscr;": "\u210A",
    "&gsim;": "\u2273",
    "&gsime;": "\u2A8E",
    "&gsiml;": "\u2A90",
    "&gt": "\u003E",
    "&gt;": "\u003E",
    "&gtcc;": "\u2AA7",
    "&gtcir;": "\u2A7A",
    "&gtdot;": "\u22D7",
    "&gtlPar;": "\u2995",
    "&gtquest;": "\u2A7C",
    "&gtrapprox;": "\u2A86",
    "&gtrarr;": "\u2978",
    "&gtrdot;": "\u22D7",
    "&gtreqless;": "\u22DB",
    "&gtreqqless;": "\u2A8C",
    "&gtrless;": "\u2277",
    "&gtrsim;": "\u2273",
    "&gvertneqq;": "\u2269\uFE00",
    "&gvnE;": "\u2269\uFE00",
    "&hArr;": "\u21D4",
    "&hairsp;": "\u200A",
    "&half;": "\u00BD",
    "&hamilt;": "\u210B",
    "&hardcy;": "\u044A",
    "&harr;": "\u2194",
    "&harrcir;": "\u2948",
    "&harrw;": "\u21AD",
    "&hbar;": "\u210F",
    "&hcirc;": "\u0125",
    "&hearts;": "\u2665",
    "&heartsuit;": "\u2665",
    "&hellip;": "\u2026",
    "&hercon;": "\u22B9",
    "&hfr;": "\uD835\uDD25",
    "&hksearow;": "\u2925",
    "&hkswarow;": "\u2926",
    "&hoarr;": "\u21FF",
    "&homtht;": "\u223B",
    "&hookleftarrow;": "\u21A9",
    "&hookrightarrow;": "\u21AA",
    "&hopf;": "\uD835\uDD59",
    "&horbar;": "\u2015",
    "&hscr;": "\uD835\uDCBD",
    "&hslash;": "\u210F",
    "&hstrok;": "\u0127",
    "&hybull;": "\u2043",
    "&hyphen;": "\u2010",
    "&iacute": "\u00ED",
    "&iacute;": "\u00ED",
    "&ic;": "\u2063",
    "&icirc": "\u00EE",
    "&icirc;": "\u00EE",
    "&icy;": "\u0438",
    "&iecy;": "\u0435",
    "&iexcl": "\u00A1",
    "&iexcl;": "\u00A1",
    "&iff;": "\u21D4",
    "&ifr;": "\uD835\uDD26",
    "&igrave": "\u00EC",
    "&igrave;": "\u00EC",
    "&ii;": "\u2148",
    "&iiiint;": "\u2A0C",
    "&iiint;": "\u222D",
    "&iinfin;": "\u29DC",
    "&iiota;": "\u2129",
    "&ijlig;": "\u0133",
    "&imacr;": "\u012B",
    "&image;": "\u2111",
    "&imagline;": "\u2110",
    "&imagpart;": "\u2111",
    "&imath;": "\u0131",
    "&imof;": "\u22B7",
    "&imped;": "\u01B5",
    "&in;": "\u2208",
    "&incare;": "\u2105",
    "&infin;": "\u221E",
    "&infintie;": "\u29DD",
    "&inodot;": "\u0131",
    "&int;": "\u222B",
    "&intcal;": "\u22BA",
    "&integers;": "\u2124",
    "&intercal;": "\u22BA",
    "&intlarhk;": "\u2A17",
    "&intprod;": "\u2A3C",
    "&iocy;": "\u0451",
    "&iogon;": "\u012F",
    "&iopf;": "\uD835\uDD5A",
    "&iota;": "\u03B9",
    "&iprod;": "\u2A3C",
    "&iquest": "\u00BF",
    "&iquest;": "\u00BF",
    "&iscr;": "\uD835\uDCBE",
    "&isin;": "\u2208",
    "&isinE;": "\u22F9",
    "&isindot;": "\u22F5",
    "&isins;": "\u22F4",
    "&isinsv;": "\u22F3",
    "&isinv;": "\u2208",
    "&it;": "\u2062",
    "&itilde;": "\u0129",
    "&iukcy;": "\u0456",
    "&iuml": "\u00EF",
    "&iuml;": "\u00EF",
    "&jcirc;": "\u0135",
    "&jcy;": "\u0439",
    "&jfr;": "\uD835\uDD27",
    "&jmath;": "\u0237",
    "&jopf;": "\uD835\uDD5B",
    "&jscr;": "\uD835\uDCBF",
    "&jsercy;": "\u0458",
    "&jukcy;": "\u0454",
    "&kappa;": "\u03BA",
    "&kappav;": "\u03F0",
    "&kcedil;": "\u0137",
    "&kcy;": "\u043A",
    "&kfr;": "\uD835\uDD28",
    "&kgreen;": "\u0138",
    "&khcy;": "\u0445",
    "&kjcy;": "\u045C",
    "&kopf;": "\uD835\uDD5C",
    "&kscr;": "\uD835\uDCC0",
    "&lAarr;": "\u21DA",
    "&lArr;": "\u21D0",
    "&lAtail;": "\u291B",
    "&lBarr;": "\u290E",
    "&lE;": "\u2266",
    "&lEg;": "\u2A8B",
    "&lHar;": "\u2962",
    "&lacute;": "\u013A",
    "&laemptyv;": "\u29B4",
    "&lagran;": "\u2112",
    "&lambda;": "\u03BB",
    "&lang;": "\u27E8",
    "&langd;": "\u2991",
    "&langle;": "\u27E8",
    "&lap;": "\u2A85",
    "&laquo": "\u00AB",
    "&laquo;": "\u00AB",
    "&larr;": "\u2190",
    "&larrb;": "\u21E4",
    "&larrbfs;": "\u291F",
    "&larrfs;": "\u291D",
    "&larrhk;": "\u21A9",
    "&larrlp;": "\u21AB",
    "&larrpl;": "\u2939",
    "&larrsim;": "\u2973",
    "&larrtl;": "\u21A2",
    "&lat;": "\u2AAB",
    "&latail;": "\u2919",
    "&late;": "\u2AAD",
    "&lates;": "\u2AAD\uFE00",
    "&lbarr;": "\u290C",
    "&lbbrk;": "\u2772",
    "&lbrace;": "\u007B",
    "&lbrack;": "\u005B",
    "&lbrke;": "\u298B",
    "&lbrksld;": "\u298F",
    "&lbrkslu;": "\u298D",
    "&lcaron;": "\u013E",
    "&lcedil;": "\u013C",
    "&lceil;": "\u2308",
    "&lcub;": "\u007B",
    "&lcy;": "\u043B",
    "&ldca;": "\u2936",
    "&ldquo;": "\u201C",
    "&ldquor;": "\u201E",
    "&ldrdhar;": "\u2967",
    "&ldrushar;": "\u294B",
    "&ldsh;": "\u21B2",
    "&le;": "\u2264",
    "&leftarrow;": "\u2190",
    "&leftarrowtail;": "\u21A2",
    "&leftharpoondown;": "\u21BD",
    "&leftharpoonup;": "\u21BC",
    "&leftleftarrows;": "\u21C7",
    "&leftrightarrow;": "\u2194",
    "&leftrightarrows;": "\u21C6",
    "&leftrightharpoons;": "\u21CB",
    "&leftrightsquigarrow;": "\u21AD",
    "&leftthreetimes;": "\u22CB",
    "&leg;": "\u22DA",
    "&leq;": "\u2264",
    "&leqq;": "\u2266",
    "&leqslant;": "\u2A7D",
    "&les;": "\u2A7D",
    "&lescc;": "\u2AA8",
    "&lesdot;": "\u2A7F",
    "&lesdoto;": "\u2A81",
    "&lesdotor;": "\u2A83",
    "&lesg;": "\u22DA\uFE00",
    "&lesges;": "\u2A93",
    "&lessapprox;": "\u2A85",
    "&lessdot;": "\u22D6",
    "&lesseqgtr;": "\u22DA",
    "&lesseqqgtr;": "\u2A8B",
    "&lessgtr;": "\u2276",
    "&lesssim;": "\u2272",
    "&lfisht;": "\u297C",
    "&lfloor;": "\u230A",
    "&lfr;": "\uD835\uDD29",
    "&lg;": "\u2276",
    "&lgE;": "\u2A91",
    "&lhard;": "\u21BD",
    "&lharu;": "\u21BC",
    "&lharul;": "\u296A",
    "&lhblk;": "\u2584",
    "&ljcy;": "\u0459",
    "&ll;": "\u226A",
    "&llarr;": "\u21C7",
    "&llcorner;": "\u231E",
    "&llhard;": "\u296B",
    "&lltri;": "\u25FA",
    "&lmidot;": "\u0140",
    "&lmoust;": "\u23B0",
    "&lmoustache;": "\u23B0",
    "&lnE;": "\u2268",
    "&lnap;": "\u2A89",
    "&lnapprox;": "\u2A89",
    "&lne;": "\u2A87",
    "&lneq;": "\u2A87",
    "&lneqq;": "\u2268",
    "&lnsim;": "\u22E6",
    "&loang;": "\u27EC",
    "&loarr;": "\u21FD",
    "&lobrk;": "\u27E6",
    "&longleftarrow;": "\u27F5",
    "&longleftrightarrow;": "\u27F7",
    "&longmapsto;": "\u27FC",
    "&longrightarrow;": "\u27F6",
    "&looparrowleft;": "\u21AB",
    "&looparrowright;": "\u21AC",
    "&lopar;": "\u2985",
    "&lopf;": "\uD835\uDD5D",
    "&loplus;": "\u2A2D",
    "&lotimes;": "\u2A34",
    "&lowast;": "\u2217",
    "&lowbar;": "\u005F",
    "&loz;": "\u25CA",
    "&lozenge;": "\u25CA",
    "&lozf;": "\u29EB",
    "&lpar;": "\u0028",
    "&lparlt;": "\u2993",
    "&lrarr;": "\u21C6",
    "&lrcorner;": "\u231F",
    "&lrhar;": "\u21CB",
    "&lrhard;": "\u296D",
    "&lrm;": "\u200E",
    "&lrtri;": "\u22BF",
    "&lsaquo;": "\u2039",
    "&lscr;": "\uD835\uDCC1",
    "&lsh;": "\u21B0",
    "&lsim;": "\u2272",
    "&lsime;": "\u2A8D",
    "&lsimg;": "\u2A8F",
    "&lsqb;": "\u005B",
    "&lsquo;": "\u2018",
    "&lsquor;": "\u201A",
    "&lstrok;": "\u0142",
    "&lt": "\u003C",
    "&lt;": "\u003C",
    "&ltcc;": "\u2AA6",
    "&ltcir;": "\u2A79",
    "&ltdot;": "\u22D6",
    "&lthree;": "\u22CB",
    "&ltimes;": "\u22C9",
    "&ltlarr;": "\u2976",
    "&ltquest;": "\u2A7B",
    "&ltrPar;": "\u2996",
    "&ltri;": "\u25C3",
    "&ltrie;": "\u22B4",
    "&ltrif;": "\u25C2",
    "&lurdshar;": "\u294A",
    "&luruhar;": "\u2966",
    "&lvertneqq;": "\u2268\uFE00",
    "&lvnE;": "\u2268\uFE00",
    "&mDDot;": "\u223A",
    "&macr": "\u00AF",
    "&macr;": "\u00AF",
    "&male;": "\u2642",
    "&malt;": "\u2720",
    "&maltese;": "\u2720",
    "&map;": "\u21A6",
    "&mapsto;": "\u21A6",
    "&mapstodown;": "\u21A7",
    "&mapstoleft;": "\u21A4",
    "&mapstoup;": "\u21A5",
    "&marker;": "\u25AE",
    "&mcomma;": "\u2A29",
    "&mcy;": "\u043C",
    "&mdash;": "\u2014",
    "&measuredangle;": "\u2221",
    "&mfr;": "\uD835\uDD2A",
    "&mho;": "\u2127",
    "&micro": "\u00B5",
    "&micro;": "\u00B5",
    "&mid;": "\u2223",
    "&midast;": "\u002A",
    "&midcir;": "\u2AF0",
    "&middot": "\u00B7",
    "&middot;": "\u00B7",
    "&minus;": "\u2212",
    "&minusb;": "\u229F",
    "&minusd;": "\u2238",
    "&minusdu;": "\u2A2A",
    "&mlcp;": "\u2ADB",
    "&mldr;": "\u2026",
    "&mnplus;": "\u2213",
    "&models;": "\u22A7",
    "&mopf;": "\uD835\uDD5E",
    "&mp;": "\u2213",
    "&mscr;": "\uD835\uDCC2",
    "&mstpos;": "\u223E",
    "&mu;": "\u03BC",
    "&multimap;": "\u22B8",
    "&mumap;": "\u22B8",
    "&nGg;": "\u22D9\u0338",
    "&nGt;": "\u226B\u20D2",
    "&nGtv;": "\u226B\u0338",
    "&nLeftarrow;": "\u21CD",
    "&nLeftrightarrow;": "\u21CE",
    "&nLl;": "\u22D8\u0338",
    "&nLt;": "\u226A\u20D2",
    "&nLtv;": "\u226A\u0338",
    "&nRightarrow;": "\u21CF",
    "&nVDash;": "\u22AF",
    "&nVdash;": "\u22AE",
    "&nabla;": "\u2207",
    "&nacute;": "\u0144",
    "&nang;": "\u2220\u20D2",
    "&nap;": "\u2249",
    "&napE;": "\u2A70\u0338",
    "&napid;": "\u224B\u0338",
    "&napos;": "\u0149",
    "&napprox;": "\u2249",
    "&natur;": "\u266E",
    "&natural;": "\u266E",
    "&naturals;": "\u2115",
    "&nbsp": "\u00A0",
    "&nbsp;": "\u00A0",
    "&nbump;": "\u224E\u0338",
    "&nbumpe;": "\u224F\u0338",
    "&ncap;": "\u2A43",
    "&ncaron;": "\u0148",
    "&ncedil;": "\u0146",
    "&ncong;": "\u2247",
    "&ncongdot;": "\u2A6D\u0338",
    "&ncup;": "\u2A42",
    "&ncy;": "\u043D",
    "&ndash;": "\u2013",
    "&ne;": "\u2260",
    "&neArr;": "\u21D7",
    "&nearhk;": "\u2924",
    "&nearr;": "\u2197",
    "&nearrow;": "\u2197",
    "&nedot;": "\u2250\u0338",
    "&nequiv;": "\u2262",
    "&nesear;": "\u2928",
    "&nesim;": "\u2242\u0338",
    "&nexist;": "\u2204",
    "&nexists;": "\u2204",
    "&nfr;": "\uD835\uDD2B",
    "&ngE;": "\u2267\u0338",
    "&nge;": "\u2271",
    "&ngeq;": "\u2271",
    "&ngeqq;": "\u2267\u0338",
    "&ngeqslant;": "\u2A7E\u0338",
    "&nges;": "\u2A7E\u0338",
    "&ngsim;": "\u2275",
    "&ngt;": "\u226F",
    "&ngtr;": "\u226F",
    "&nhArr;": "\u21CE",
    "&nharr;": "\u21AE",
    "&nhpar;": "\u2AF2",
    "&ni;": "\u220B",
    "&nis;": "\u22FC",
    "&nisd;": "\u22FA",
    "&niv;": "\u220B",
    "&njcy;": "\u045A",
    "&nlArr;": "\u21CD",
    "&nlE;": "\u2266\u0338",
    "&nlarr;": "\u219A",
    "&nldr;": "\u2025",
    "&nle;": "\u2270",
    "&nleftarrow;": "\u219A",
    "&nleftrightarrow;": "\u21AE",
    "&nleq;": "\u2270",
    "&nleqq;": "\u2266\u0338",
    "&nleqslant;": "\u2A7D\u0338",
    "&nles;": "\u2A7D\u0338",
    "&nless;": "\u226E",
    "&nlsim;": "\u2274",
    "&nlt;": "\u226E",
    "&nltri;": "\u22EA",
    "&nltrie;": "\u22EC",
    "&nmid;": "\u2224",
    "&nopf;": "\uD835\uDD5F",
    "&not": "\u00AC",
    "&not;": "\u00AC",
    "&notin;": "\u2209",
    "&notinE;": "\u22F9\u0338",
    "&notindot;": "\u22F5\u0338",
    "&notinva;": "\u2209",
    "&notinvb;": "\u22F7",
    "&notinvc;": "\u22F6",
    "&notni;": "\u220C",
    "&notniva;": "\u220C",
    "&notnivb;": "\u22FE",
    "&notnivc;": "\u22FD",
    "&npar;": "\u2226",
    "&nparallel;": "\u2226",
    "&nparsl;": "\u2AFD\u20E5",
    "&npart;": "\u2202\u0338",
    "&npolint;": "\u2A14",
    "&npr;": "\u2280",
    "&nprcue;": "\u22E0",
    "&npre;": "\u2AAF\u0338",
    "&nprec;": "\u2280",
    "&npreceq;": "\u2AAF\u0338",
    "&nrArr;": "\u21CF",
    "&nrarr;": "\u219B",
    "&nrarrc;": "\u2933\u0338",
    "&nrarrw;": "\u219D\u0338",
    "&nrightarrow;": "\u219B",
    "&nrtri;": "\u22EB",
    "&nrtrie;": "\u22ED",
    "&nsc;": "\u2281",
    "&nsccue;": "\u22E1",
    "&nsce;": "\u2AB0\u0338",
    "&nscr;": "\uD835\uDCC3",
    "&nshortmid;": "\u2224",
    "&nshortparallel;": "\u2226",
    "&nsim;": "\u2241",
    "&nsime;": "\u2244",
    "&nsimeq;": "\u2244",
    "&nsmid;": "\u2224",
    "&nspar;": "\u2226",
    "&nsqsube;": "\u22E2",
    "&nsqsupe;": "\u22E3",
    "&nsub;": "\u2284",
    "&nsubE;": "\u2AC5\u0338",
    "&nsube;": "\u2288",
    "&nsubset;": "\u2282\u20D2",
    "&nsubseteq;": "\u2288",
    "&nsubseteqq;": "\u2AC5\u0338",
    "&nsucc;": "\u2281",
    "&nsucceq;": "\u2AB0\u0338",
    "&nsup;": "\u2285",
    "&nsupE;": "\u2AC6\u0338",
    "&nsupe;": "\u2289",
    "&nsupset;": "\u2283\u20D2",
    "&nsupseteq;": "\u2289",
    "&nsupseteqq;": "\u2AC6\u0338",
    "&ntgl;": "\u2279",
    "&ntilde": "\u00F1",
    "&ntilde;": "\u00F1",
    "&ntlg;": "\u2278",
    "&ntriangleleft;": "\u22EA",
    "&ntrianglelefteq;": "\u22EC",
    "&ntriangleright;": "\u22EB",
    "&ntrianglerighteq;": "\u22ED",
    "&nu;": "\u03BD",
    "&num;": "\u0023",
    "&numero;": "\u2116",
    "&numsp;": "\u2007",
    "&nvDash;": "\u22AD",
    "&nvHarr;": "\u2904",
    "&nvap;": "\u224D\u20D2",
    "&nvdash;": "\u22AC",
    "&nvge;": "\u2265\u20D2",
    "&nvgt;": "\u003E\u20D2",
    "&nvinfin;": "\u29DE",
    "&nvlArr;": "\u2902",
    "&nvle;": "\u2264\u20D2",
    "&nvlt;": "\u003C\u20D2",
    "&nvltrie;": "\u22B4\u20D2",
    "&nvrArr;": "\u2903",
    "&nvrtrie;": "\u22B5\u20D2",
    "&nvsim;": "\u223C\u20D2",
    "&nwArr;": "\u21D6",
    "&nwarhk;": "\u2923",
    "&nwarr;": "\u2196",
    "&nwarrow;": "\u2196",
    "&nwnear;": "\u2927",
    "&oS;": "\u24C8",
    "&oacute": "\u00F3",
    "&oacute;": "\u00F3",
    "&oast;": "\u229B",
    "&ocir;": "\u229A",
    "&ocirc": "\u00F4",
    "&ocirc;": "\u00F4",
    "&ocy;": "\u043E",
    "&odash;": "\u229D",
    "&odblac;": "\u0151",
    "&odiv;": "\u2A38",
    "&odot;": "\u2299",
    "&odsold;": "\u29BC",
    "&oelig;": "\u0153",
    "&ofcir;": "\u29BF",
    "&ofr;": "\uD835\uDD2C",
    "&ogon;": "\u02DB",
    "&ograve": "\u00F2",
    "&ograve;": "\u00F2",
    "&ogt;": "\u29C1",
    "&ohbar;": "\u29B5",
    "&ohm;": "\u03A9",
    "&oint;": "\u222E",
    "&olarr;": "\u21BA",
    "&olcir;": "\u29BE",
    "&olcross;": "\u29BB",
    "&oline;": "\u203E",
    "&olt;": "\u29C0",
    "&omacr;": "\u014D",
    "&omega;": "\u03C9",
    "&omicron;": "\u03BF",
    "&omid;": "\u29B6",
    "&ominus;": "\u2296",
    "&oopf;": "\uD835\uDD60",
    "&opar;": "\u29B7",
    "&operp;": "\u29B9",
    "&oplus;": "\u2295",
    "&or;": "\u2228",
    "&orarr;": "\u21BB",
    "&ord;": "\u2A5D",
    "&order;": "\u2134",
    "&orderof;": "\u2134",
    "&ordf": "\u00AA",
    "&ordf;": "\u00AA",
    "&ordm": "\u00BA",
    "&ordm;": "\u00BA",
    "&origof;": "\u22B6",
    "&oror;": "\u2A56",
    "&orslope;": "\u2A57",
    "&orv;": "\u2A5B",
    "&oscr;": "\u2134",
    "&oslash": "\u00F8",
    "&oslash;": "\u00F8",
    "&osol;": "\u2298",
    "&otilde": "\u00F5",
    "&otilde;": "\u00F5",
    "&otimes;": "\u2297",
    "&otimesas;": "\u2A36",
    "&ouml": "\u00F6",
    "&ouml;": "\u00F6",
    "&ovbar;": "\u233D",
    "&par;": "\u2225",
    "&para": "\u00B6",
    "&para;": "\u00B6",
    "&parallel;": "\u2225",
    "&parsim;": "\u2AF3",
    "&parsl;": "\u2AFD",
    "&part;": "\u2202",
    "&pcy;": "\u043F",
    "&percnt;": "\u0025",
    "&period;": "\u002E",
    "&permil;": "\u2030",
    "&perp;": "\u22A5",
    "&pertenk;": "\u2031",
    "&pfr;": "\uD835\uDD2D",
    "&phi;": "\u03C6",
    "&phiv;": "\u03D5",
    "&phmmat;": "\u2133",
    "&phone;": "\u260E",
    "&pi;": "\u03C0",
    "&pitchfork;": "\u22D4",
    "&piv;": "\u03D6",
    "&planck;": "\u210F",
    "&planckh;": "\u210E",
    "&plankv;": "\u210F",
    "&plus;": "\u002B",
    "&plusacir;": "\u2A23",
    "&plusb;": "\u229E",
    "&pluscir;": "\u2A22",
    "&plusdo;": "\u2214",
    "&plusdu;": "\u2A25",
    "&pluse;": "\u2A72",
    "&plusmn": "\u00B1",
    "&plusmn;": "\u00B1",
    "&plussim;": "\u2A26",
    "&plustwo;": "\u2A27",
    "&pm;": "\u00B1",
    "&pointint;": "\u2A15",
    "&popf;": "\uD835\uDD61",
    "&pound": "\u00A3",
    "&pound;": "\u00A3",
    "&pr;": "\u227A",
    "&prE;": "\u2AB3",
    "&prap;": "\u2AB7",
    "&prcue;": "\u227C",
    "&pre;": "\u2AAF",
    "&prec;": "\u227A",
    "&precapprox;": "\u2AB7",
    "&preccurlyeq;": "\u227C",
    "&preceq;": "\u2AAF",
    "&precnapprox;": "\u2AB9",
    "&precneqq;": "\u2AB5",
    "&precnsim;": "\u22E8",
    "&precsim;": "\u227E",
    "&prime;": "\u2032",
    "&primes;": "\u2119",
    "&prnE;": "\u2AB5",
    "&prnap;": "\u2AB9",
    "&prnsim;": "\u22E8",
    "&prod;": "\u220F",
    "&profalar;": "\u232E",
    "&profline;": "\u2312",
    "&profsurf;": "\u2313",
    "&prop;": "\u221D",
    "&propto;": "\u221D",
    "&prsim;": "\u227E",
    "&prurel;": "\u22B0",
    "&pscr;": "\uD835\uDCC5",
    "&psi;": "\u03C8",
    "&puncsp;": "\u2008",
    "&qfr;": "\uD835\uDD2E",
    "&qint;": "\u2A0C",
    "&qopf;": "\uD835\uDD62",
    "&qprime;": "\u2057",
    "&qscr;": "\uD835\uDCC6",
    "&quaternions;": "\u210D",
    "&quatint;": "\u2A16",
    "&quest;": "\u003F",
    "&questeq;": "\u225F",
    "&quot": "\u0022",
    "&quot;": "\u0022",
    "&rAarr;": "\u21DB",
    "&rArr;": "\u21D2",
    "&rAtail;": "\u291C",
    "&rBarr;": "\u290F",
    "&rHar;": "\u2964",
    "&race;": "\u223D\u0331",
    "&racute;": "\u0155",
    "&radic;": "\u221A",
    "&raemptyv;": "\u29B3",
    "&rang;": "\u27E9",
    "&rangd;": "\u2992",
    "&range;": "\u29A5",
    "&rangle;": "\u27E9",
    "&raquo": "\u00BB",
    "&raquo;": "\u00BB",
    "&rarr;": "\u2192",
    "&rarrap;": "\u2975",
    "&rarrb;": "\u21E5",
    "&rarrbfs;": "\u2920",
    "&rarrc;": "\u2933",
    "&rarrfs;": "\u291E",
    "&rarrhk;": "\u21AA",
    "&rarrlp;": "\u21AC",
    "&rarrpl;": "\u2945",
    "&rarrsim;": "\u2974",
    "&rarrtl;": "\u21A3",
    "&rarrw;": "\u219D",
    "&ratail;": "\u291A",
    "&ratio;": "\u2236",
    "&rationals;": "\u211A",
    "&rbarr;": "\u290D",
    "&rbbrk;": "\u2773",
    "&rbrace;": "\u007D",
    "&rbrack;": "\u005D",
    "&rbrke;": "\u298C",
    "&rbrksld;": "\u298E",
    "&rbrkslu;": "\u2990",
    "&rcaron;": "\u0159",
    "&rcedil;": "\u0157",
    "&rceil;": "\u2309",
    "&rcub;": "\u007D",
    "&rcy;": "\u0440",
    "&rdca;": "\u2937",
    "&rdldhar;": "\u2969",
    "&rdquo;": "\u201D",
    "&rdquor;": "\u201D",
    "&rdsh;": "\u21B3",
    "&real;": "\u211C",
    "&realine;": "\u211B",
    "&realpart;": "\u211C",
    "&reals;": "\u211D",
    "&rect;": "\u25AD",
    "&reg": "\u00AE",
    "&reg;": "\u00AE",
    "&rfisht;": "\u297D",
    "&rfloor;": "\u230B",
    "&rfr;": "\uD835\uDD2F",
    "&rhard;": "\u21C1",
    "&rharu;": "\u21C0",
    "&rharul;": "\u296C",
    "&rho;": "\u03C1",
    "&rhov;": "\u03F1",
    "&rightarrow;": "\u2192",
    "&rightarrowtail;": "\u21A3",
    "&rightharpoondown;": "\u21C1",
    "&rightharpoonup;": "\u21C0",
    "&rightleftarrows;": "\u21C4",
    "&rightleftharpoons;": "\u21CC",
    "&rightrightarrows;": "\u21C9",
    "&rightsquigarrow;": "\u219D",
    "&rightthreetimes;": "\u22CC",
    "&ring;": "\u02DA",
    "&risingdotseq;": "\u2253",
    "&rlarr;": "\u21C4",
    "&rlhar;": "\u21CC",
    "&rlm;": "\u200F",
    "&rmoust;": "\u23B1",
    "&rmoustache;": "\u23B1",
    "&rnmid;": "\u2AEE",
    "&roang;": "\u27ED",
    "&roarr;": "\u21FE",
    "&robrk;": "\u27E7",
    "&ropar;": "\u2986",
    "&ropf;": "\uD835\uDD63",
    "&roplus;": "\u2A2E",
    "&rotimes;": "\u2A35",
    "&rpar;": "\u0029",
    "&rpargt;": "\u2994",
    "&rppolint;": "\u2A12",
    "&rrarr;": "\u21C9",
    "&rsaquo;": "\u203A",
    "&rscr;": "\uD835\uDCC7",
    "&rsh;": "\u21B1",
    "&rsqb;": "\u005D",
    "&rsquo;": "\u2019",
    "&rsquor;": "\u2019",
    "&rthree;": "\u22CC",
    "&rtimes;": "\u22CA",
    "&rtri;": "\u25B9",
    "&rtrie;": "\u22B5",
    "&rtrif;": "\u25B8",
    "&rtriltri;": "\u29CE",
    "&ruluhar;": "\u2968",
    "&rx;": "\u211E",
    "&sacute;": "\u015B",
    "&sbquo;": "\u201A",
    "&sc;": "\u227B",
    "&scE;": "\u2AB4",
    "&scap;": "\u2AB8",
    "&scaron;": "\u0161",
    "&sccue;": "\u227D",
    "&sce;": "\u2AB0",
    "&scedil;": "\u015F",
    "&scirc;": "\u015D",
    "&scnE;": "\u2AB6",
    "&scnap;": "\u2ABA",
    "&scnsim;": "\u22E9",
    "&scpolint;": "\u2A13",
    "&scsim;": "\u227F",
    "&scy;": "\u0441",
    "&sdot;": "\u22C5",
    "&sdotb;": "\u22A1",
    "&sdote;": "\u2A66",
    "&seArr;": "\u21D8",
    "&searhk;": "\u2925",
    "&searr;": "\u2198",
    "&searrow;": "\u2198",
    "&sect": "\u00A7",
    "&sect;": "\u00A7",
    "&semi;": "\u003B",
    "&seswar;": "\u2929",
    "&setminus;": "\u2216",
    "&setmn;": "\u2216",
    "&sext;": "\u2736",
    "&sfr;": "\uD835\uDD30",
    "&sfrown;": "\u2322",
    "&sharp;": "\u266F",
    "&shchcy;": "\u0449",
    "&shcy;": "\u0448",
    "&shortmid;": "\u2223",
    "&shortparallel;": "\u2225",
    "&shy": "\u00AD",
    "&shy;": "\u00AD",
    "&sigma;": "\u03C3",
    "&sigmaf;": "\u03C2",
    "&sigmav;": "\u03C2",
    "&sim;": "\u223C",
    "&simdot;": "\u2A6A",
    "&sime;": "\u2243",
    "&simeq;": "\u2243",
    "&simg;": "\u2A9E",
    "&simgE;": "\u2AA0",
    "&siml;": "\u2A9D",
    "&simlE;": "\u2A9F",
    "&simne;": "\u2246",
    "&simplus;": "\u2A24",
    "&simrarr;": "\u2972",
    "&slarr;": "\u2190",
    "&smallsetminus;": "\u2216",
    "&smashp;": "\u2A33",
    "&smeparsl;": "\u29E4",
    "&smid;": "\u2223",
    "&smile;": "\u2323",
    "&smt;": "\u2AAA",
    "&smte;": "\u2AAC",
    "&smtes;": "\u2AAC\uFE00",
    "&softcy;": "\u044C",
    "&sol;": "\u002F",
    "&solb;": "\u29C4",
    "&solbar;": "\u233F",
    "&sopf;": "\uD835\uDD64",
    "&spades;": "\u2660",
    "&spadesuit;": "\u2660",
    "&spar;": "\u2225",
    "&sqcap;": "\u2293",
    "&sqcaps;": "\u2293\uFE00",
    "&sqcup;": "\u2294",
    "&sqcups;": "\u2294\uFE00",
    "&sqsub;": "\u228F",
    "&sqsube;": "\u2291",
    "&sqsubset;": "\u228F",
    "&sqsubseteq;": "\u2291",
    "&sqsup;": "\u2290",
    "&sqsupe;": "\u2292",
    "&sqsupset;": "\u2290",
    "&sqsupseteq;": "\u2292",
    "&squ;": "\u25A1",
    "&square;": "\u25A1",
    "&squarf;": "\u25AA",
    "&squf;": "\u25AA",
    "&srarr;": "\u2192",
    "&sscr;": "\uD835\uDCC8",
    "&ssetmn;": "\u2216",
    "&ssmile;": "\u2323",
    "&sstarf;": "\u22C6",
    "&star;": "\u2606",
    "&starf;": "\u2605",
    "&straightepsilon;": "\u03F5",
    "&straightphi;": "\u03D5",
    "&strns;": "\u00AF",
    "&sub;": "\u2282",
    "&subE;": "\u2AC5",
    "&subdot;": "\u2ABD",
    "&sube;": "\u2286",
    "&subedot;": "\u2AC3",
    "&submult;": "\u2AC1",
    "&subnE;": "\u2ACB",
    "&subne;": "\u228A",
    "&subplus;": "\u2ABF",
    "&subrarr;": "\u2979",
    "&subset;": "\u2282",
    "&subseteq;": "\u2286",
    "&subseteqq;": "\u2AC5",
    "&subsetneq;": "\u228A",
    "&subsetneqq;": "\u2ACB",
    "&subsim;": "\u2AC7",
    "&subsub;": "\u2AD5",
    "&subsup;": "\u2AD3",
    "&succ;": "\u227B",
    "&succapprox;": "\u2AB8",
    "&succcurlyeq;": "\u227D",
    "&succeq;": "\u2AB0",
    "&succnapprox;": "\u2ABA",
    "&succneqq;": "\u2AB6",
    "&succnsim;": "\u22E9",
    "&succsim;": "\u227F",
    "&sum;": "\u2211",
    "&sung;": "\u266A",
    "&sup1": "\u00B9",
    "&sup1;": "\u00B9",
    "&sup2": "\u00B2",
    "&sup2;": "\u00B2",
    "&sup3": "\u00B3",
    "&sup3;": "\u00B3",
    "&sup;": "\u2283",
    "&supE;": "\u2AC6",
    "&supdot;": "\u2ABE",
    "&supdsub;": "\u2AD8",
    "&supe;": "\u2287",
    "&supedot;": "\u2AC4",
    "&suphsol;": "\u27C9",
    "&suphsub;": "\u2AD7",
    "&suplarr;": "\u297B",
    "&supmult;": "\u2AC2",
    "&supnE;": "\u2ACC",
    "&supne;": "\u228B",
    "&supplus;": "\u2AC0",
    "&supset;": "\u2283",
    "&supseteq;": "\u2287",
    "&supseteqq;": "\u2AC6",
    "&supsetneq;": "\u228B",
    "&supsetneqq;": "\u2ACC",
    "&supsim;": "\u2AC8",
    "&supsub;": "\u2AD4",
    "&supsup;": "\u2AD6",
    "&swArr;": "\u21D9",
    "&swarhk;": "\u2926",
    "&swarr;": "\u2199",
    "&swarrow;": "\u2199",
    "&swnwar;": "\u292A",
    "&szlig": "\u00DF",
    "&szlig;": "\u00DF",
    "&target;": "\u2316",
    "&tau;": "\u03C4",
    "&tbrk;": "\u23B4",
    "&tcaron;": "\u0165",
    "&tcedil;": "\u0163",
    "&tcy;": "\u0442",
    "&tdot;": "\u20DB",
    "&telrec;": "\u2315",
    "&tfr;": "\uD835\uDD31",
    "&there4;": "\u2234",
    "&therefore;": "\u2234",
    "&theta;": "\u03B8",
    "&thetasym;": "\u03D1",
    "&thetav;": "\u03D1",
    "&thickapprox;": "\u2248",
    "&thicksim;": "\u223C",
    "&thinsp;": "\u2009",
    "&thkap;": "\u2248",
    "&thksim;": "\u223C",
    "&thorn": "\u00FE",
    "&thorn;": "\u00FE",
    "&tilde;": "\u02DC",
    "&times": "\u00D7",
    "&times;": "\u00D7",
    "&timesb;": "\u22A0",
    "&timesbar;": "\u2A31",
    "&timesd;": "\u2A30",
    "&tint;": "\u222D",
    "&toea;": "\u2928",
    "&top;": "\u22A4",
    "&topbot;": "\u2336",
    "&topcir;": "\u2AF1",
    "&topf;": "\uD835\uDD65",
    "&topfork;": "\u2ADA",
    "&tosa;": "\u2929",
    "&tprime;": "\u2034",
    "&trade;": "\u2122",
    "&triangle;": "\u25B5",
    "&triangledown;": "\u25BF",
    "&triangleleft;": "\u25C3",
    "&trianglelefteq;": "\u22B4",
    "&triangleq;": "\u225C",
    "&triangleright;": "\u25B9",
    "&trianglerighteq;": "\u22B5",
    "&tridot;": "\u25EC",
    "&trie;": "\u225C",
    "&triminus;": "\u2A3A",
    "&triplus;": "\u2A39",
    "&trisb;": "\u29CD",
    "&tritime;": "\u2A3B",
    "&trpezium;": "\u23E2",
    "&tscr;": "\uD835\uDCC9",
    "&tscy;": "\u0446",
    "&tshcy;": "\u045B",
    "&tstrok;": "\u0167",
    "&twixt;": "\u226C",
    "&twoheadleftarrow;": "\u219E",
    "&twoheadrightarrow;": "\u21A0",
    "&uArr;": "\u21D1",
    "&uHar;": "\u2963",
    "&uacute": "\u00FA",
    "&uacute;": "\u00FA",
    "&uarr;": "\u2191",
    "&ubrcy;": "\u045E",
    "&ubreve;": "\u016D",
    "&ucirc": "\u00FB",
    "&ucirc;": "\u00FB",
    "&ucy;": "\u0443",
    "&udarr;": "\u21C5",
    "&udblac;": "\u0171",
    "&udhar;": "\u296E",
    "&ufisht;": "\u297E",
    "&ufr;": "\uD835\uDD32",
    "&ugrave": "\u00F9",
    "&ugrave;": "\u00F9",
    "&uharl;": "\u21BF",
    "&uharr;": "\u21BE",
    "&uhblk;": "\u2580",
    "&ulcorn;": "\u231C",
    "&ulcorner;": "\u231C",
    "&ulcrop;": "\u230F",
    "&ultri;": "\u25F8",
    "&umacr;": "\u016B",
    "&uml": "\u00A8",
    "&uml;": "\u00A8",
    "&uogon;": "\u0173",
    "&uopf;": "\uD835\uDD66",
    "&uparrow;": "\u2191",
    "&updownarrow;": "\u2195",
    "&upharpoonleft;": "\u21BF",
    "&upharpoonright;": "\u21BE",
    "&uplus;": "\u228E",
    "&upsi;": "\u03C5",
    "&upsih;": "\u03D2",
    "&upsilon;": "\u03C5",
    "&upuparrows;": "\u21C8",
    "&urcorn;": "\u231D",
    "&urcorner;": "\u231D",
    "&urcrop;": "\u230E",
    "&uring;": "\u016F",
    "&urtri;": "\u25F9",
    "&uscr;": "\uD835\uDCCA",
    "&utdot;": "\u22F0",
    "&utilde;": "\u0169",
    "&utri;": "\u25B5",
    "&utrif;": "\u25B4",
    "&uuarr;": "\u21C8",
    "&uuml": "\u00FC",
    "&uuml;": "\u00FC",
    "&uwangle;": "\u29A7",
    "&vArr;": "\u21D5",
    "&vBar;": "\u2AE8",
    "&vBarv;": "\u2AE9",
    "&vDash;": "\u22A8",
    "&vangrt;": "\u299C",
    "&varepsilon;": "\u03F5",
    "&varkappa;": "\u03F0",
    "&varnothing;": "\u2205",
    "&varphi;": "\u03D5",
    "&varpi;": "\u03D6",
    "&varpropto;": "\u221D",
    "&varr;": "\u2195",
    "&varrho;": "\u03F1",
    "&varsigma;": "\u03C2",
    "&varsubsetneq;": "\u228A\uFE00",
    "&varsubsetneqq;": "\u2ACB\uFE00",
    "&varsupsetneq;": "\u228B\uFE00",
    "&varsupsetneqq;": "\u2ACC\uFE00",
    "&vartheta;": "\u03D1",
    "&vartriangleleft;": "\u22B2",
    "&vartriangleright;": "\u22B3",
    "&vcy;": "\u0432",
    "&vdash;": "\u22A2",
    "&vee;": "\u2228",
    "&veebar;": "\u22BB",
    "&veeeq;": "\u225A",
    "&vellip;": "\u22EE",
    "&verbar;": "\u007C",
    "&vert;": "\u007C",
    "&vfr;": "\uD835\uDD33",
    "&vltri;": "\u22B2",
    "&vnsub;": "\u2282\u20D2",
    "&vnsup;": "\u2283\u20D2",
    "&vopf;": "\uD835\uDD67",
    "&vprop;": "\u221D",
    "&vrtri;": "\u22B3",
    "&vscr;": "\uD835\uDCCB",
    "&vsubnE;": "\u2ACB\uFE00",
    "&vsubne;": "\u228A\uFE00",
    "&vsupnE;": "\u2ACC\uFE00",
    "&vsupne;": "\u228B\uFE00",
    "&vzigzag;": "\u299A",
    "&wcirc;": "\u0175",
    "&wedbar;": "\u2A5F",
    "&wedge;": "\u2227",
    "&wedgeq;": "\u2259",
    "&weierp;": "\u2118",
    "&wfr;": "\uD835\uDD34",
    "&wopf;": "\uD835\uDD68",
    "&wp;": "\u2118",
    "&wr;": "\u2240",
    "&wreath;": "\u2240",
    "&wscr;": "\uD835\uDCCC",
    "&xcap;": "\u22C2",
    "&xcirc;": "\u25EF",
    "&xcup;": "\u22C3",
    "&xdtri;": "\u25BD",
    "&xfr;": "\uD835\uDD35",
    "&xhArr;": "\u27FA",
    "&xharr;": "\u27F7",
    "&xi;": "\u03BE",
    "&xlArr;": "\u27F8",
    "&xlarr;": "\u27F5",
    "&xmap;": "\u27FC",
    "&xnis;": "\u22FB",
    "&xodot;": "\u2A00",
    "&xopf;": "\uD835\uDD69",
    "&xoplus;": "\u2A01",
    "&xotime;": "\u2A02",
    "&xrArr;": "\u27F9",
    "&xrarr;": "\u27F6",
    "&xscr;": "\uD835\uDCCD",
    "&xsqcup;": "\u2A06",
    "&xuplus;": "\u2A04",
    "&xutri;": "\u25B3",
    "&xvee;": "\u22C1",
    "&xwedge;": "\u22C0",
    "&yacute": "\u00FD",
    "&yacute;": "\u00FD",
    "&yacy;": "\u044F",
    "&ycirc;": "\u0177",
    "&ycy;": "\u044B",
    "&yen": "\u00A5",
    "&yen;": "\u00A5",
    "&yfr;": "\uD835\uDD36",
    "&yicy;": "\u0457",
    "&yopf;": "\uD835\uDD6A",
    "&yscr;": "\uD835\uDCCE",
    "&yucy;": "\u044E",
    "&yuml": "\u00FF",
    "&yuml;": "\u00FF",
    "&zacute;": "\u017A",
    "&zcaron;": "\u017E",
    "&zcy;": "\u0437",
    "&zdot;": "\u017C",
    "&zeetrf;": "\u2128",
    "&zeta;": "\u03B6",
    "&zfr;": "\uD835\uDD37",
    "&zhcy;": "\u0436",
    "&zigrarr;": "\u21DD",
    "&zopf;": "\uD835\uDD6B",
    "&zscr;": "\uD835\uDCCF",
    "&zwj;": "\u200D",
    "&zwnj;": "\u200C"
};

// jaon <-> iimage
const imgMap: Record<string, string> = {
    "id": "ID",
    "name": "File.Name", // same url+md5+ext, There is a real file name property called "Metadata.filename.0", but it is rarely seen and an error occurs if this property does not exist.
    "file_url": "File.Url",
    "file_size": "File.Size",
    //"sha256": "File.Sha256",
    "md5": "File.Md5",
    "date": "File.CreatedDate",
    "width": "Dimension.Width",
    "height": "Dimension.Height",
    "preview_url": "Thumbnails.2.File.Url", // thumbnails size - 0:256px, 1:512px, 2:1024px
    "preview_width": "Thumbnails.2.File.Url",
    "preview_height": "Thumbnails.2.File.Url",
    "sources": "Metadata.source",
};

// -------- Static Map End --------


// -------- Custom Function Start --------

// Convert to be compatible with existing tag DB
function tag_refinement(images: IImage): void {
    if (images.tags) {
        if (isString(images.tags)) {
            let vaild_tag_check_temp = images.tags.toString();
            if (vaild_tag_check_temp === "null") { // Image tag Not existing image
                images.tags = []; // If tags is (null), the image will not be displayed. Therefore, it is set to an empty list.
            }
            else {
                vaild_tag_check_temp = recover_html_entity(vaild_tag_check_temp, 2); // convert HTML entity to special characters
                images.tags = JSON.parse('{"tags":'+vaild_tag_check_temp+'}')["tags"];
            }
        }
        // I don't think ITag[] will ever be used as an argument in this function.
        /*else if (isITagArr(images.tags)) {
            for(let tag_index=0; tag_index < images.tags.length; tag_index++) {
                images.tags[tag_index].name = images.tags[tag_index].name;
            }
        }*/
    }
}

// Convert Tag Type and Tag Type ID to be compatible with Grabber
function tag_type_compat(tags: ITag): void {
    if (typeof tags.typeId === "number") { // type check
        tags.typeId = TAG_NAME_TO_TTYPE_ID_MAP[tags.typeId] || 0;
    }
    if (typeof tags.type === "string") { // type check
        tags.type = TAG_NAME_TO_GRB_TTYPE_MAP[tags.type] || "general";
    }
}

// Fill in placeholders if no preview image is available (ex. swf file)
function fill_placeholder_preview(images: IImage): void {
    if (!images.preview_url) {
        images.preview_url = "/favicon.ico";
    }
}

// Returns the extracted tags as a string separated by commas.
function extracted_tags_to_string(tags: ITag[]): string {
    let result = String();
    let tag_temp = String();
    for (let tag_index = 0; tag_index < tags.length; tag_index++) {
        tag_temp = tags[tag_index].name.trim(); // Remove left and right spaces
        tag_temp = tag_temp.replace(/^(~[!|+@])\s?/, ''); // Remove prefix, For spaces between prefixes and words, use lazy quantifiers by limiting to 1 in the Grabber.regexToTags function.
        result = result.concat(tag_temp);
        if (tag_index < tags.length - 1) {
            result = result.concat(',');
        }
    }
    return result;
}

// Separate search tags
// Prefix: (no prefix):And, ~|:Or, ~!:Filter, ~+:Unless
// The Permanent Booru's tags will not be searched as intended if they match the prefixes used by the search keyword parser.
// The search parser currently appears to be functioning normally, but it may fail due to various tags in The Permanent Booru that do not conform to its rules. Therefore, we need your feedback.
function search_keyword_parser(search_query: ISearchQuery): Record<string, any>  {
    const parsed_keyword: Record<string, any> = {};
    const extracted_tags_and = Grabber.regexToTags("(?<name>(?:^|(?<!\\\\,)(?<=,))(?!\\s*(?:~[+|!@]))(?:\\\\,|[^,])+(?:(?=,)|$))", search_query.search); // Hints about AND regular expressions came from generative artificial intelligence(Gemini-3.1-Pro).
    const extracted_tags_filter = Grabber.regexToTags("(?<name>(?<![^,]|\\\\,)(?=\\s*~!)(?:\\\\,|[^,])+)", search_query.search);
    const extracted_tags_or = Grabber.regexToTags("(?<name>(?<![^,]|\\\\,)(?=\\s*~\\|)(?:\\\\,|[^,])+)", search_query.search);
    const extracted_tags_unless = Grabber.regexToTags("(?<name>(?<![^,]|\\\\,)(?=\\s*~\\+)(?:\\\\,|[^,])+)", search_query.search);
    const extracted_advanced_option = Grabber.regexToTags("(?<name>(?<![^,]|\\\\,)(?=\\s*~@)(?:\\\\,|[^,])+)", search_query.search);
    parsed_keyword["and"] = extracted_tags_to_string(extracted_tags_and);
    parsed_keyword["filter"] = extracted_tags_to_string(extracted_tags_filter);
    parsed_keyword["or"] = extracted_tags_to_string(extracted_tags_or);
    parsed_keyword["unless"] = extracted_tags_to_string(extracted_tags_unless);
    parsed_keyword["advanced"] = extracted_tags_to_string(extracted_advanced_option);
    return parsed_keyword;
}

// Get the source from the image details page.
function regexToSources(regexp: string, src: string): string[] {
    const urls = Grabber.regexMatches(regexp, src);
    const result = [];
    for (let url_index = 0; url_index < urls.length; url_index++) {
        result[url_index] = urls[url_index]["url"];
    }
    return result;
}

/* In case of JavaScript, if <IImage object>.tags, which is information received through Grabber.
 * regexToImages() in search->parse, is a string, Grabber automatically extracts the rating.
 * However, when writing in TypeScript, <IImage object>.tags must be of type string[] or ITag[].
 * If this constraint is met, tags are stored as string[] (e.g., ["tag1", "tag2", "tag3"]).
 * In this case, Grabber does not automatically extract the rating.
*/
/* There are many variations of the ratring tag, but in this case we only deal with safe, questionable, and explicit.
 * And when there are multiple levels of rating, the priority is explicit > questionable > safe.
*/
// Extract ratring from tags extracted from search->parse.
function fill_rating_from_string_tag(images: IImage): void {
    if (images.tags) {
        if (isITagArr(images.tags)) {
            if (images.tags.filter( (itag: ITag) => { return itag.name === "rating:safe"; }).length > 0) {
                images.rating = "safe";
            }
            if (images.tags.filter( (itag: ITag) => { return itag.name === "rating:questionable"; }).length > 0) {
                images.rating = "questionable";
            }
            if (images.tags.filter( (itag: ITag) => { return itag.name === "rating:explicit"; }).length > 0) {
                images.rating = "explicit";
            }
        }
        else if (isStringArr(images.tags)) {
            if (images.tags.indexOf("rating:safe") > -1) {
                images.rating = "safe";
            }
            if (images.tags.indexOf("rating:questionable") > -1) {
                images.rating = "questionable";
            }
            if (images.tags.indexOf("rating:explicit") > -1) {
                images.rating = "explicit";
            }
        }
    }
}

// HTML Entity code to Normal Character
// code from https://velog.io/@sisofiy626/JS-HTML-entity와-변환
function recover_html_entity(html_str: string, option: number): string {
    if (!html_str) {
        return html_str;
    }
    let ret = html_str;
    if (option === 0) { // HTML Entity name, code replace
        ret = ret.replace(/&\w+;/g, (matched) => html_entity_name_normal_character_maps[matched] || matched);
        ret = ret.replace(/&#\d+;/g, (matched) => html_entity_code_normal_character_maps[matched] || matched);
    }
    else if (option === 1) { // HTML Entity name replace
        ret = ret.replace(/&\w+;/g, (matched) => html_entity_name_normal_character_maps[matched] || matched);
    }
    else if (option === 2) { // HTML Entity code replace
        ret = ret.replace(/&#\d+;/g, (matched) => html_entity_code_normal_character_maps[matched] || matched);
    }
    return ret
}

// wrapper traveling the name of the ITag
function recover_html_entity_itag_wrapper(tag: ITag): void {
    tag.name = recover_html_entity(tag.name, 2);
}

// Json to IImage
function jason_to_iimage(image: Record<string, any>): IImage {
    const img = Grabber.mapFields(image, imgMap);
    // I haven't figured out yet what "Removed" and "Deleted" do.
    img.status = "active";
    if (image.Removed) {
        img.status = "flagged";
    }
    else if (image.Deleted) {
        img.status = "deleted";
    }
    const tags: ITag[] = [];
    let name = undefined;
    let type = undefined;
    let count = undefined;
    let typeId = undefined;
    for (const tag_index in image.Tags) {
        name = (image.Tags[tag_index].Namespace === "none") ? (image.Tags[tag_index].Tag) : (image.Tags[tag_index].Namespace + ':' + image.Tags[tag_index].Tag);
        typeId = TAG_NAME_TO_TTYPE_ID_MAP[image.Tags[tag_index].Namespace] || 0;
        type = TAG_NAME_TO_GRB_TTYPE_MAP[image.Tags[tag_index].Namespace] || "general";
        count = image.Tags[tag_index].Count;
        tags.push({ name, type, count, typeId });
    }
    img.tags = tags;
    img.ext = Grabber.regexMatch("(?<=\\.)\\w+$", img.name)[0]; // extension from file name
    img.name = Grabber.regexMatch("(?<=/)\\p{Hex_Digit}{64}", img.name)[0]; // Remove url and extension parts
    return img;
}

// Creating URL parameters for advanced search options
function mk_adva_url_para(parameters: string): string {
    // Collect alts - alts= on | 
    // Tombstone - tombstone= on | 
    // Since last - since= day | week | month | year
    // Order - order= desc | asc | random | score
    // Mime(multiple) - mime= application | application/x-shockwave-flash | image | image/apng | image/gif | image/jpeg | image/png | video | video/mp4 | video/quicktime | video/webm | video/x-flv | video/x-m4v | video/x-matroska | video/x-msvideo
    let ret = String();
    // default option
    const advanced_search_option: Record<string, any> = {
        "alts": false,
        "tombstone": false,
        "since": "",
        "order": "desc",
        "mime": [],
    };
    const params = parameters.split(',')

    // Analyzing Advanced Search Options
    let param = undefined;
    for (let idx = 0; idx < params.length; idx++) {
        param = params[idx].split(':');
        if (param[0] === "alts") {
            advanced_search_option["alts"] = true;
        }
        else if (param[0] === "tombstone") {
            advanced_search_option["tombstone"] = true;
        }
        else if (param[0] === "since") {
            advanced_search_option["since"] = param[1];
        }
        else if (param[0] === "order") {
            advanced_search_option["order"] = param[1];
        }
        else if (param[0] === "mime") {
            advanced_search_option["mime"].push(param[1]);
        }
    }
    
    // Generate URL parameters
    if (advanced_search_option["alts"]) {
        ret = ret.concat("&alts=on");
    }
    if (advanced_search_option["tombstone"]) {
        ret = ret.concat("&tombstone=on");
    }
    if (advanced_search_option["since"] !== "") {
        ret = ret.concat("&since=" + advanced_search_option["since"]);
    }
    ret = ret.concat("&order=" + advanced_search_option["order"]);
    for (let idx = 0; idx < advanced_search_option["mime"].length; idx++) {
        ret = ret.concat("&mime=" + encodeURIComponent(advanced_search_option["mime"][idx]));
    }

    return ret;
}

// -------- Custom Function End --------

export const source: ISource = {
    name: "The Permanent Booru",
    modifiers: ["rating:", "meta:", "medium:", "series:", "gender:", "species:", "creator:", "character:"], // Tag types defined in the CSS file
    tagFormat: {
        case: "lower",
        wordSeparator: " ",
    },
    searchFormat: {
        and: {
            separator: ",",
            prefix: "",
        },
        or: {
            separator: ",",
            prefix: "~|",
        },
        parenthesis: false,
        precedence: "or",
    },
    auth: {
        session: {
            type: "post",
            url: "/login/",
            fields: [
                {
                    id: "pseudo",
                    key: "username",
                },
                {
                    id: "password",
                    key: "password",
                    type: "password",
                },
                {
                    // Captcha Challenge Token (Check through the browser's developer tools)
                    // HTML ex. <input type="hidden" name="key" value="{Here is the captcha token value}">
                    // I hope that in the future we can load captcha images from Grabber
                    id: "accessToken",
                    key: "key",
                    type: "text",
                },
                {
                    // Captcha Challenge Answer
                    // Response to captcha image (text inside the image)
                    id: "refreshToken",
                    key: "code",
                    type: "text",
                },
            ],
            check: {
                type: "cookie",
                key: "session",
            },
        },
    },
    apis: {
        html: {
            name: "Regex",
            auth: [],
            forcedLimit: 250, // page per image limit
            forcedTokens: [], // If there is information listed in this list that cannot be retrieved from the image list, open the image detail page to retrieve the information.
            search: {
                parseErrors: true,
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        const vaild_page = Grabber.pageUrl(query.page, previous, Number.MAX_VALUE, "{page}", "{max}", "{min}");
                        // Since this booru receives the search query as multiple parameters instead of one, it is necessary to properly separate them and pass them to the appropriate parameters.
                        // Prefix: (no prefix):And, ~|:Or, ~!:Filter, ~+:Unless
                        const parsed_keyword = search_keyword_parser(query);
                        return "/posts/" + vaild_page + '/' + encodeURIComponent(parsed_keyword["and"]) + "?filter=" + encodeURIComponent(parsed_keyword["filter"]) + "&or=" + encodeURIComponent(parsed_keyword["or"]) + "&unless=" + encodeURIComponent(parsed_keyword["unless"]) + mk_adva_url_para(parsed_keyword["advanced"]); // query.page is deprecated and should be replaced with opts.page in the future.
                    }
                    catch (e: any) {
                        return { error: e.message };
                    }
                },
                parse: (src: string, statusCode: number): IParsedSearch | IError => {
                    if (!src.match(/^<!DOCTYPE html>\s+<html>/)) {
                        return { error: "Server Error(HTTP" + statusCode.toString() + "): " + src };
                    }
                    const images = Grabber.regexToImages('<div\\s+data-context-menu="thumbnail"\\s+data-id="(?<id>\\d+)"\\s+data-sha256="(?<sha256>\\p{Hex_Digit}{64})"\\s+data-md5="(?<md5>\\p{Hex_Digit}{32})"\\s+data-tags="(?<tags>[^"]+)"\\s+data-file-url="(?<file_url>[\\/\\.\\d\\w]+)"\\s+class="thumbnail (image|video|application)\\s+" >\\s+(<img src="(?<preview_url>[\\d\\w\\/\\.]+)" alt="\\p{Hex_Digit}{64}" class="">\\s+<div class="tagbox hint">\\s+<div class="score">\\s+<div><span>[\\w\\d\\s]+</span></div>\\s+<div><span>Score:\\s+(?<score>[\\d.]+)</span></div>)?', src);
                    const imageCount = Grabber.regexMatch('<div id="sidebar">\\s+<div>(?<image_count>\\d+)</div>', src);
                    const page_navigator = Grabber.regexMatch('<div class="paginator">\\s+(<span(><a href="(?<first_page_url>/posts/(?<first_page_number>\\d+)(/[^"]*)?)">First</a>|><a href="(?<previous_page_url>/posts/(?<previous_page_number>\\d+)(/[^"]*)?)">Previous</a>| class="current-page">(?<current_page_number>\\d+)|><a href="(?<next_page_url>/posts/(?<next_page_number>\\d+)(/[^"]*)?)">Next</a>|><a href="(?<last_page_url>/posts/(?<last_page_number>\\d+)(/[^"]*)?)">Last</a>|><a href="/posts/\\d+(/[^"]*)?">\\d+</a>)+</span>\\s+)+</div>', src);
                    const img_tags = Grabber.regexToTags('<tr>\\s+<td class="tag namespace-(?<type>none|rating|meta|medium|series|gender|species|creator|character|[^"]+)">\\s+<span class="tag-toggle" data-tag="[^"]+">\\+</span>\\s+<a href="[^"]+"><span>(?<name>[^"]+)</span></a>\\s+</td>\\s+<td class="counter">(?<count>\\d+)</td>\\s+</tr>', src);
                    images.forEach(tag_refinement);
                    images.forEach(fill_placeholder_preview);
                    images.forEach(fill_rating_from_string_tag);
                    img_tags.forEach(tag_type_compat);
                    img_tags.forEach(recover_html_entity_itag_wrapper);
                    return {
                        images: images,
                        imageCount: imageCount ? imageCount.hasOwnProperty("image_count") ? parseInt(imageCount["image_count"], 10) : 0 : 0,
                        pageCount: page_navigator ? page_navigator.hasOwnProperty("last_page_number") ? parseInt(page_navigator["last_page_number"], 10) : page_navigator.hasOwnProperty("current_page_number") ? parseInt(page_navigator["current_page_number"], 10) : 0 : 0,
                        //wiki: undefined,
                        tags: img_tags,
                        urlNextPage: recover_html_entity(page_navigator ? page_navigator.hasOwnProperty("next_page_url") ? page_navigator["next_page_url"] : undefined : undefined, 1),
                        urlPrevPage: recover_html_entity(page_navigator ? page_navigator.hasOwnProperty("previous_page_url") ? page_navigator["previous_page_url"] : undefined : undefined, 1),
                        //md5: undefined
                    };
                },
            },
            details: {
                fullResults: false,
                url: (id: string, md5: string): string => {
                    //return "/post/md5/" + md5; // view image by md5
                    //return "/post/sha356/" + sha256; // view image by sha256
                    return "/post/" + id;
                },
                parse: (src: string): IParsedDetails => {
                    const img_tags = Grabber.regexToTags('<tr>\\s+<td class="tag namespace-(?<type>none|rating|meta|medium|series|gender|species|creator|character|[^"]+)">\\s+<span class="tag-toggle" data-tag="[^"]+">\\+</span>\\s+<a href="[^"]+"><span>(?<name>[^"]+)</span></a>\\s+</td>\\s+<td class="counter">(?<count>\\d+)</td>\\s+</tr>', src);
                    img_tags.forEach(tag_type_compat);
                    img_tags.forEach(recover_html_entity_itag_wrapper);
                    return {
                        tags: img_tags,
                        imageUrl: Grabber.regexToConst("url", '<a href="(?<url>[\\d\\w\\/\\.]+)" download>Download</a>', src),
                        createdAt: Grabber.regexToConst("date", '<div class="metadata">\\s+<div>[\\w\\d\\/]+</div>\\s+<div title="\\d+">[\\w\\d\\.\\s]+</div>\\s+<div title="[\\w\\d\\s]+">(?<date>\\d{4}-\\d{2}-\\d{2} \\d{2}:\\d{2}:\\d{2})</div>', src),
                        sources: regexToSources('<div title="(?:[\\w]+://)[^"]+">\\s+<a href="(?<url>(?:[\\w]+://)[^"]+)">(?:[\\w]+://)[^<]+</a>\\s+</div>', src),
                        // This booru doesn't seem to show a pool link. If you look at the detailed image, you'll see something that looks like a pool, but it's called an "alt-group," which is similar to a pool but not strictly speaking a pool.
                        //pools: Grabber.regexToPools('~~~~~~', src),
                    };
                },
            },
            /*
             * The Permanent Booru offers both image Booru and gallery services.
             * However, we have decided to temporarily discontinue this feature as image board and gallery cannot be displayed simultaneously in search results.
             * Even if it were possible, there are concerns that it could clutter search results.
             * The commented code below is written in JavaScript and needs to be modified(To TS).
             * Code snippets for the future
             */
            /*
            gallery: {
                url: function (query) {
                    return "/comics/" + query.id;
                },
                parse: function (src) {
                    console.warn(src);
                    var images = Grabber.regexToImages('<div id="diff_\\d*"[^>]*>.*?<img src="(?<file_url>[^"]+)"', src).map(completeImage);
                    return {
                        images: images,
                        pageCount: 1,
                        imageCount: images.length,
                    };
                },
            },
            */
            /*
             * The Permanent Booru does not appear to have a pool list.
             * There is a way to access the pool through the user information page.
             */
            /*
             * Bad news: I checked other image board request URLs with pool functionality via Grabber's pool tab and found that Grabber works by searching for pool:poolID in the basic search query. However, The Permanent Booru doesn't support searching for pools via basic search.
             * In other words, the method of accessing the URL directly through the pool ID in the Pool tab of Grabber must be supported.
             * I found out later that endpoints can support various search modes and supposedly provide advanced features, but I couldn't figure out how or when the endpoint in endpoints are triggered even after checking the models.ts files of other sources.
             */
            /*
            endpoints: {
                pool_list: {
                    name: "Pools",
                    input: {},
                    url: function (query, opts) {
                        var pid = (opts.page - 1) * 25;
                        console.warn(query, opts);
                        return "/index.php?page=pool&s=list&pid=" + String(pid);
                    },
                    parse: function (src) {
                        var html = Grabber.parseHTML(src);
                        var images = [];
                        var rows = html.find("table tr");
                        for (var _i = 0, rows_1 = rows; _i < rows_1.length; _i++) {
                            var row = rows_1[_i];
                            var parts = row.find("td");
                            var link = parts[1].find("a")[0];
                            var id = link.attr("href").match(/id=(\d+)/)[1];
                            images.push({
                                id: id,
                                name: link.innerText(),
                                type: "gallery",
                                gallery_count: parts[2].innerText().match(/(\d+)\s+Images/)[1],
                                details_endpoint: {
                                    endpoint: "pool_details",
                                    input: { id: id },
                                },
                            });
                        }
                        return { images: images };
                    },
                },
                pool_details: {
                    input: {
                        id: {
                            type: "input",
                        },
                    },
                    url: function (query) {
                        console.info(query);
                        return "/index.php?page=pool&s=show&id=" + String(query.id);
                    },
                    parse: function (src) {
                        // The regular expression below conflicts with the comment mark, so inline comments are used.
                        *///var images = Grabber.regexToImages('<span[^>]*(?: id="?\\w(?<id>\\d+)"?)?>\\s*<a[^>]*(?: id="?\\w(?<id_2>\\d+)"?)[^>]*>\\s*<img [^>]*(?:src|data-original)="(?<preview_url>[^"]+/thumbnail_(?<md5>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags>[^"]+)"[^>]*/?>\\s*</a>|<img\\s+class="preview"\\s+src="(?<preview_url_2>[^"]+/thumbnail_(?<md5_2>[^.]+)\\.[^"]+)" [^>]*title="\\s*(?<tags_2>[^"]+)"[^>]*/?>', src);
                        /*return {
                            images: images.map(completeImage),
                        };
                    },
                },
            },
            */
            tags: {
                url: (query: ITagsQuery, opts: IUrlOptions): string => {
                    return "/tags/" + query.page; // query.page is deprecated and should be replaced with opts.page in the future.
                },
                parse: (src: string): IParsedTags => {
                    const parsed_tags = Grabber.regexToTags('<div class="tag-toggle" data-tag="[^"]+">\\+</div>\\s+<div class="tag namespace-(?<type>none|rating|meta|medium|series|gender|species|creator|character|[^"]+)"><a href="/tags/\\d+/(?<id>\\d+)">(?<name>[^<]+)</a></div>\\s+<div class="count">(?<count>\\d+)</div>', src);
                    parsed_tags.forEach(tag_type_compat);
                    parsed_tags.forEach(recover_html_entity_itag_wrapper);
                    return {
                        tags: parsed_tags,
                    };
                },
            },
            /*
             * The Permanent Booru only displays a list of tags. There is no page where you can search by tag category.
             * The commented code below is written in JavaScript and needs to be modified(to TS).
             * Code snippets for the future
             */
            /*
            tagTypes: {
                url: function () {
                    return "/tag";
                },
                parse: function (src) {
                    var contents = src.match(/<select[^>]* name="type"[^>]*>([\s\S]+)<\/select>/);
                    if (!contents) {
                        return { error: "Parse error: could not find the tag type <select> tag" };
                    }
                    var results = Grabber.regexMatches('<option value="(?<id>\\d+)">(?<name>[^<]+)</option>', contents[1]);
                    var types = results.map(function (r) { return ({
                        id: r.id,
                        name: r.name.toLowerCase(),
                    }); });
                    return { types: types };
                },
            },
            */
            check: {
                url: (): string => {
                    return "/";
                },
                parse: (src: string): boolean => {
                    return src.search('<div class="logo">The Permanent Booru</div>') !== -1;
                },
            },
        },
        json: {
            name: "JSON",
            auth: [],
            maxLimit: 50,
            search: {
                parseErrors: true,
                url: (query: ISearchQuery, opts: IUrlOptions, previous: IPreviousSearch | undefined): string | IError => {
                    try {
                        // Since this booru receives the search query as multiple parameters instead of one, it is necessary to properly separate them and pass them to the appropriate parameters.
                        // Prefix: (no prefix):And, ~|:Or, ~!:Filter, ~+:Unless
                        const vaild_page = Grabber.pageUrl(query.page, previous, Number.MAX_VALUE, "{page}", "{max}", "{min}") - 1; // start at 0
                        const parsed_keyword = search_keyword_parser(query);
                        return "/api/v1/posts?" + "offset=" + vaild_page + "&tags=" + encodeURIComponent(parsed_keyword["and"]) + "&filter=" + encodeURIComponent(parsed_keyword["filter"]) + "&or=" + encodeURIComponent(parsed_keyword["or"]) + "&unless=" + encodeURIComponent(parsed_keyword["unless"]) + mk_adva_url_para(parsed_keyword["advanced"]); // query.page is deprecated and should be replaced with opts.page in the future.
                    } catch (e: any) {
                        return { error: e.message };
                    }
                },
                parse: (src: string): IParsedSearch | IError => {
                    const data = JSON.parse(src);
                    if (data["Error"]) {
                        return { error: data["Error"] + '(' + data["Code"] + ')'};
                    }
                    const image_count = parseInt(data["TotalPosts"], 10);
                    const images: IImage[] = [];
                    for (let i = 0; i < data["Posts"].length; i++) {
                        images.push(jason_to_iimage(data["Posts"][i]));
                    }
                    images.forEach(fill_rating_from_string_tag);
                    images.forEach(fill_placeholder_preview);
                    return {
                        images: images,
                        imageCount: image_count,
                        pageCount: Math.ceil(image_count / 50),
                    };
                },
            },
        },
    },
};
