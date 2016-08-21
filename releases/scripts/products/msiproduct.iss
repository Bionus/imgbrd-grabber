[Code]
#IFDEF UNICODE
    #DEFINE AW "W"
#ELSE
    #DEFINE AW "A"
#ENDIF

type
    INSTALLSTATE = Longint;
const
    INSTALLSTATE_INVALIDARG = -2;  // An invalid parameter was passed to the function.
    INSTALLSTATE_UNKNOWN = -1;     // The product is neither advertised or installed.
    INSTALLSTATE_ADVERTISED = 1;   // The product is advertised but not installed.
    INSTALLSTATE_ABSENT = 2;       // The product is installed for a different user.
    INSTALLSTATE_DEFAULT = 5;      // The product is installed for the current user.

function MsiQueryProductState(szProduct: string): INSTALLSTATE;
external 'MsiQueryProductState{#AW}@msi.dll stdcall';

function msiproduct(const ProductID: string): boolean;
begin
    Result := MsiQueryProductState(ProductID) = INSTALLSTATE_DEFAULT;
end;

function MsiEnumRelatedProducts(szUpgradeCode: String; nReserved: DWORD; nIndex: DWORD; szProductCode: String): Integer;
external 'MsiEnumRelatedProducts{#AW}@msi.dll stdcall';

function MsiGetProductInfo(szProductCode: String; szProperty: String; szValue: String; var nValueBufSize: DWORD): Integer;
external 'MsiGetProductInfo{#AW}@msi.dll stdcall';

function MsiProductInstalled(szUpgradeCode: String; nProductVersion: DWORD): Boolean;
var
    szProductCode, szValue, szBuild: String;
    nValueBufSize, nBuild: DWORD;
begin
    SetLength( szProductCode, 39 );
    SetLength( szValue, 39 );
    nValueBufSize := Length( szValue );
    Result := false;

    if ( MsiEnumRelatedProducts( szUpgradeCode, 0, 0, szProductCode ) = 0 ) then begin
        if ( MsiGetProductInfo( szProductCode, 'VersionString', szValue, nValueBufSize ) = 0 ) then begin
            szBuild := ExtractFileExt( szValue );
            szBuild := Copy( szBuild, 2, Length( szBuild ) - 1 );
            nBuild := StrToInt( szBuild );
            if ( nBuild >= nProductVersion ) then begin
                Result := true;
            end;
        end;
    end;
end;

[Setup]
