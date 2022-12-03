/*
    ��Ȩ������
    ���ļ���ȨΪ�����������������У�����Ȩ����������������������֧�ֿ⣬��ֹ���������κγ��ϡ�
*/
#include "fnshare.h"

INT g_nLastNotifyResult;
PFN_NOTIFY_SYS g_fnNotifySys = NULL;

//add by liigo
PFN_ON_SYS_NOTIFY g_fn_OnSysNotify = NULL;
/*
�û���ʹ�����´��뼴�ɱ�֤���޸ı��ļ������ﵽ��ʱ����ϵͳͨѶ��Ŀ�ģ�

void My_OnSysNotify (INT nMsg, DWORD dwParam1, DWORD dwParam2)
{
}

extern PFN_ON_SYS_NOTIFY g_fn_OnSysNotify;
g_fn_OnSysNotify = (PFN_ON_SYS_NOTIFY) My_OnSysNotify;

*/
//end add

INT WINAPI ProcessNotifyLib (INT nMsg, DWORD dwParam1, DWORD dwParam2)
{
	INT nRet = NR_OK;
	switch (nMsg)
	{
	case NL_SYS_NOTIFY_FUNCTION:
		g_fnNotifySys = (PFN_NOTIFY_SYS)dwParam1;
		break;
	case NL_FREE_LIB_DATA:
		break;
	default:
		nRet = NR_ERR;
		break;
	}

	//�����û�����
	if(g_fn_OnSysNotify) 
		nRet = g_fn_OnSysNotify(nMsg,dwParam1,dwParam2);

	return nRet;
}

INT WINAPI eapi_NotifySys (INT nMsg, DWORD dwParam1, DWORD dwParam2)
{
	if (g_fnNotifySys != NULL)
		return g_nLastNotifyResult = g_fnNotifySys (nMsg, dwParam1, dwParam2);
	else
		return g_nLastNotifyResult = 0;
}

// ʹ��ָ���ı����ݽ����׳�����ʹ�õ��ı����ݡ�
char* eapi_CloneTextData (char* ps)
{
    if (ps == NULL || *ps == '\0')
        return NULL;

    INT nTextLen = strlen (ps);
    char* pd = (char*)eapi_NotifySys (NRS_MALLOC, (DWORD)(nTextLen + 1), 0);
    memcpy (pd, ps, nTextLen);
    pd [nTextLen] = '\0';
    return pd;
}

// ʹ��ָ���ı����ݽ����׳�����ʹ�õ��ı����ݡ�
//   nTextLen����ָ���ı����ֵĳ��ȣ������������㣩��
// ���Ϊ-1����ȡps��ȫ�����ȡ�
char* eapi_CloneTextData (char* ps, INT nTextLen)
{
    if (nTextLen <= 0)
        return NULL;

    char* pd = (char*)eapi_NotifySys (NRS_MALLOC, (DWORD)(nTextLen + 1), 0);
    memcpy (pd, ps, nTextLen);
    pd [nTextLen] = '\0';
    return pd;
}

// ʹ��ָ�����ݽ����׳�����ʹ�õ��ֽڼ����ݡ�
LPBYTE eapi_CloneBinData (LPBYTE pData, INT nDataSize)
{
    if (nDataSize == 0)
        return NULL;

    LPBYTE pd = (LPBYTE)eapi_NotifySys (NRS_MALLOC, (DWORD)(sizeof (INT) * 2 + nDataSize), 0);
    *(LPINT)pd = 1;
    *(LPINT)(pd + sizeof (INT)) = nDataSize;
    memcpy (pd + sizeof (INT) * 2, pData, nDataSize);
    return pd;
}

// ��������ʱ����
void eapi_GReportError (char* szErrText)
{
    eapi_NotifySys (NRS_RUNTIME_ERR, (DWORD)szErrText, 0);
}

void* eapi_MMalloc (INT nSize)
{
	return (void*)eapi_NotifySys (NRS_MALLOC, (DWORD)nSize, 0);
}

void eapi_MFree (void* p)
{
	eapi_NotifySys (NRS_MFREE, (DWORD)p, 0);
}

