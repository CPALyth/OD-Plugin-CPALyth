
#include "pch.h"
#include "Plugin.h"

HWND g_hOllyDbg;	// OD��������
bool isLogging = false;	// �Ƿ������־

void RenameCall(t_dump* pDump);
bool Str_IsBeginWith(const char* str1, const char* str2);
void logJcc();

/* [2020/05/02 23:11]-[Remark: ����ĵ�������] */
/* [�˵�����ʾ�Ĳ����]-[Return:None] */
extc __declspec(dllexport) cdecl int ODBG_Plugindata(char* shortName)
{
	_Mergequicknames();
	const char* pluginName = "CPALyth";
	strcpy_s(shortName, strlen(pluginName) + 1, pluginName);
	return PLUGIN_VERSION;
}

/* [2020/05/02 23:11]-[Remark: ����ĵ�������] */
/* [�����ʼ���������жϵ�ǰOD�汾�źͲ����֧�ֵİ汾�Ƿ�һ��]-[Return:None] */
extc __declspec(dllexport) cdecl int ODBG_Plugininit(int ollydbgversion, HWND hw, ulong *features)
{

	if (ollydbgversion < PLUGIN_VERSION)
	{
		MessageBox(hw, "�������֧�ֵ�ǰ�汾OD!", "CPALyth", MB_ICONERROR);
		return -1;
	}
	g_hOllyDbg = hw;
	return 0;
}

/* [2020/05/02 23:12]-[Remark: ��ʾ��Ӧ�Ĳ˵�ѡ��] */
/* [��ʾ�˵���]-[Return:None] */
extc __declspec(dllexport) cdecl int  ODBG_Pluginmenu(int origin, TCHAR data[4096], VOID *item)
{

	if (origin == PM_MAIN)		// ���˵����
	{
		strcpy(data, "0&JCCָ���¼��,1&ֹͣ��¼JCCָ��");
	}
	if (origin == PM_DISASM)	// ����ര�ڵ��
	{
		strcpy(data, "0&�������ú���");
	}
	if (origin == PM_MEMORY)	// �ڴ洰�ڵ��
	{
		strcpy(data, "0&�������ú���");
	}
	return 1;
}

/* [2020/05/02 23:14]-[Remark: ���еĲ˵���������ִ�е��������] */
/* [�˵�����ִ�к���]-[Return:None] */
extc __declspec(dllexport) cdecl void ODBG_Pluginaction(int origin, int action, VOID *item)
{
	// ������������ڵ��
	if (origin == PM_MAIN)
	{
		if (action == 0)
		{
			char szSug[100] = { 0 };
			strcpy_s(szSug, "********** CPALyth Jccָ���¼��ʼ **********");
			_Addtolist(0, 0, szSug);

			logJcc();
		}
		if (action == 1)
		{
			isLogging = false;

			char szSug[100] = { 0 };
			strcpy_s(szSug, "********** CPALyth Jccָ���¼���� **********");
			_Addtolist(0, 0, szSug);
			//MessageBoxA(g_hOllyDbg, "��¼�Ѿ�ֹͣ", "CPALyth", MB_ICONINFORMATION);
		}
	}
	// ������ڷ���ര�ڵ��
	if (origin == PM_DISASM)
	{
		if (action == 0)
		{
			t_dump* pDump = (t_dump*)item;
			RenameCall(pDump);
		}
	}
	// ��������ڴ洰�ڵ��
	if (origin == PM_MEMORY)
	{

	}
}

/* [2020/05/01 16:06]-[Remark: None] */
/* [CALL����������]-[Return:None] */
void RenameCall(t_dump* ptDump)
{
	DWORD dwSelAddr = ptDump->sel0;
	if (dwSelAddr == 0)	return;

	uchar pBuffer[MAXCMDSIZE];
	// ��ȡѡ�еĵ�һ�еĻ�����
	_Readmemory(pBuffer, dwSelAddr, MAXCMDSIZE, MM_SILENT);
	// ����һ�����������
	t_disasm td;
	// ��ѡ�н��з����
	ulong lSize = _Disasm(pBuffer, 16, dwSelAddr, NULL, &td, DISASM_ALL, NULL);
	if (!Str_IsBeginWith("call", td.result))	return;		// ����CALL�ͷ���
	uchar bufOffset[4];
	_Readmemory(bufOffset, dwSelAddr + 1, 4, MM_SILENT);
	// ת����
	int nOffset;
	memcpy_s(&nOffset, 4, bufOffset, 4);
	// ����CALL��Ŀ���ַ
	int callTargetAddr = dwSelAddr + nOffset + 5;
	// ��ȡĿ���ַ�ı�ǩ���ݵ�szUserInput
	char szUserInput[TEXTLEN] = { 0 };
	_Findlabel(callTargetAddr, szUserInput);	
	// �����Ի���,���û������µı�ǩ
	if (_Gettext((char*)"����������",szUserInput,0,NM_NONAME,0) != -1)
	{
		// �����ǩ
		_Insertname(callTargetAddr, NM_LABEL, szUserInput);
	}
}

/* [2020/05/02 22:12]-[Remark: None] */
/* [���º󶼻���õĻص�����]-[Return:None] */
extc int ODBG_Pausedex(int reason, int extdata, t_reg *reg, DEBUG_EVENT* debugEvent)
{
	if (!isLogging)
	{
		return 1;
	}
	if (reason == PP_SINGLESTEP)
	{
		// �õ�����ര�ڵĽṹ��
		t_dump* pDump = (t_dump*)_Plugingetvalue(VAL_CPUDASM);
		// ��ȡCPU�е�ǰѡ�е�ָ���
		ulong selectLen = pDump->sel1 - pDump->sel0;
		// ����һ�����������
		t_disasm td;
		uchar cmd[MAXCMDSIZE];
		// ��ȡCPU��ǰѡ�е�ʮ������
		_Readmemory(&cmd, pDump->sel0, selectLen, MM_SILENT);
		// �����ѡ�е�ʮ������
		_Disasm(cmd, selectLen, pDump->addr, NULL, &td, DISASM_ALL, _Plugingetvalue(VAL_MAINTHREADID));
		if (Str_IsBeginWith("j", td.result))	// ָ����j��ͷ
		{
			if (!Str_IsBeginWith("jmp", td.result))	// ��ָ�Ϊjmp,��������תָ��
			{

				if (strcmp(td.opinfo[1], "��ת��ʵ��") == 0)
				{
					_Addtolist(pDump->sel0, 0, td.opinfo[1]);
				}
				else
				{
					_Addtolist(pDump->sel0, 1, td.opinfo[1]);
				}
			}
		}
		_Go(0, 0, STEP_IN, 0, 0);
	}
	return 1;
}

/* [2020/05/02 21:30]-[Remark: ��¼�����ܹ���ָ��,��¼��֪�����κ���Ϣ] */
/* [��¼������תָ�����ת���]-[Return:None] */
void logJcc()
{
	isLogging = true;
	_Go(0, 0, STEP_IN, 0, 0);
}

/* [2020/05/01 18:49]-[Remark: None] */
/* [�ж�ĸ��str2�Ƿ����Ӵ�str1��ʼ]-[Return:None] */
bool Str_IsBeginWith(const char* str1, const char* str2)
{
	size_t lenpre = strlen(str1);	// �Ӵ�
	size_t lenstr = strlen(str2);	// ĸ��
	return lenstr < lenpre ? false : strncmp(str1, str2, lenpre) == 0;
}