#ifndef FERMAT_COMMON_ENV_H_
#define FERMAT_COMMON_ENV_H_

#include <string>
#include <cstdint>

namespace fermat {
	/*!
	*@brief  This class provides access to environment variables
	* and some general system information.
	*/
class  Environment {
public:
	
	/*!
	* get the given name environment variable
	*@return the value of the environment variable
	*@param name with the given name
	*@note if the variable does not exist return a empty string object.
	*/
	static std::string get(const std::string& name);
	
	/*!	
	*with the given name. If the environment variable 
	*is undefined, returns defaultValue instead.	
	*@eturn the value of the environment variable
	*@param name with the given name
	*@param defaultValue with the given name
	*/
	static std::string get(const std::string& name, const std::string& defaultValue);
	
	/*!
	*test if the given enviroment variable exists
	* @return true iff an environment variable
	* @param name  the given name.
	*/
	static bool has(const std::string& name);
		
	/*!
	* Sets the environment variable with the given name
	* to the given value.	
	*@retuen option set enviroment true or false
	*@param name enviroment variable name to set
	*@param value the value of enviroment variable 
	*/
	static bool set(const std::string& name, const std::string& value);
	
	/*!
	*get os name
	*@returns the operating system name.
	*/
	static std::string os_name();
	
	
	/*!
	*get os version
	*@returns  the operating system version.
	*/
	static std::string os_version();
	
	/*!
	*get os architecure
	*@returns   the operating system architecture.
	*/
	static std::string os_architecture();

	/*!
	*get host name
	*@returns  the node (or host) name.
	*/	
	static std::string node_name();

	/*!
	*@returns  the processors had been install in system,
	* if it can not be count it it return just 1. 0? mean no brain?
	*/	
	static int processor_count();
		
};


} 


#endif 

