#pragma once

#define _WIN32_WINNT 0x0501
#include <windows.h>
#include <wlanapi.h>
#include <map>
#include <string>

#ifdef NATIVE_WIFI_CONNECT_EXPORTS
#define NATIVE_WIFI_CONNECT_API __declspec(dllexport)
#else
#define NATIVE_WIFI_CONNECT_API __declspec(dllimport)
#endif /* NATIVE_WIFI_CONNECT_EXPORTS */

class NATIVE_WIFI_CONNECT_API NativeWifiConnect
{
public:
	NativeWifiConnect();
	~NativeWifiConnect();

	/*func 打开并搜索网络列表
	@param wlanMap 本地网络列表，key-网络名，value-信号强度
	@return 成功返回true,失败返回false	*/
	bool  openWLAN(std::map<std::string, int> &wlanMap);

	/*func 直接连接已保存过密码的Wifi
	@return 成功返回true,失败返回false	*/
	bool  connectWLAN(const std::string wlanName);

	/*func 通过密码连接网络
	@return 成功返回true,失败返回false	*/
	bool  passwordToConnectWLAN(const std::string wlanName, const std::string password);

	/*func 刷新网络
	@param wlanMap 本地网络列表，key-网络名，value-信号强度
	@return 成功返回true,失败返回false	*/
	bool  refreshWLAN(std::map<std::string, int> &wlanMap);

	/*func 查询当前连接网络
	@param connectedWifi 当前连接网络名.若当前未连接网络，此值为空。
	@param signalQuality 当前连接网络信号强度
	@return 查询成功返回true,失败返回false	*/
	bool  queryInterface(std::string &connectedWifi,int& signalQuality);

	bool  disConnect();
	bool  closeWLAN();
	std::string getError()const;
private:
	//不要忘记在使用完wchar_t*后delete[]释放内存
	wchar_t*  stringToWideChar(const std::string pKey);

	bool  setProfile(const std::string wlanName, const std::string password);
	void  stringReplace(std::string &inputoutput, const std::string oldStr, const std::string newStr);

	HANDLE hClientHandle = NULL;
	GUID guid;
	// variables used for WlanEnumInterfaces
	PWLAN_INTERFACE_INFO_LIST  pInterfaceList=NULL;
	PWLAN_INTERFACE_INFO pIfInfo = NULL;

	// variables used for WlanQueryInterfaces for opcode = wlan_intf_opcode_current_connection
	PWLAN_CONNECTION_ATTRIBUTES pConnectInfo = NULL;
	DWORD connectInfoSize;
	WLAN_OPCODE_VALUE_TYPE opCode;

	DWORD dwResult = 0;
	std::string STR_NAME;
	std::string STR_PASSWORD;
	std::string STR_PROFILE_DEMO;
};