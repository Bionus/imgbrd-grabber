// requires Windows 10, Windows 7 Service Pack 1, Windows 8, Windows 8.1, Windows Server 2003 Service Pack 2, Windows Server 2008 R2 SP1, Windows Server 2008 Service Pack 2, Windows Server 2012, Windows Vista Service Pack 2, Windows XP Service Pack 3

// Visual C++ Redistributable for Visual Studio 2015 Update 3 (14.0.24210)
// https://www.visualstudio.com/downloads/download-visual-studio-vs#d-visual-c

[CustomMessages]
vcredist2015_title=Microsoft Visual C++ 2015 Update 3 Redistributable (x86)
vcredist2015_title_x64=Microsoft Visual C++ 2015 Update 3 Redistributable (x64)

en.vcredist2015_size=13.7 MB
de.vcredist2015_size=13,7 MB

en.vcredist2015_size_x64=14.5 MB
de.vcredist2015_size_x64=14,5 MB


[Code]
const
  vcredist_version_major = 14;
  vcredist_version_minor = 0;
  vcredist_version_build = 24210;

    vcredist2015_url = 'http://download.microsoft.com/download/d/e/c/dec58546-c2f5-40a7-b38e-4df8d60b9764/vc_redist.x86.exe';
    vcredist2015_url_x64 = 'http://download.microsoft.com/download/2/c/6/2c675af0-2155-4961-b32e-289d7addfcec/vc_redist.x64.exe';

    vcredist2015_productcode = '{8FD71E98-EE44-3844-9DAD-9CB0BBBC603C}';
    vcredist2015_productcode_x64 = '{C0B2C673-ECAA-372D-94E5-E89440D087AD}';

  vcredist2015_upgradecode = '{65E5BD06-6392-3027-8C26-853107D3CF1A}';
  vcredist2015_upgradecode_x64 = '{36F68A90-239C-34DF-B58C-64B30153CE35}';

procedure vcredist2015();
begin
    if (not IsIA64()) then begin
        if (not MsiProductInstalled(GetString(vcredist2015_upgradecode, vcredist2015_upgradecode_x64, ''), vcredist_version_build)) then
            AddProduct('vcredist2015' + GetArchitectureString() + '.exe',
                '/passive /norestart',
                CustomMessage('vcredist2015_title' + GetArchitectureString()),
                CustomMessage('vcredist2015_size' + GetArchitectureString()),
                GetString(vcredist2015_url, vcredist2015_url_x64, ''),
                false, false, false);
    end;
end;