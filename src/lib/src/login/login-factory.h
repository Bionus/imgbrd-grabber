#ifndef LOGIN_FACTORY_H
#define LOGIN_FACTORY_H

class Auth;
class Login;
class NetworkManager;
class Site;


/**
 * Factory to build Login instances.
 */
class LoginFactory
{
	public:
		/**
		 * Build a Login from an Auth object.
		 *
		 * @param site The site to build the Login instance for.
		 * @param auth The Auth instance containing all necessary information to build the Login instance.
		 * @param manager The network manager to use in the Login class for HTTP requests.
		 * @return A new Login instance on success, `nullptr` on failure.
		 */
		static Login *build(Site *site, Auth *auth, NetworkManager *manager);
};

#endif // LOGIN_FACTORY_H
