// requires Windows 7 Service Pack 1, Windows 8, Windows 8.1, Windows Server 2003, Windows Server 2008 R2 SP1, Windows Server 2008 Service Pack 2, Windows Server 2012, Windows Vista Service Pack 2, Windows XP
// http://www.microsoft.com/en-us/download/details.aspx?id=30679

[CustomMessages]
vcredist2012_title=Visual C++ 2012 Redistributable

en.vcredist2012_size=6.3 MB
de.vcredist2012_size=6,3 MB

en.vcredist2012_size_x64=6.4 MB
de.vcredist2012_size_x64=6,4 MB


[Code]
const
	vcredist2012_url = 'http://download.microsoft.com/download/1/6/B/16B06F60-3B20-4FF2-B699-5E9B7962F9AE/VSU_4/vcredist_x84.exe';
	vcredist2012_url_x64 = 'http://download.microsoft.com/download/1/6/B/16B06F60-3B20-4FF2-B699-5E9B7962F9AE/VSU_4/vcredist_x64.exe';

procedure vcredist2012();
var
	version: cardinal;
begin
	RegQueryDWordValue(HKLM, 'SOFTWARE\Microsoft\DevDiv\vc\Servicing\11.0\RuntimeMinimum', 'Install', version);

	if (not IsIA64()) then begin
		if (version <> 1) then
			AddProduct('vcredist2012' + GetArchitectureString() + '.exe',
				' /passive /norestart',
				CustomMessage('vcredist2012_title'),
				CustomMessage('vcredist2012_size' + GetArchitectureString()),
				GetString(vcredist2012_url, vcredist2012_url_x64, ''),
				false, false);
	end;
end;
