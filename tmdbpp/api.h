#ifndef TMDBPP_API_H
#define TMDBPP_API_H

#include <tmdbpp/util.h>
#include <tmdbpp/search.h>
#include <tmdbpp/get.h>
#include <tmdbpp/apiagent.h>


#include <string>
#include <map>

namespace tmdbpp {
    
    /** @short Central manager of the TMDBPP interface. Holds the API key
        and the configuration for a given connection.

        An Api can not be constructed directly but has to be retrieved
        via the instance() method.

        @see Configuration, ApiAgent
    */

    class Api {
    public:
        static const std::string BaseUrl;
        static const std::string MethodConfig;
        static const std::string MethodSearch;
        static const std::string MethodMovie;
        static const std::string MethodCollection;
        static const std::string MethodCompany;
        static const std::string MethodKeyword;
        static const std::string MethodTv;
        static const std::string MethodPerson;
        static const std::string MethodGenre;
        static const std::string MethodAuthentication;
        static const std::string MethodTimezones;

        static const std::string ObjectCompany;
        static const std::string ObjectMovie;
        static const std::string ObjectCollection;
        static const std::string ObjectKeyword;
        static const std::string ObjectTv;
        static const std::string ObjectPopular;
        static const std::string ObjectTopRated;
        static const std::string ObjectAiringToday;
        static const std::string ObjectOnTheAir;
        static const std::string ObjectPerson;
        static const std::string ObjectCredits;
        static const std::string ObjectMovieCredits;
        static const std::string ObjectTvCredits;
        static const std::string ObjectCombinedCredits;
        static const std::string ObjectList;
        static const std::string ObjectToken;
        static const std::string ObjectSession;

        static const std::string OptionTv;
        static const std::string OptionNew;
        static const std::string OptionValidateWithLogin;

        enum StatusCode {
            StatusInvalidId    =  6,
            StatusDenied       =  17,
            StatusInvalidLogin =  30,
        };

        /** @short Factory function: Retrieve an API instance for the given
            API Key.

            - Behaves like an "per-key" singleton function. No new instance will
            be created for the same key.

            - Calling with no key will try to pull the api key from the "apikey"
            environment variable.
        */

        static Api & instance(const std::string & apiKey="")  {
            static std::map<std::string,std::shared_ptr<Api>> m;
            std::string k=apiKey;
            if(k.empty()) {
                if(::getenv("apikey") == NULL ) {
                    throw std::runtime_error(" no env apikey set");
                }
                k = std::string(::getenv("apikey"));
            }


            if(m.find(k)==m.end()) {
                m[k] = std::shared_ptr<Api>(new Api(k));
            }

            return *m[k];
        }
        const Configuration & configuration() const {
            return _config;
        }
        Url url() const {
            return Url(BaseUrl).add(UrlArg("api_key",key()));
        }

        const std::string & key() const {
            return _key;
        }

        Search  search() {
            return Search(*this);
        }

        Get get() {
            return Get(*this);
        }

        const ErrorStatus status() const {
            return _status;
        }

    private:

        friend class ApiAgent;

        /** @short Most basic fetch() method for an ApiAgent. Retrieve
            an URL addressed data as a string.

            - Returns empty string if the requested record it not available

            - Throws an exception if the I/O layer (i.e. network error) fails.
         */

        std::string fetch(const std::string & url);

        Api(const std::string & apiKey) : _key(apiKey) {
            ApiAgent ag(*this);

            JSonMapper::ptree_t p0;
            JSonMapper::ptree_t p1;

            Url u0 = this->url().add(MethodConfig);
            Url u1 = this->url().add(MethodTimezones).add(ObjectList);

            std::stringstream s0(ag.fetch(u0));
            std::stringstream s1(ag.fetch(u1));

            boost::property_tree::read_json(s0,p0);
            boost::property_tree::read_json(s1,p1);
            
            _config = Configuration(p0,p1);

            std::cerr << " ++ " << _config.timezones().size() << std::endl;
        }

        std::string   _key;
        Configuration _config;
        ErrorStatus   _status;
    };
}

#endif // API_H
