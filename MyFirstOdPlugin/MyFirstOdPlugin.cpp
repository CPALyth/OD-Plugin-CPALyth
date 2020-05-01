
#include "pch.h"
#include "Plugin.h"

//************************************
// Method:�˵�����ʾ�Ĳ����
// Description: ����ĵ�������
//************************************
extern "C" __declspec(dllexport) cdecl int ODBG_Plugindata(char* shortName)
{
	_Mergequicknames();
	const char* pluginName = "FuncNameLabel";
	strcpy_s(shortName, strlen(pluginName) + 1, pluginName);
	return PLUGIN_VERSION;
}

//OD��������
HWND g_hOllyDbg;
//************************************
// Method:�����ʼ���������жϵ�ǰOD�汾�źͲ����֧�ֵİ汾�Ƿ�һ��
// Description:����ĵ������� 
//************************************
extern "C" __declspec(dllexport) cdecl int ODBG_Plugininit(int ollydbgversion, HWND hw, ulong *features)
{

	if (ollydbgversion < PLUGIN_VERSION)
	{
		MessageBox(hw, "�������֧�ֵ�ǰ�汾OD!", "MyFirstPlugin", MB_ICONERROR);
		return -1;
	}
	g_hOllyDbg = hw;
	return 0;
}

//************************************
// Method:��ʾ�˵���
// Description:��ʾ��Ӧ�Ĳ˵�ѡ��
//************************************
extern "C" __declspec(dllexport) cdecl int  ODBG_Pluginmenu(int origin, TCHAR data[4096], VOID *item)
{

	if (origin == PM_MAIN)		// ���˵����
	{
		strcpy(data, "0&�����˵��Ӳ˵�һ,1&�����˵��Ӳ˵���");
	}
	if (origin == PM_DISASM)
	{
		strcpy(data, "����Ҽ����˵�{0&����Ҽ��Ӳ˵�һ{3&�����˵�1,4&�����˵�2},1&����Ҽ��Ӳ˵���}");
	}
	return 1;
}

//************************************
// Method:�˵�����ִ�к���
// Description:���еĲ˵���������ִ�е��������
//************************************
extern "C" __declspec(dllexport) cdecl void ODBG_Pluginaction(int origin, int action, VOID *item)
{
	//������������ڵ��
	if (origin == PM_MAIN)
	{
		if (action == 0)
		{
			MessageBoxA(g_hOllyDbg, "�����˵��Ӳ˵�һ", "www.bcdaren.com", MB_ICONINFORMATION);
		}
		if (action == 1)
		{
			MessageBoxA(g_hOllyDbg, "�����˵��Ӳ˵���", "www.bcdaren.com", MB_ICONINFORMATION);
		}
	}
	//������ڷ���ര�ڵ��
	if (origin == PM_DISASM)
	{
		if (action == 0)
		{
			MessageBoxA(g_hOllyDbg, "����Ҽ��Ӳ˵�һ", "www.bcdaren.com", MB_ICONINFORMATION);
		}
		if (action == 1)
		{
			MessageBoxA(g_hOllyDbg, "����Ҽ��Ӳ˵���", "www.bcdaren.com", MB_ICONINFORMATION);
		}
	}
}