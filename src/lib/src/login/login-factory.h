#ifndef LOGIN_FACTORY_H
#define LOGIN_FACTORY_H

class Auth;
class Login;
class NetworkManager;
class Site;


class LoginFactory
{
	public:
		static Login *build(Site *site, Auth *auth, NetworkManager *manager);
};

#endif // LOGIN_FACTORY_H