// ������������ݲ����׵�ַ����Ա��Ŀ��
LPBYTE eapi_GetAryElementInf (void* pAryData, LPINT pnElementCount)
{
	LPINT pnData = (LPINT)pAryData;
	INT nArys = *pnData++;  // ȡ��ά����
	// �����Ա��Ŀ��
	INT nElementCount = 1;
	while (nArys > 0)
	{
		nElementCount *= *pnData++;
		nArys--;
	}

	if (pnElementCount != NULL)
		*pnElementCount = nElementCount;
	return (LPBYTE)pnData;
}

#define DTT_IS_NULL_DATA_TYPE   0
#define DTT_IS_SYS_DATA_TYPE    1
#define DTT_IS_USER_DATA_TYPE   2
#define DTT_IS_LIB_DATA_TYPE    3
// ȡ���������͵����
INT eapi_GetDataTypeType (DATA_TYPE dtDataType)
{
	if (dtDataType == _SDT_NULL)
		return DTT_IS_NULL_DATA_TYPE;

	DWORD dw = dtDataType & 0xC0000000;
	return dw == DTM_SYS_DATA_TYPE_MASK ? DTT_IS_SYS_DATA_TYPE :
			dw == DTM_USER_DATA_TYPE_MASK ? DTT_IS_USER_DATA_TYPE :
			DTT_IS_LIB_DATA_TYPE;
}


//������liigo����


/*
//!!! ���º�����������������֮��Աȫ��ΪINT����(��ͬ��ռ��4���ֽ�)
//index: 0..*/


_private void* eapi__GetPointerByIndex(void *pBase, int index)
{
	return (void*) ((INT)pBase + index * sizeof(INT));
}

_private INT eapi__GetIntByIndex(void *pBase, INT index)
{
	return *((INT*)eapi__GetPointerByIndex(pBase,index));
}

_private void eapi__SetIntByIndex(void *pBase, INT index, INT value)
{
	*((INT*)eapi__GetPointerByIndex(pBase,index)) = value;
}


INT eapi_GetIntByIndex(PMDATA_INF pArgInf, INT index)
{
	return eapi__GetIntByIndex(pArgInf->m_pCompoundData,index);
}

void eapi_SetIntByIndex(PMDATA_INF pArgInf, INT index, INT value)
{
	eapi__SetIntByIndex(pArgInf->m_pCompoundData,index,value);
}

void* eapi_GetPointerByIndex(PMDATA_INF pArgInf, INT index)
{
	return eapi__GetPointerByIndex(pArgInf->m_pCompoundData,index);
}

void eapi_SetLogfont_CompileLanVer(LPLOGFONT pLogfont)
{
	::GetObject (::GetStockObject (DEFAULT_GUI_FONT), sizeof (LOGFONT), pLogfont);
    #ifndef __COMPILE_LANG_VER
        #error *** lang.h must be imported!
    #endif
    #if (__COMPILE_LANG_VER == __BIG5_LANG_VER)
        /* �ڷ����Windows��DEFAULT_GUI_FONT�е��������ǡ��¼����w����
           ���������Ǳ���ģ����Ա���ǿ�Ƹ�Ϊ�������w����
           ����Ϊ�����wBIG5���롣Ϊ�˱��ⱻת�������滻�����������ָ�ʽд�������С� */
        _tcscpy (pLogfont->lfFaceName, _T("\xB2\xD3\xA9\xFA\xC5\xE9"));
    #elif (__COMPILE_LANG_VER == __SJIS_JP_LANG_VER)
        /* �����İ�Windows��DEFAULT_GUI_FONT�е��������ǡ�MS UI Gothic����
           ���������Ǳ���ģ����Ա���ǿ�Ƹ�Ϊ��MS Gothic����*/
        pLogfont->lfCharSet = SHIFTJIS_CHARSET;
        _tcscpy (pLogfont->lfFaceName, "MS Gothic");
    #elif (__COMPILE_LANG_VER == __ENGLISH_LANG_VER)
        _tcscpy (pLogfont->lfFaceName, _T("Courier New"));
    #endif
}